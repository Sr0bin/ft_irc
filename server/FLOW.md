# FLOW — aide-mémoire logique serveur

Memo compact des boucles logiques, pour référence rapide. Détails : [ARCHITECTURE.md](ARCHITECTURE.md) §6/§7, [EXCEPTIONS.md](EXCEPTIONS.md) §3. Version visuelle complète : `ft_irc_reference.html`.

## Boot
`argv (==3)` → `Config` → **Server** : `socket` → `setsockopt(SO_REUSEADDR)` → `fcntl(O_NONBLOCK)` → `bind` → `listen` → `mux.watch(listenFd, POLLIN)` → `run()`.
Échec init ⇒ `throw FatalException` → `main` catch → `exit(1)`.

## Event loop — `Server::run()`, un seul `poll()`
`mux.wait(events)` [dort] → pour chaque Event :
- `listenFd` → `acceptClient()` (new Client, fd non-bloquant, `mux.watch`)
- `POLLHUP/POLLERR` → `disconnectClient(fd)`
- `POLLIN` → recv (→ read path)
- `POLLOUT` → send (→ write path)

## Read path
`recv` : `==0` → disconnect · `<0` (EAGAIN) → skip · `>0` → `Client.appendInput` (→ `_inBuffer`).
Puis boucle : `_inBuffer` contient CRLF ? **non** → garder (fragmentation TCP) · **oui** → `extractLine` → `Parser::parse` → `Message{prefix,command,params}` → `dispatch` → reboucle (draine plusieurs commandes d'un même recv).

## Dispatch — `CommandDispatcher::dispatch()`
`_commands.find(command)` absent ⇒ `throw UnknownCommand (421)`.
Pré-checks : `requiresRegistration() && !registered` ⇒ `451` · `params < minParams()` ⇒ `461`.
`cmd.execute()` : refus métier ⇒ `throw NoSuchChannel 403 / BadChannelKey 475 / ChanOPrivsNeeded 482…` · succès ⇒ mutation + broadcast.
`catch(ACommandError&)` → `queueReply(e.toMessage(serverName, nick).serialize())` (nick = `*` si pas enregistré).
`catch(std::exception&)` → log & continue (**never crash**).

## Write path
`queueReply` / `Channel.broadcast` → `_outBuffer` des destinataires.
`hasPendingOutput()` ⇒ `setWriteInterest(fd, true)`.
`POLLOUT` → `send` : **partiel** (buffer noyau plein) → retirer seulement les n octets partis, retry au prochain POLLOUT · **complet** → `_outBuffer` vide → `setWriteInterest(fd, false)` (sinon poll() spin = 100% CPU).

## Disconnect — ordre impératif
1. retirer de tous ses `_channels` → 2. broadcast `QUIT` → 3. `close(fd)` → 4. `mux.unwatch(fd)` → 5. `delete`.
Inverser ⇒ pointeur pendant dans les `_members` → crash.

## Registration
`_state = HANDSHAKE` : `PASS` (vs password ; faux → 464) · `NICK` (`_nickname` ; déjà pris → 433) · `USER` (`_username`, `_realname`).
PASS + NICK + USER reçus ⇒ `_state = REGISTERED` → welcome `001/002/003/004`.

## Invariants
- **Never crash** : 2 seuls sites de catch — `main` (`FatalException`) et `dispatch` (`ACommandError` puis `std::exception`).
- **Fragmentation TCP** : `appendInput` accumule ≠ `extractLine` ne libère que sur CRLF.
- **Envoi partiel** : `_outBuffer` persistant, `POLLOUT` à la demande seulement.
- 1 seul `poll()`, tous les fds non-bloquants, aucune I/O directe depuis les commandes.

## Qui fait quoi (classe par étape)
| Classe | Responsabilité dans les boucles |
|---|---|
| `main` | valide argv, build `Config`, catch `FatalException` |
| `Server` | setup socket, boucle `run()`, accept/disconnect, recv/send |
| `AMultiplexer`/`PollMultiplexer` | `watch`/`unwatch`/`setWriteInterest`/`wait` |
| `Client` | `_inBuffer`/`_outBuffer`, `appendInput`/`extractLine`/`queueReply`, `_state`/`_nickname` |
| `Parser` | `parse` → `Message` (syntaxe seule) |
| `Message` | `command`/`params`/`_trailing` (donnée) ; `serialize()` = format fil sortant |
| `CommandDispatcher` | lookup, pré-checks, catch, sérialise la reply d'erreur |
| `ACommand` | `execute`, mutation d'état |
| `ACommandError` | porte le numeric (`toMessage` → `Message::numeric`) |
| `Channel` | `_members`/`_operators`, `broadcast`/`canJoin` |
