# Avancement — ft_irc

Suivi par jalon (cf. [ARCHITECTURE.md](ARCHITECTURE.md) §9). Cocher au fur et à mesure.

Légende : `[ ]` à faire · `[~]` en cours · `[x]` fait

> **Briques de base prêtes (2026-06-18, branche `client-channel`) :** classes `Client` et `Channel` implémentées (buffers, `extractLine` `\r\n`, membres/ops/invités, `canJoin`, `broadcast`). Pas un jalon en soi — consommées par les Jalons 1/3/4/5. Cf. [WORKLOG](WORKLOG.md).

---

## Jalon 0 — Squelette qui compile *(hors liste officielle, préalable)*
- [x] Tous les headers + .cpp compilent sous `-Wall -Wextra -Werror -std=c++98`
- [x] Makefile liste les 11 sources, pas de relink inutile (`all`/`clean`/`fclean`/`re`)
- [x] Signatures conformes à la spec, corps vides

## Jalon 1 — Tuyauterie réseau seule (echo, zéro IRC) — *fait*
- [x] `main` parse `<port> <password>` (+ check `argc`), instancie le `Server`, pose `_serverName`
- [~] `Server` : `socket` + `bind` + `listen` + fd **non-bloquant** (`fcntl O_NONBLOCK`) faits ; **reste `SO_REUSEADDR`**
- [x] `PollMultiplexer` : `watch`/`unwatch`/`setWriteInterest`/`wait` utilisés par `run()`
- [x] `Server::run()` : `poll` + `acceptClient` (accept `-1` → retry, pas de crash) + `recv`→`appendInput`→`extractLine`→`Parser`→`_dispatcher` branchés
- [x] `recv()<=0` → `disconnectClient` ; write path OK (flush `_outBuffer`, send partiel géré) ; POLLOUT (ré)armé pour tout client avec sortie en attente (broadcast inclus)
- [~] Testé `nc` (register / JOIN / PART / erreurs / 2 clients) ; **fragmentation 2 paquets pas encore testée**

## Jalon 2 — Squelette OOP + dispatch
- [~] `Parser::parse()` renvoie un vrai `Message` *(fonctionne via `parseRawMessage` ; pas renommé, `std::cout` de debug à retirer)*
- [x] `CommandDispatcher` : `registerCommand` + `dispatch` (vérifs `requiresRegistration`/`minParams`, try/catch, lookup case-insensitive `Utils::toLower`)
- [x] Hiérarchie d'exceptions : `FatalException`/`ACommandError` + dérivées, **câblées** dans `dispatch` (421/451/461/462/464/433/431/432) ; câblage `main`/`FatalException` en attente de `run()`
- [x] Une commande bidon (`random`) enregistrée — routage validé bout-en-bout (harness standalone)

## Jalon 3 — Enregistrement *(testable e2e : `run()` dispatch)*
- [x] PASS / NICK / USER (états `CONNECTING → PASSWORD_OK → REGISTERED`, burst 001-004) + **validation nick RFC 2812** (431/432/433) — vérifié e2e via `nc`
- [ ] PING / PONG *(prochain : sans ça les vrais clients timeout)*
- [~] Un vrai client (irssi/HexChat) peut se connecter *(`run()` OK ; testé `nc`, pas encore irssi ; bloqué par PING/PONG)*

## Jalon 4 — Channels & messagerie
- [x] JOIN / PART (enregistrés, broadcast OK, RPL 331/332/353/366) — vérifié e2e ; **reste** : numerics de refus 471/473/475 (avec MODE) + consommation de l'invitation
- [ ] PRIVMSG (user + channel)
- [ ] QUIT (broadcast + retrait propre des channels)
- [ ] TOPIC
- [x] `addClientToChannel` / `removeClientFromChannel` (synchro des deux côtés) + `removeClientFromAllChannels`, suppression des channels vides

## Jalon 5 — Opérateurs
- [ ] INVITE
- [ ] KICK
- [ ] MODE : `+i`, `+t`, `+k`, `+o`, `+l` (le plus retors — en dernier)

## Jalon 6 — Durcissement
- [ ] Déconnexions brutales (Ctrl+D, kill) sans crash
- [ ] Résistance au flood
- [ ] `valgrind` : zéro fuite, à la déconnexion et à l'arrêt
- [ ] Relecture des cas limites du sujet
