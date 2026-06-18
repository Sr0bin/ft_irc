# Journal de décisions — ft_irc

Choix d'implémentation tranchés, avec leur *pourquoi*. But : ne pas rouvrir dix fois une question déjà réglée. Le plus récent en bas.

Format : `Dn — décision — pourquoi — date`.

---

### D1 — `Server::disconnectClient` ferme le fd, pas le destructeur de `Client`
`~Client` ne fait **pas** `close(_fd)`. C'est `Server::disconnectClient(fd)` qui orchestre `close` + `unwatch` + retrait des channels + `delete`.
**Pourquoi :** un seul site de fermeture = pas de double-`close`, et la séquence de déconnexion (retrait des channels avant `delete`) est centralisée là où la cohérence importe.
*(2026-06-17)*

### D2 — `getClientByNick` / `getChannelByName` renvoient des pointeurs
Signature `Client*` / `Channel*` (et non `Client&` / `Channel&`). `NULL` = absent.
**Pourquoi :** cohérent avec les maps (`map<int, Client*>`, `map<string, Channel*>`), et donne une sémantique « non trouvé » propre sans avoir à lever une exception ou fabriquer un objet bidon. Le code appelant teste `if (!c) throw NoSuchNick(...)`.
*(2026-06-17)*

### D3 — `ACommand` reçoit `Server&` par le constructeur
`ACommand(Server& server) : _server(server)`. Pas de constructeur par défaut possible (membre référence). Chaque commande concrète transmet la référence vers le haut ; le `CommandDispatcher` la fournit en construisant les commandes.
**Pourquoi :** un membre référence *doit* être initialisé à la construction. Injection par le constructeur = couplage explicite et testable.
*(2026-06-17)*

### D4 — Classes non-copiables : copie/`operator=` privés et non implémentés
Pour `Server`, `Client`, `Channel`, `PollMultiplexer`, `CommandDispatcher` : déclaration privée, **sans** définition.
**Pourquoi :** forme canonique 42 pour les classes qui possèdent des ressources (fds, pointeurs). La copie n'a pas de sens et doit échouer à la compilation, pas silencieusement.
*(2026-06-17)*

### D5 — `Message` est un objet valeur, ignorant de `ACommand`
`Message` ne `#include` plus `ACommand.hpp` et n'a pas de méthode renvoyant un `ACommand*`. Il porte juste `_prefix`, `_command`, `_params` + getters `const`. C'est le `CommandDispatcher` qui fait le lien commande↔string.
**Pourquoi :** séparation des couches. Les données parsées ne doivent rien savoir de la logique de dispatch. Évite une dépendance circulaire `Message` ↔ `ACommand`.
*(2026-06-17)*

### D6 — `IrcException` : destructeur et méthodes `throw()`
`IrcException` hérite de `std::exception`, stocke un `std::string _msg`, et déclare tous ses membres pertinents `throw()` (dont le destructeur).
**Pourquoi :** piège C++98 — une exception qui stocke une `std::string` doit avoir un destructeur `throw()` pour matcher `std::exception` sous `-std=c++98`, sinon erreur de compilation.
*(2026-06-17)*

### D7 — `Client` est une file d'octets passive ; le formatage (préfixe inclus) revient à la couche commandes
`queueReply(msg)` fait juste `_outBuffer += msg` — aucun préfixe, aucun `\r\n` ajouté. La chaîne complète (`:nick!user@host CMD … \r\n`) est construite par les commandes *avant* d'être empilée. `Client::prefix()` n'est qu'un helper renvoyant `nick!user@host` pour ce client. L'envoi réel (`send()`) est fait par la boucle `poll()` du serveur.
**Pourquoi :** séparation des couches (cf. D5). `Client` ne sait ni formater ni envoyer ; il stocke. Évite de disperser la logique de protocole.
*(2026-06-18)*

### D8 — `extractLine` découpe sur `\r\n` strict
Recherche de `"\r\n"` ; `\n` seul ne déclenche pas. Renvoie la ligne sans le délimiteur, qu'on retire du buffer.
**Pourquoi :** conformité RFC à la lettre. Choix assumé même si `nc` en mode ligne peut envoyer `\n` seul (on testera avec `nc -C` ou un vrai client).
*(2026-06-18)*

### D9 — `clientState` : `CONNECTING → PASSWORD_OK → REGISTERED` ; `isRegistered()` ≡ `== REGISTERED`
Enum linéaire des phases d'enregistrement. `isRegistered()` ne renvoie vrai qu'à `REGISTERED`. C'est la couche commandes (PASS/NICK/USER, Jalon 3) qui fait avancer `_state`.
**Pourquoi :** un état grossier suffit ici ; le suivi fin « nick posé mais pas user » se fera côté commandes, pas dans l'enum.
*(2026-06-18)*

### D10 — `Channel::canJoin` : `_userLimit == 0` = pas de limite
Convention : une limite à 0 signifie « illimité ». `canJoin` refuse si invite-only et non invité, ou mauvais mot de passe, ou limite atteinte.
**Pourquoi :** évite un booléen `hasLimit` séparé ; 0 est une sentinelle naturelle, alignée sur MODE `+l`.
*(2026-06-18)*

### D11 — Frontière `Client`↔serveur : il manquera un accesseur de sortie pour le flush
`Client` n'expose que `hasPendingOutput()`. Pour faire `send()` + envoi partiel, le serveur (prigaudi) aura besoin de lire/consommer `_outBuffer` (p.ex. `const std::string& outBuffer()` + `consumeOutput(size_t)`). **Non ajouté** tant que prigaudi ne le demande pas — c'est son besoin, à acter ensemble.
**Pourquoi :** ne pas élargir l'interface `Client` unilatéralement ; respecter le découpage des responsabilités.
*(2026-06-18)*

### D12 — Gestion des exceptions : conception détaillée dans `EXCEPTIONS.md` (partagé)
Le « comment/pourquoi » complet de la gestion d'erreurs vit dans `EXCEPTIONS.md` (doc partagé, branche `doc`). Calls non triviaux : (1) erreurs I/O **par-client** (recv/send/accept, recv==0) = **pas d'exception**, juste `disconnectClient(fd)` — c'est attendu, pas exceptionnel ; (2) filet `catch(std::exception&)` dans `dispatch` = **log & continue** (on ne tue pas le serveur, on ne drop pas le client ; si c'est grave ça remontera au `catch` de `main`) ; (3) `buildReply(nick)` renvoie `"<code> <nick> <params> :<text>"` sans `:<server>` → préfixé par le dispatcher, donc l'exception ignore `Server`/`Client`.
**Pourquoi :** ne lever que pour le vraiment exceptionnel (anti sur-catch) ; garantir « never crash » sans masquer les vrais bugs ; découplage de couches.
*(2026-06-18)*

### D13 — Layout des exceptions : pas d'impl en header → numerics groupés
Pas d'implémentation dans les headers (cf. §3). Les 3 sous-classes numériques témoins sont **groupées** dans `ACommandError.hpp/.cpp` (déclarations + impls), au lieu d'un header + un .cpp chacune. Option « header-only inline » rejetée (impl en header interdite).
**Pourquoi :** sous-classes triviales (un ctor) → un fichier par classe = boilerplate quasi vide + entrées Makefile inutiles. Compromis assumé vs « 1 classe/fichier » de §3.
*(2026-06-18)*
