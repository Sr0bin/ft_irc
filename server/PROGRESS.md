# Avancement — ft_irc

Suivi par jalon (cf. [ARCHITECTURE.md](ARCHITECTURE.md) §9). Cocher au fur et à mesure.

Légende : `[ ]` à faire · `[~]` en cours · `[x]` fait

> **Briques de base prêtes (2026-06-18, branche `client-channel`) :** classes `Client` et `Channel` implémentées (buffers, `extractLine` `\r\n`, membres/ops/invités, `canJoin`, `broadcast`). Pas un jalon en soi — consommées par les Jalons 1/3/4/5. Cf. [WORKLOG](WORKLOG.md).

---

## Jalon 0 — Squelette qui compile *(hors liste officielle, préalable)*
- [x] Tous les headers + .cpp compilent sous `-Wall -Wextra -Werror -std=c++98`
- [x] Makefile liste les 11 sources, pas de relink inutile (`all`/`clean`/`fclean`/`re`)
- [x] Signatures conformes à la spec, corps vides

## Jalon 1 — Tuyauterie réseau seule (echo, zéro IRC) — *partiel (PA), à finir*
- [ ] `main` parse `<port> <password>`, instancie le `Server` *(main = scratch ; `_serverName` pas posé)*
- [~] `Server` : `socket` + `bind` + `listen` faits ; **manquent `SO_REUSEADDR` + fd non-bloquant (`fcntl`)**
- [~] `PollMultiplexer` : `watch`/`unwatch`/`wait` utilisés par `run()` (internes non audités)
- [~] `Server::run()` : boucle `poll` + `acceptClient` + `recv`→`appendInput` OK ; **input jamais dispatché** (`extractLine`/Parser/`_dispatcher` non branchés, `_dispatcher` NULL)
- [~] `recv()<=0` → `disconnectClient` OK ; **write path cassé** (envoie un buffer non initialisé, ne flush pas `_outBuffer`) ; send partiel non géré
- [ ] Testé avec `nc`, y compris commande fragmentée en deux paquets

## Jalon 2 — Squelette OOP + dispatch
- [~] `Parser::parse()` renvoie un vrai `Message` *(fonctionne via `parseRawMessage` ; pas renommé, `std::cout` de debug à retirer)*
- [x] `CommandDispatcher` : `registerCommand` + `dispatch` (vérifs `requiresRegistration`/`minParams`, try/catch, lookup case-insensitive `Utils::toLower`)
- [x] Hiérarchie d'exceptions : `FatalException`/`ACommandError` + dérivées, **câblées** dans `dispatch` (421/451/461/462/464/433/431/432) ; câblage `main`/`FatalException` en attente de `run()`
- [x] Une commande bidon (`random`) enregistrée — routage validé bout-en-bout (harness standalone)

## Jalon 3 — Enregistrement *(couche commandes faite ; intestable e2e tant que `run()` ne dispatch pas)*
- [~] PASS / NICK / USER (états `CONNECTING → PASSWORD_OK → REGISTERED`, burst 001-004) + **validation nick RFC 2812** (431/432/433)
- [ ] PING / PONG
- [ ] Un vrai client (irssi/HexChat) peut se connecter *(bloqué par `run()`)*

## Jalon 4 — Channels & messagerie
- [ ] JOIN / PART *(registre `Server` prêt ; reste la commande : policy `canJoin` + numerics)*
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
