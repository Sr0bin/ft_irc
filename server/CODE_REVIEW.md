# Code Review — ft_irc (audit qualité / archi / edge cases)

Date : 2026-06-25. Audit mené par 4 agents en parallèle (transport/core,
framework commandes, commandes registration/base, commandes op/messaging) +
scan mécanique tree-wide. Périmètre : **mandatory only**. Destiné à l'équipe
(prigaudi / vpirotti).

Axes : (1) conformité archi, (2) élégance / code propre, (3) edge cases.

---

## Synthèse — ce qui est sain (aucune action)

Le fond du serveur est **solide** et l'archi est **respectée** :

- **Un seul `poll()`**, confiné à `PollMultiplexer::wait`. Fds non-bloquants sur le
  listen et l'accept. `EMFILE`/`ECONNABORTED`/`EAGAIN` gérés sans crash.
- **Aucune I/O directe depuis la couche commande** : tout passe par
  `Client::queueReply` + le framework `Message`. Vérifié mécaniquement (zéro
  `cout`/`cerr`/`write`/`send` hors Utils/main/Server/Multiplexer).
- **Lifetime des `Client*` propre** : `disconnectClient` retire le client de tous
  les sets de channel (members/operators/invited) **avant** `delete` ; itération sur
  copie ; channels vides détruits. Pas de dangling pointer.
- **Pas de use-after-free sur QUIT** : `disconnectClient` est la dernière action
  d'`execute`, et `handleReadable` re-teste `_clients.find(fd)` avant de réutiliser
  le pointeur.
- **Trailing-colon correct partout** (TOPIC / KICK / PRIVMSG / QUIT / PART) — le
  contenu multi-mots survit.
- **Hiérarchie d'exceptions** saine : `what()` renvoie un membre (pas un temporaire),
  pas de slicing (catch par référence), ordre des catch correct
  (`FatalException` → `ACommandError` → `std::exception`).
- **C++98 strict** : zéro `nullptr`/`auto`/`override`/range-for ; zéro fonction libc
  interdite (`getenv`/`system`/`sprintf`/`strcpy`…). Build `-Wall -Wextra -Werror`
  vert, **valgrind 0 leak / 0 erreur / fds clean**.

---

## 0. Nettoyages appliqués (sûrs, dans ce commit)

> Build re-vérifié vert + valgrind clean après application. À review dans le diff.

| # | Fichier | Changement |
|---|---------|-----------|
| C1 | `src/main.cpp` | Suppression du `main()` de test commenté (45 lignes mortes) |
| C2 | `include/ft_irc.hpp` | Suppression du bloc `/*A SUPPRIMER, MEMO PA …*/` |
| C3 | `src/Message.cpp` | `#include "../include/Message.hpp"` → `"Message.hpp"` (cohérence, `-I` déjà posé) |
| C4 | `src/commands/CommandDispatcher.cpp` | TODO périmé/trompeur supprimé (le rethrow `FatalException` existe déjà au-dessus) |
| C5 | `Privmsg` + `ACommandError` | `NoRecipient` (411) **câblé** dans PRIVMSG : `minParams`→0, bare PRIVMSG = 411, cible sans texte = 412 (avant : tout en 461) |
| C6 | `Invite`/`Privmsg` `.{hpp,cpp}` | Suppression des surcharges `requiresRegistration(){return true;}` redondantes (héritent du `true` de base, comme Join/Part/Topic/Kick/Mode) |
| C7 | Join / ACommand / Privmsg / Invite `.cpp` | Includes morts retirés (`<iostream>`, `<ostream>`, `<stdexcept>`, `<sys/socket.h>`) |
| C8 | `src/commands/PrivmsgCommand.cpp` | **Fix UB** : `getParam(0)[0]` sur string vide → guard `.empty()` |
| C9 | `src/Server.cpp` | `struct sockaddr_in x = {};` (C++11) → `std::memset` (conformance C++98 stricte) |
| C10 | `Channel.{hpp,cpp}` | `setTopic(std::string&)` → `setTopic(const std::string&)` |
| C11 | `Join`/`Part` `.cpp` | `const std::string &x = getParam(0)` (ref sur temporaire) → copie explicite |
| C12 | `include/commands/JoinCommand.hpp` | `sendTopic`/`sendNames` passés en `private` |
| C13 | `src/PollMultiplexer.cpp` | `POLLNVAL` ajouté au masque `closed` (anti busy-loop défensif) |

---

## 1. À décider — touche le design partagé ou la conformité RFC

> Pas appliqué unilatéralement : ça touche le code/les choix de prigaudi/vpirotti
> ou ça dépasse le scope sujet. À trancher en équipe.

### [Majeur] Doublon `ACommandReply` / `ACommandError`
- **Axe** : Élégance. `ACommandReply.{hpp,cpp}` et `ACommandError.{hpp,cpp}` sont
  structurellement identiques (mêmes 3 ctors, mêmes membres `code/params/text`,
  `toMessage` byte-for-byte identique). Seule diff : `ACommandError` hérite
  d'`IrcException` (donc throwable). Beaucoup de copier-coller pour un seul edge
  d'héritage. + `ACommandError` stocke le texte 2× (base `_msg` pour `what()` ET
  `_text`).
- **Piste** : extraire un porteur unique (code/params/text + `toMessage`) ;
  `ACommandError = IrcException + ce porteur`. **Touche le code de prigaudi → à
  discuter, pas refacto unilatérale.**

### [Majeur] MODE : mutation d'état partielle si la cible `+o` est invalide — ✅ CORRIGÉ
- **Axe** : EdgeCase. `ModeCommand.cpp` (~60-71, 134-160). `applyOne` jette
  `UserNotInChannel` (441) **en plein milieu** de la boucle d'application. Sur
  `MODE #c +ti +o ghost`, `+t` et `+i` sont déjà committés quand le throw part : le
  client reçoit 441 **sans aucun écho MODE**, et l'état du channel diverge
  silencieusement de ce qui a été annoncé.
- **Piste** : résoudre les cibles `+o/-o` pendant `parseModeChange` (pré-passe), pour
  qu'un nick invalide annule **avant** toute mutation.

### [Mineur] MODE query (324) expose la clé du channel — ✅ CORRIGÉ (clé masquée)
- **Axe** : EdgeCase. `ModeCommand.cpp` (~76-100). `currentModeString` colle
  `k <key>` et `l <limit>` dans le trailing de `RPL_CHANNELMODEIS`. (a) pas d'op-check
  sur la query → n'importe quel membre apprend la clé `+k` (les vrais ircd la
  cachent) ; (b) key/limit devraient être des params discrets, pas collés dans le
  trailing.
- **Piste** : sur la query, émettre le flag `k` sans la valeur (ou `*`).

### [Mineur] INVITE : `ERR_USERONCHANNEL` (443) omet le nick cible — ✅ CORRIGÉ (Invite + Join)
- **Axe** : EdgeCase. `UserOnChannel` ne prend qu'un param et reçoit le **channel**.
  Format RFC 443 = `<nick> <channel> :is already on channel`. Le nick invité
  n'apparaît jamais. (Note : `UserOnChannel` est aussi throw dans `JoinCommand:31`,
  usage douteux en JOIN où la RFC dit plutôt d'ignorer — à revoir ensemble, c'est
  pour ça que je ne l'ai pas corrigé seul : ça touche 2 commandes.)
- **Piste** : `UserOnChannel(nick, channel)` (mirror de `UserNotInChannel`), corriger
  les 2 call-sites.

### [Mineur] `prefix()` hardcode `@localhost` — ✅ CORRIGÉ (host via inet_ntoa à l'accept)
- **Axe** : EdgeCase RFC. `Client.cpp:79` — le préfixe des messages relayés utilise
  `@localhost` au lieu du vrai host. Déjà marqué `ponytail:` (capter via
  `getpeername` au moment de l'accept). Cosmétique côté clients mais non-conforme.

### [Mineur] PING : pas de `409 ERR_NOORIGIN` ; autorisé avant registration
- **Axe** : EdgeCase + Archi. `PingCommand` : `minParams()==0` → un PING sans token
  répond silencieusement (RFC : 409). Et `requiresRegistration()==false`, alors que
  le reste impose registration sauf PASS/NICK/USER. À confirmer : intentionnel ?

### [Mineur] JOIN/PART : pas de validation fine ni de listes `#a,#b`
- **Axe** : EdgeCase (hors scope sujet probablement). `JoinCommand.cpp:21` ne rejette
  que `[0] != '#'` → un nom avec `,` (séparateur de liste IRC) crée un channel nommé
  littéralement `#a,#b`. Les listes séparées par virgule (`JOIN #a,#b`) ne sont pas
  gérées. **Si volontairement skippé, le documenter.**

### [Mineur] `Server.hpp` : includes inutiles dans le header
- **Axe** : Élégance. `Server.hpp` inclut `PollMultiplexer/Parser/Utils/IrcException`,
  aucun requis par le header (tous utilisés seulement dans le `.cpp`). Pollue chaque
  TU qui include `Server.hpp`. **Pas appliqué** : retirer des includes d'un header
  partagé la veille du rendu peut cascader — à faire à tête reposée.

### [Mineur] Constructeurs par défaut inutilisés laissent des POD non-initialisés — ✅ CORRIGÉ (supprimés)
- **Axe** : EdgeCase. `Server()`, `Client()`, `Channel()` ne sont jamais utilisés
  (toujours construits avec args). `Server()` en particulier laisse
  `_config._port/_listenFd` non-initialisés. À supprimer, ou justifier si la norme
  Coplien les impose.

---

## 2. Nits (cosmétique, non appliqués pour éviter le bruit de diff)

- **else après `throw`** (mort) dans `TopicCommand.cpp:38`, `ModeCommand.cpp` ;
  corps de ctor/dtor vides paddés de lignes blanches (`ModeCommand`).
- **Duplication** du pattern `if (!ch->isOperator(client)) throw ChanOPrivsNeeded`
  (4 commandes) et du couple « broadcast-all-but-sender puis queueReply au sender »
  (TOPIC, MODE) ; boucle de dedupe broadcast réimplémentée dans NICK/QUIT/PRIVMSG.
  → un `requireOperator(ch, client)` + `Server::broadcastToPeers(...)` retireraient
  la répétition. **Touche le code partagé → à discuter.**
- **TOPIC** : sur set réussi, le setter reçoit `332` au lieu de l'écho `TOPIC` (les
  autres membres reçoivent l'écho). Asymétrique, inoffensif.
- ~~**KICK** : motif par défaut = nick du kické~~ — ✅ CORRIGÉ (défaut = kicker).
- **PingCommand** (vpirotti) : includes relatifs `../../include/...` + redondants
  (Client/Message déjà via ACommand) + style d'accolades `{return 0;}` divergent du
  reste. Cosmétique — laissé tel quel (fichier d'un coéquipier).
- `Message::operator<<` et `IrcException::operator=` : non utilisés (aides debug /
  habitude Coplien). Inoffensifs.
- `PollMultiplexer.hpp` indenté en 2-espaces vs tabs ailleurs.
- Parser : limite RFC des **15 params** non appliquée (les middles en trop ne sont
  pas repliés dans le trailing). Inoffensif ici (lecture par index), à noter si on
  veut la stricte conformité.

---

## Note hors-code : docs de référence sur une autre branche

`CLAUDE.md` (projet) pointe vers `ARCHITECTURE.md`, `DECISIONS.md`, `PROGRESS.md`,
`FLOW.md` et `ft_irc_reference.html`. Ces fichiers **existent mais vivent sur la
branche `doc`**, pas sur `main` — d'où leur absence du working tree quand on bosse
sur `main` (les agents d'audit les ont crus disparus pour ça). **Pas de perte de
données.** Friction mineure : sur `main`, les liens de `CLAUDE.md` ne résolvent pas.
Au choix : merger `doc`, ou ajouter une ligne dans `CLAUDE.md` rappelant que la doc
de référence est sur `doc`.
