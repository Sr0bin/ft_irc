# ft_irc — architecture & contexte projet

Serveur IRC en **C++98**, conforme au sujet de l'École 42. Ce document fixe l'architecture, les conventions et les contraintes. **À lire avant toute génération de code.** Quand un choix d'implémentation n'est pas couvert ici, le signaler plutôt que d'improviser une entorse aux principes ci-dessous.

> Les décisions d'implémentation prises en cours de route (et leur *pourquoi*) sont consignées dans [DECISIONS.md](DECISIONS.md). La gestion des erreurs est détaillée dans [EXCEPTIONS.md](EXCEPTIONS.md).

---

## 1. Contraintes impératives (non négociables)

Ces règles viennent du sujet et de l'évaluation. Les enfreindre = projet invalidé.

- **Standard : C++98 strict.** Pas de `std::function`, lambdas, `auto`, smart pointers, `nullptr`, `std::stoi`, range-based for, ni rien de C++11+. La modularité passe par le polymorphisme classique (virtuel pur, héritage).
- **Compilation :** `c++ -Wall -Wextra -Werror -std=c++98`. Aucun warning toléré.
- **Makefile** avec les règles : `all`, `clean`, `fclean`, `re`, et pas de relink inutile.
- **Un seul `poll()`** (ou équivalent : `select`/`epoll`/`kqueue`) pour TOUTES les I/O, accept inclus. C'est le cœur du sujet.
- **Tous les file descriptors sont non-bloquants** (`fcntl(fd, F_SETFL, O_NONBLOCK)`).
- **Aucune lecture/écriture bloquante.** Jamais de `recv`/`send`/`accept` hors d'un événement signalé par `poll()`.
- **Interdit :** `fork()`, threads, et toute forme de multiprocessing/multithreading.
- **Le serveur ne doit jamais crasher** (ni segfault, ni exception non rattrapée), quoi que fasse le client — y compris données partielles, `Ctrl+D`, déconnexion brutale, flood.
- **Zéro fuite mémoire.** Tout `new` a son `delete`, vérifié à la déconnexion et à l'arrêt.
- **Lancement :** `./ircserv <port> <password>`.
- Référence protocole : RFC 1459 / 2812. Client de test de référence : un vrai client IRC (irssi, HexChat) + `nc` pour les tests bas niveau.

---

## 2. Vue d'ensemble — architecture en couches

Trois domaines indépendants, reliés par un flux unidirectionnel d'événements :

```
poll() → [Couche réseau] → [Couche protocole] → [Couche commandes] → mutation de l'état
```

- **Couche réseau** — sockets, `poll()`, accept, recv/send, gestion des fds. Encapsulée derrière `AMultiplexer`. Ne connaît rien d'IRC.
- **Couche protocole** — transforme un flux d'octets en messages structurés. `Parser` → `Message`.
- **Couche commandes** — exécute la logique IRC sur l'état. Command pattern : `ACommand` + une dérivée par commande, dispatch par `CommandDispatcher`.
- **État du serveur** — entités concrètes `Client` et `Channel`, possédées par `Server`.
- **Transversal** — hiérarchie `IrcException`.

Principe directeur : **on abstrait les comportements qui varient, pas les données qui décrivent.** Les seules abstractions justifiées sont `ACommand` (11 comportements différents) et `AMultiplexer` (mécanique réseau interchangeable). `Client`, `Channel`, `Message`, `Event` sont des classes/structs concrets — pas de sous-typage artificiel.

---

## 3. Conventions de code

- **Forme Canonique Orthodoxe (OCF / Coplien)** pour toute classe qui gère des ressources : constructeur par défaut, constructeur de copie, `operator=`, destructeur. Si la copie n'a pas de sens (ex. `Server`, `Client` qui possèdent des fds), déclarer copie et `operator=` **privés et non implémentés** pour les interdire.
- **Nommage :** classes en `PascalCase`, méthodes en `camelCase`, attributs membres préfixés `_` (ex. `_clients`). Classes abstraites préfixées `A` (`ACommand`, `AMultiplexer`).
- **Un header + un .cpp par classe.** Include guards systématiques.
- **Pas de dispatch par `if/else` géant.** Le routage des commandes passe par une `std::map<std::string, ACommand*>`. Ajouter une commande = ajouter une classe + une ligne d'enregistrement, sans toucher au dispatcher.
- **Gestion d'erreurs par exceptions typées** (voir §5), pas de codes de retour disséminés. Les commandes lèvent, le dispatcher rattrape et formate la réponse.
- **Pas d'I/O directe depuis les commandes.** Une commande empile sa réponse dans le `_outBuffer` du `Client` cible via `queueReply()` ; c'est la boucle `poll()` qui envoie quand le fd est écrivable.
- **Const-correctness** partout où c'est pertinent (getters `const`, paramètres `const&`).

---

## 4. Classes — responsabilités et possession

### Règles de possession (source n°1 des segfaults — à respecter absolument)

- **`Server` possède** les `Client*` et les `Channel*` (composition). Il les alloue (`new`) et les détruit (`delete`). Il en est l'unique propriétaire.
- **`Channel` ne possède PAS** ses membres : ses `std::set<Client*>` sont des pointeurs **non-propriétaires** (agrégation) vers des objets possédés par `Server`.
- **Conséquence critique :** à la déconnexion d'un client, le retirer de TOUS ses channels (`_members`, `_operators`, `_invited`) **avant** le `delete`. Sinon, pointeurs pendants → crash.
- **Synchronisation `Client._channels` ↔ `Channel._members` :** ces deux structures se référencent mutuellement. Ne jamais les modifier directement depuis une commande. Passer par une paire de méthodes uniques sur `Server` (ex. `addClientToChannel` / `removeClientFromChannel`) qui écrivent les deux côtés ensemble. Un seul endroit où la cohérence peut casser.

### Server
Orchestrateur. Tient la boucle d'événements.
- `serverConfig _config` (`_port`, `_password`, `_listenFd`, `_serverName`)
- `std::map<int, Client*> _clients` — indexé par fd (car `poll()` parle en fds ; lookup en O(log n)).
- `std::map<std::string, Channel*> _channels` — indexé par nom **en minuscules** (`find(toLower)` en O(log n), cf. D14).
- `AMultiplexer* _mux`, `CommandDispatcher* _dispatcher`
- `run()`, `acceptClient()`, `disconnectClient(fd)`, `getClientByNick(nick)` (scan linéaire, acceptable à cette échelle), `getChannelByName(name)`, `addClientToChannel(c, name)` (crée si absent, 1er = op), `removeClientFromChannel(c, name)`, `removeClientFromAllChannels(c)`, `getServerName()`, `getPassword()` ; `destroyChannel(chan)` privé.

### Client
Représente une connexion. Possède ses buffers.
- `_fd`, `_inBuffer`, `_outBuffer` (deux buffers : entrée à parser, sortie à envoyer)
- `_nickname`, `_username`, `_realname`, `_state` (enum : non enregistré → mdp ok → enregistré)
- `std::set<Channel*> _channels` — channels rejoints (non possédés)
- `appendInput(data)` (accumule les octets reçus), `extractLine(out)` (extrait une ligne complète terminée par `\r\n`, retourne false si incomplète), `queueReply(msg)`, `hasPendingOutput()`, `isRegistered()`, `prefix()` (fabrique `:nick!user@host` qui préfixe tous les messages relayés).

### Channel
État d'un salon.
- `_name`, `_topic`, `_key` (mot de passe, mode +k), `_userLimit` (mode +l), `_inviteOnly` (+i), `_topicRestricted` (+t)
- `std::set<Client*> _members`, `_operators`, `_invited` (tous non-propriétaires)
- Le **seul mode par-utilisateur** dans le périmètre est opérateur (+o) → un `std::set` suffit, pas de map de modes.
- `addMember`, `removeMember`, `isMember`, `isOperator`, `promote`, `demote`, `canJoin(c, key)` (vérifie +i/+k/+l, lève l'exception adéquate si refus), `broadcast(msg, except)` (empile dans le `_outBuffer` de chaque membre sauf l'émetteur).

### Message
La ligne IRC en tant que donnée, dans **les deux sens**. Objet valeur.
- `_prefix`, `_command`, `std::vector<std::string> _params`, `bool _trailing`
- entrée : `getCommand()`, `getParam(i)`, `paramCount()`
- sortie : `serialize()` (réciproque de `Parser::parse` → `[:prefix ]cmd params :trailing\r\n`) + fabrique `static numeric(server, code, nick, middle[, text])`. Point unique de formatage du fil. Cf. D5 (amendé) / D18.

### Parser
Sans état. `static Message parse(const std::string& rawLine)`. Découpe préfixe optionnel / commande / paramètres, gère le trailing (`:` introduit le dernier paramètre qui peut contenir des espaces).

### CommandDispatcher
- `std::map<std::string, ACommand*> _commands`
- `registerCommand(name, cmd)`, `dispatch(client, msg)` : retrouve la commande, vérifie pré-requis (`requiresRegistration()`, `minParams()`), entoure `execute()` d'un `try/catch (ACommandError&)`.

### ACommand (abstrait)
- `Server& _server` (protégé)
- `virtual void execute(Client&, Message&) = 0`
- `virtual size_t minParams() const`, `virtual bool requiresRegistration() const` (défauts surchargeables)
- Une dérivée concrète par commande (voir §8).

### AMultiplexer (abstrait) / PollMultiplexer
Interface réseau découplée de `poll()`.
- `virtual void watch(int fd) = 0` / `unwatch(int fd) = 0` / `setWriteInterest(int fd, bool on) = 0` / `int wait(std::vector<Event>& out) = 0`
- `PollMultiplexer` : `std::vector<struct pollfd> _fds`, implémente l'interface, traduit les `revents` en `Event` neutres. Le mot `pollfd` / les flags `POLLIN` ne sortent jamais de cette classe.

### Event (struct concret, pas abstrait)
Passeport neutre entre couche réseau et couche métier.
```cpp
struct Event { int fd; bool readable; bool writable; bool closed; };
```

---

## 5. Hiérarchie d'exceptions

L'élégance : en IRC, presque toute erreur est un *numeric reply* normalisé. L'exception **porte la réponse à envoyer**.

```
IrcException (abstrait, hérite de std::exception)
├── FatalException        // socket/bind/listen/poll ratés → arrêt du serveur
└── ACommandError         // abstrait, porte un numeric + ses paramètres
    ├── NeedMoreParams    // 461
    ├── NoSuchNick        // 401
    ├── NoSuchChannel     // 403
    ├── NotRegistered     // 451
    ├── NicknameInUse     // 433
    ├── ChanOPrivsNeeded  // 482
    ├── InviteOnlyChan    // 473
    ├── BadChannelKey     // 475
    └── ...               // une dérivée par numeric utilisé
```

- `ACommandError` expose `Message toMessage(const std::string& server, const std::string& nick) const` qui bâtit le `Message` numeric ; `Message::serialize()` produit la ligne complète (ex. `:server 461 nick JOIN :Not enough parameters`).
- **Seulement deux sites de catch dans tout le programme :**
  1. `main()` : `catch (FatalException&)` → log + arrêt propre (libération de toutes les ressources).
  2. `CommandDispatcher::dispatch()` autour de `execute()` : `catch (ACommandError& e)` → `client.queueReply(e.toMessage(serverName, nick).serialize())`. Le serveur ne bronche pas.
- Résultat : le corps des commandes est déclaratif — `if (!chan) throw NoSuchChannel(name);` et rien d'autre. La plomberie d'envoi est centralisée.
- **Piège C++98 :** si une exception stocke une `std::string`, son destructeur doit être déclaré `throw()` pour matcher `std::exception` sous `-std=c++98`.

---

## 6. Boucle d'événements — mécanique et pièges

Cycle de `Server::run()`, à chaque tour :
1. `_mux->wait(events)` — le processus dort dans `poll()` jusqu'à ce qu'un fd soit prêt.
2. Pour chaque `Event` :
   - `fd == _listenFd` → `acceptClient()` (le socket d'écoute se surveille comme les autres, via POLLIN).
   - `e.closed` (POLLHUP/POLLERR) → `disconnectClient(fd)`.
   - `e.readable` → `recv()` → `appendInput()` → boucle `extractLine()` → `Parser::parse()` → `dispatch()`.
   - `e.writable` → `send()` du `_outBuffer` ; si tout est parti → `setWriteInterest(fd, false)`.

Pièges qui font échouer l'éval :
- **`recv()` retourne 0 = déconnexion propre.** Déclencher `disconnectClient` (retrait des channels, broadcast du QUIT, `close`, `unwatch`, `delete`).
- **`send()` peut écrire MOINS que demandé** (buffer noyau plein). Ne retirer du `_outBuffer` que ce qui est réellement parti ; réessayer au prochain POLLOUT. D'où la persistance du buffer.
- **Fragmentation TCP.** Une commande peut arriver en plusieurs `recv()`, ou plusieurs commandes en un seul. C'est pourquoi `appendInput` (accumule) et `extractLine` (ne libère que sur `\r\n`) sont deux opérations distinctes. Test classique : `nc` qui envoie une commande coupée en deux paquets.
- **Ne demander POLLOUT que si `hasPendingOutput()`.** Sinon `poll()` boucle à vide en consommant le CPU.

---

## 7. Flux de données — exemple `JOIN #techno`

1. POLLIN sur fd 7 → `recv()` → `client.appendInput(bytes)` (accumulation dans `_inBuffer`).
2. `client.extractLine(line)` détecte `\r\n` → extrait `"JOIN #techno"`.
3. `Parser::parse(line)` → `Message{command:"JOIN", params:["#techno"]}`.
4. `dispatch` : `_commands["JOIN"]`, vérifie enregistrement + nb params (sinon throw), `execute(client, msg)`.
5. `JoinCommand` : `_server.getOrCreateChannel("#techno")`, puis `channel->canJoin(client, key)` (throw 473/475/471 si refus).
6. Mutation : `addClientToChannel(...)` (écrit les deux côtés) ; si channel créé → `promote(client)` (le créateur est op).
7. Réponses : `channel->broadcast(client.prefix() + " JOIN #techno")` empile dans chaque `_outBuffer` ; puis replies au joiner (332 topic, 353 noms, 366 fin).
8. La boucle active POLLOUT sur les fds concernés → envoi au tour suivant.

---

## 8. Commandes à implémenter

Authentification / enregistrement : **PASS**, **NICK**, **USER** (ordre PASS → NICK → USER ; le client n'est `isRegistered()` qu'après les trois).

Channels & messagerie : **JOIN**, **PART**, **PRIVMSG** (vers user ou channel), **QUIT**, **TOPIC**, **INVITE**, **KICK**.

Opérateur — **MODE** avec au minimum les flags : `+i` (invite-only), `+t` (topic restreint aux ops), `+k` (clé/mot de passe), `+o` (donner/retirer le statut d'op), `+l` (limite d'utilisateurs).

> **MODE est la commande la plus retorse** : parsing de chaînes type `+ik-l <clé> <limite>` avec arguments positionnels consommés dans l'ordre des flags qui en attendent un. La traiter en dernier, après une base solide.

PING/PONG nécessaires pour que les vrais clients ne timeout pas.

---

## 9. Jalons de développement suggérés

Construire et tester par paliers, jamais tout d'un coup :

1. **Tuyauterie réseau seule** — serveur qui accepte des connexions via `nc`, accumule les buffers, et fait un echo des lignes complètes. Zéro logique IRC. Valide socket + poll + non-bloquant + fragmentation.
2. **Squelette OOP** — `Server`, `Client`, `AMultiplexer`/`PollMultiplexer`, `Parser`/`Message`, `CommandDispatcher`/`ACommand`, hiérarchie d'exceptions. Une commande bidon enregistrée pour valider le dispatch.
3. **Enregistrement** — PASS/NICK/USER + gestion de l'état du client. À partir d'ici, un vrai client IRC peut se connecter.
4. **Channels** — JOIN/PART/PRIVMSG/QUIT/TOPIC. Cœur fonctionnel.
5. **Opérateurs** — INVITE/KICK puis MODE (tous les flags).
6. **Durcissement** — déconnexions brutales, flood, valgrind sans fuite, relecture des cas limites du sujet.

---

## 10. Anti-patterns à éviter

- God class `Server` de 600 lignes mêlant `poll()`, parsing et logique IRC. La frontière `AMultiplexer` existe pour l'empêcher.
- `if (cmd == "JOIN") ... else if (cmd == "KICK") ...` → utiliser la map de commandes.
- `send()` direct depuis une commande → passer par `queueReply` + POLLOUT.
- Manipuler `_members` / `_channels` à la main des deux côtés → passer par les méthodes de `Server`.
- Supposer que `recv` rend une commande entière, ou que `send` écrit tout → gérer les retours partiels.
- Abstraire `Client`/`Channel`/`Message` « pour faire propre » → ce sont des entités concrètes, l'abstraction y serait du bruit.
- Oublier de retirer un client des channels avant `delete` → dangling pointer.

---

*Document de référence. En cas de doute entre « faire vite » et « respecter ces principes », respecter les principes : ce projet est noté autant sur la propreté OOP et l'absence de crash/fuite que sur les fonctionnalités.*
