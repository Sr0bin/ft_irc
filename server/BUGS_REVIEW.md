# ft_irc — Revue de code & campagne de test (2026-06-25)

Revue géante pré-rendu : tests dynamiques (clients python/nc scriptés, valgrind,
stress) + revue statique (C++98, mémoire, RFC 1459/2812). Périmètre **mandatory**.

Chaque bug est **reproduit** (sauf mention « statique »).

---

## 0. Statut des corrections (session fix du 2026-06-25)

Scope appliqué : **« à partir de #3 vers le bas », sans ajout de feature non-mandatory.**

| Bug | Statut | Note |
|---|---|---|
| #1 PRIVMSG trailing | ✅ corrigé | par l'équipe (rorollin), `, true` lignes 43/61 |
| #3 QUIT format + dedupe | ✅ corrigé | param canal retiré, `trailing=true`, un seul envoi par peer |
| #4 NICK broadcast | ✅ corrigé | relai `:old NICK new` au client + membres des canaux (dedupe) |
| #7 412 mauvais param | ✅ corrigé | `NoTextToSend("PRIVMSG")` |
| #8 341 `:` parasite | ✅ corrigé | `Message::numeric` direct dans InviteCommand ; `RplInviting` (code mort) supprimé |
| #10 fcntl non vérifié | ✅ corrigé | listen → FatalException ; client → close & skip |
| #12 pollution stdout | ✅ corrigé | cout `completeRegistration` retiré |
| #13 commande debug `random` | ✅ retirée | dispatcher + Makefile + fichiers supprimés |
| #14 KICK `>= 2` | ✅ corrigé | `>= 3` |
| #15 FatalException avalée | ✅ corrigé | `catch (FatalException&){throw;}` avant le filet |
| #2 TOPIC trailing | ✅ corrigé | par l'équipe, `, true` dans TopicCommand |
| #5 JOIN/PART multi-canaux | ⏭️ non fait | feature non-mandatory |
| #6 NOTICE | ⏭️ non fait | feature non-mandatory |
| #9 PING préfixe serveur | ✅ corrigé | `:server PONG server :token` via le framework Message |
| #11 `_inBuffer` non borné | ✅ corrigé | plafond 512 (RFC) : ligne sans CRLF trop longue jetée, client gardé connecté |
| #16 classe 482 dupliquée | ✅ corrigé | `NotChanOp` supprimée, `ChanOPrivsNeeded` partout |
| #17 `#pragma once` | ✅ corrigé | include guards dans les 4 headers restants |

**Après fixes** : build vert `-Wall -Wextra -Werror -std=c++98`, valgrind `0 leak / 0 error`,
fds propres, stdout vide, suite fonctionnelle sans régression. Détails par bug ci-dessous.

---

## 1. Synthèse — preuve de fonctionnement

| Domaine | Statut | Détail |
|---|---|---|
| Build `-Wall -Wextra -Werror -std=c++98` | ✅ | vert, zéro warning |
| **valgrind** (stress + QUIT + KICK + déco brutes + 40 clients) | ✅ | `0 bytes in use at exit`, `All heap blocks freed`, **0 errors** |
| **Fuites de fd** | ✅ | `FILE DESCRIPTORS: 3 open (3 std) at exit` |
| Un seul `poll()` | ✅ | `PollMultiplexer.cpp:52` uniquement |
| fds non-bloquants, `SO_REUSEADDR`, accept non-bloquant | ✅ | statique + restart rapide OK |
| **Données fragmentées** (NICK envoyé octet par octet via nc) | ✅ | réassemblé → 001 |
| Fonctions interdites (`fork`/`getenv`/`time`/`read`/`write`…) | ✅ | aucune ; I/O socket = `recv`/`send` |
| C++11+ | ✅ | aucun construct détecté |
| Arrêt propre `SIGINT`/`SIGTERM` | ✅ | exit 0, teardown loggé |
| Registration PASS/NICK/USER + numerics (464/451/433/432/462/431) | ✅ | + welcome 001-004 |
| JOIN / PART / TOPIC(query) / modèle op / destroy canal vide | ✅ | |
| MODE `i t k o l`, `+o`/`-o`, 324/472/482/441/461 | ✅ | écho du delta réel |
| KICK (format + raison + 482) / INVITE (`+i` flow, 341/473/443) | ✅ | |
| PRIVMSG routing (401/403/404/412) / PING-PONG / broadcast sender exclu | ✅ | (contenu : voir bug #1) |
| Stress 40 clients + broadcast | ✅ | 39/39 peers reçoivent |

**Aucun crash, aucune fuite, aucun fd perdu** sur toute la campagne. La base réseau /
mémoire / robustesse est solide. Les bugs ci-dessous sont **applicatifs** (formatage
de messages et quelques gaps RFC), pas structurels.

---

## 2. Bugs — par sévérité

### 🔴 CRITIQUE

#### #1 — PRIVMSG perd le `:` du trailing → messages multi-mots cassés
- **Fichier** : `src/commands/PrivmsgCommand.cpp:43` (user) et `:61` (canal)
- **Repro** : `PRIVMSG #chat :multi word channel message`
- **Attendu** : `:msgr!msgr@localhost PRIVMSG #chat :multi word channel message`
- **Obtenu** : `:msgr!msgr@localhost PRIVMSG #chat multi word channel message`
- **Impact** : le client receveur découpe sur les espaces → seul le 1er mot survit comme
  texte. **Le cœur du chat est cassé pour tout message de plus d'un mot.** C'est le bug
  le plus important du projet.
- **Fix** : ajouter `true` (flag trailing) au constructeur, sur les deux lignes :
  `Message(client.prefix(), "PRIVMSG", p, true).serialize();`

### 🟠 HAUTE (écarts RFC visibles)

#### #2 — TOPIC (broadcast) perd le `:` du trailing
- **Fichier** : `src/commands/TopicCommand.cpp:44-45`
- **Repro** : `TOPIC #t :a long multi word topic` → ce que voit un autre membre :
- **Obtenu** : `:ta!ta@localhost TOPIC #t a long multi word topic` (sans `:`)
- **Note** : le **332 query** (lecture du topic) est correct ; seul le **relai** est cassé.
- **Fix** : `Message(client.prefix(), "TOPIC", p, true)`

#### #3 — QUIT mal formé : param canal en trop + `:` manquant (+ doublon)
- **Fichier** : `src/commands/QuitCommand.cpp:40-42`
- **Repro** : `QUIT :leaving now` → ce que voit un peer :
- **Obtenu** : `:qa!qa@localhost QUIT #q leaving now`
- **Attendu** : `:qa!qa@localhost QUIT :leaving now`
- **Double bug** : (a) `p.push_back(channel->getName())` injecte le nom du canal — QUIT
  n'en prend pas ; (b) `trailing` absent. **(c)** la boucle rediffuse **par canal
  partagé** : un peer sur 2 canaux communs reçoit 2 QUIT (RFC : un seul).
- **Fix** : `p = {reason}; Message(client.prefix(), "QUIT", p, true);` et dédoublonner
  les destinataires (set de `Client*` accumulé puis envoi unique).

#### #4 — NICK (changement mid-session) jamais rediffusé
- **Fichier** : `src/commands/NickCommand.cpp:49` (fait juste `setNickName`)
- **Repro** : client enregistré dans `#r` fait `NICK gamma`
- **Obtenu** : aucun écho au client, **aucun** `:oldnick NICK gamma` aux membres du canal
- **Impact** : les autres ne savent jamais que le nick a changé ; leur état devient
  périmé, un KICK/PRIVMSG vers l'ancien nick échoue. (À l'enregistrement initial, pas de
  broadcast nécessaire — le bug ne concerne que le rename en session.)
- **Fix** : construire `Message(oldPrefix, "NICK", {newNick}, true)` et le diffuser au
  client + à tous les membres des canaux du client.

### 🟡 MOYENNE

#### #5 — JOIN / PART : liste multi-canaux séparée par virgule non gérée
- **Fichier** : `src/commands/JoinCommand.cpp:16`, `PartCommand.cpp:12` (statique + repro)
- **Repro** : `JOIN #x,#y` → crée **un** canal littéralement nommé `#x,#y`
- **Attendu RFC** : rejoindre `#x` et `#y` (clés : `JOIN #a,#b k1,k2`)
- **Impact** : beaucoup de clients envoient des JOIN unitaires, donc impact réel limité,
  mais un évaluateur peut tester `JOIN #a,#b`. À trancher selon le temps restant.

#### #6 — NOTICE non implémentée
- **Repro** : `NOTICE bob :hi` → `:server 421 ... NOTICE :Unknown command`
- **Note** : NOTICE n'est pas strictement exigée par le sujet, mais les clients de
  référence (weechat/irssi) l'utilisent. Triviale à ajouter (= PRIVMSG sans réponses
  d'erreur). À décider.

#### #7 — 412 NoTextToSend : mauvais paramètre
- **Fichier** : `src/commands/PrivmsgCommand.cpp:36,54` (statique, corroboré dynamiquement)
- **Obtenu** : `:server 412 newalice  :No text to send` (champ vide → double espace)
- **Attendu** : `:server 412 newalice PRIVMSG :No text to send`
- **Cause** : `throw NoTextToSend(content)` passe le corps (vide) au lieu de `"PRIVMSG"`.
- **Fix** : `throw NoTextToSend("PRIVMSG");`

#### #8 — 341 RplInviting : `:` parasite sur le canal
- **Fichier** : `src/commands/ACommandReply.cpp:56-57` (le canal part en `_text`)
- **Obtenu** : `:ircserv.42.fr 341 ia ib :#i`
- **Attendu RFC 2812** : `341 ia ib #i` (le canal est un middle param, sans `:`)
- **Fix** : passer le canal dans `_params`, pas dans `_text`.

#### #9 — PING : réponse sans préfixe serveur
- **Fichier** : `src/commands/PingCommand.cpp:27-28` (statique, corroboré : `PONG :tok`)
- **Obtenu** : `PONG :token`
- **Attendu RFC** : `:server PONG server :token`
- **Impact** : la plupart des clients tolèrent, mais les stricts peuvent ne pas valider
  le keep-alive. Fix simple (préfixer avec `_server.getServerName()`).

### 🟢 BASSE / robustesse / nettoyage

#### #10 — `fcntl` return non vérifié → risque de socket bloquante
- `src/Server.cpp:89` (listen) et `:219` (client). Si `O_NONBLOCK` échoue, la socket
  reste bloquante → le prochain `recv`/`send` **gèle toute la boucle**. Throw
  `FatalException` sur `== -1`. (statique — non déclenché en test)

#### #11 — `_inBuffer` non borné → DoS mémoire
- `src/Client.cpp:23`. Un client qui envoie un flux sans jamais de `\r\n` fait croître le
  buffer jusqu'à l'OOM. Cap (p.ex. 512 o / message IRC) + déconnexion si dépassé.
  (statique — la robustesse testée n'inclut pas ce cas adverse précis)

#### #12 — Pollution stdout (debug oublié)
- `src/commands/ACommand.cpp:26` : `std::cout << "completeRegistration"` à **chaque**
  enregistrement (201 lignes pendant le stress). `src/commands/randomCmd.cpp:32` :
  `std::cout << "test randomCmd"`. **À retirer** (sortie standard doit rester propre).

#### #13 — Commande debug `random` exposée aux clients
- `CommandDispatcher.cpp:37` enregistre `random` (`randomCmd.cpp`, marqué « TESTING
  ONLY »). N'importe quel client peut l'appeler. **Retirer** l'enregistrement + le
  fichier du Makefile avant le rendu.

#### #14 — KickCommand : condition `>= 2` au lieu de `>= 3`
- `KickCommand.cpp:66` : `if (msg.paramCount() >= 2) reason = msg.getParam(2);`.
  Inoffensif (`getParam(2)` renvoie `""` hors borne) mais trompeur. Mettre `>= 3`.

#### #15 — FatalException avalée par le catch-all du dispatcher
- `CommandDispatcher.cpp:88`. Le TODO du fichier le note déjà. Ajouter
  `catch (FatalException &) { throw; }` avant le `catch (std::exception &)`. (théorique)

#### #16 — Classe 482 dupliquée
- `ChanOPrivsNeeded` (ModeCommand) et `NotChanOp` (KICK/INVITE) = même code 482, même
  texte. En supprimer une.

#### #17 — `#pragma once` dans 5 headers
- `InviteCommand/PrivmsgCommand/randomCmd/QuitCommand/PingCommand.hpp`. Compile sans
  warning chez gcc/clang (pas de norminette en C++), mais incohérent avec les autres
  headers (include guards). Cosmétique — uniformiser si le temps le permet.

#### #18 — Divers (info, statique)
- `RplInviting` sans spécif `throw()` (incohérent avec la hiérarchie).
- JOIN d'un canal déjà rejoint renvoie 443 (RFC ne l'exige pas ; cosmétique).
- `CommandDispatcher` : fuite partielle si un `new XCommand` throw en plein ctor (les
  ctors ne throwent pas → théorique).
- `PollMultiplexer.cpp:52` : `_fds[0]` sûr par invariant (listen fd toujours surveillé),
  pas de garde explicite.

---

## 3. Recommandation de priorisation (rendu demain)

1. **Avant tout** : #1 (PRIVMSG), #2 (TOPIC), #3 (QUIT) — 3 one-liners `trailing=true`
   (+ retrait du param canal QUIT). Ce sont des bugs visibles immédiatement par un
   correcteur avec un vrai client.
2. **Ensuite** : #12 + #13 (nettoyage stdout / commande debug) — rapide et « propre ».
3. **Si temps** : #4 (NICK broadcast), #7/#8/#9 (numerics), #5 (multi-canaux).
4. Le reste (#10–#18) : robustesse/cosmétique, non bloquant pour le rendu.

Les #1/#2/#3 touchent du code de prigaudi/vpirotti → à appliquer avec eux.
