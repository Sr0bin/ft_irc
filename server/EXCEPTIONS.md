# Gestion des exceptions — ft_irc

Comment les erreurs circulent dans le serveur, et **où** elles sont rattrapées.
Document de référence pour quiconque touche au serveur ou aux commandes.
Complète [ARCHITECTURE.md](ARCHITECTURE.md) §5 ; les choix sous-jacents sont dans
[DECISIONS.md](DECISIONS.md).

Principe directeur : **on ne lève une exception que pour ce qui est vraiment
exceptionnel.** Ce qui est fréquent et attendu (un client qui se déconnecte) se
gère en flux normal, pas par `throw`.

---

## 1. Trois phases, trois traitements

| Phase | Erreur typique | Mécanisme | Conséquence |
|-------|----------------|-----------|-------------|
| **Init** | `socket`/`bind`/`listen` ratés | `throw FatalException` | `main` arrête le serveur proprement |
| **Boucle serveur** | `poll()` rate (hors `EINTR`) | `throw FatalException` | idem : fatal |
| **Boucle serveur** | `recv`/`send`/`accept` ratés sur **un** fd, ou `recv()==0` | **pas d'exception** → `disconnectClient(fd)` | on retire ce client, le serveur continue |
| **Commande user** | param manquant, nick/chan inexistant, droits… | `throw` d'une dérivée d'`ACommandError` | numeric reply au client, serveur intact |

La ligne rouge : **fatal = tout le serveur** (init + `poll()` lui-même).
**Per-client = jamais une exception** — un client qui part est la normalité, pas
une anomalie. Mélanger les deux ferait sur-catcher.

---

## 2. La hiérarchie

```
std::exception
└── IrcException                 // base concrète, porte un _msg + what()
    ├── FatalException           // init/poll ratés → arrêt (catché en main)
    └── ACommandError            // porte un numeric : _code + _params + _text
        ├── NeedMoreParams       // 461
        ├── NoSuchNick           // 401
        ├── NoSuchChannel        // 403
        └── …                    // une dérivée par numeric, ajoutée au besoin
```

- `ACommandError` est **data-driven** : il porte `_code`/`_params`/`_text` et les passe à `Message::numeric` via `toMessage(server, nick)`. Le formatage est mutualisé dans `Message::serialize` (partagé avec `ACommandReply`, cf. D18).
  Les sous-classes sont des **constructeurs fins** qui figent code + texte
  (`throw NoSuchChannel(name)`), pour des sites de throw lisibles et un texte
  cohérent partout.
- `FatalException` ne porte qu'un message ; elle ne descend **pas** d'`ACommandError`,
  donc elle n'est jamais confondue avec une erreur de commande.

---

## 3. Les deux seuls sites de catch

```
main():
    try { Server server(config); server.run(); }
    catch (FatalException &e)  → log + exit(1)        // init OU poll() mort
    catch (std::exception &e)  → log + exit(1)        // dernier rempart

CommandDispatcher::dispatch(client, msg):
    try {
        // pré-checks (requiresRegistration, minParams) — peuvent throw NeedMoreParams
        cmd.execute(client, msg);                     // peut throw n'importe quel ACommandError
    }
    catch (ACommandError &e)  → client.queueReply(e.toMessage(serverName, nick).serialize())
    catch (std::exception &e) → log ; on continue     // FILET : ne tue jamais le serveur
```

### Ordre des catch (anti sur/sous-catch)
1. `ACommandError&` **d'abord** : réponse sémantique (numeric).
2. `std::exception&` **ensuite** : filet de sécurité.

`FatalException` n'est **jamais** levée depuis l'exécution d'une commande (une commande
ne fait pas d'init socket), donc le filet `std::exception` du dispatcher ne risque pas
d'avaler un fatal qui aurait dû arrêter le serveur.

### Le filet : log & continue
Si une commande lève autre chose qu'un `ACommandError` (`std::bad_alloc`, un `.at()`
hors borne, un bug), le dispatcher **logge et continue** — on suppose une erreur
ponctuelle sans effet de bord destructeur. On ne tue pas le serveur, on ne drop pas
le client. Raisonnement : si c'est réellement grave, ça se reproduira plus haut
(dans la boucle, à l'init) et le `catch` de `main` arrêtera le programme **proprement**.
Garantit le « never crash » du sujet sans masquer les vrais problèmes.

---

## 4. Comment la réponse circule (data transit)

`ACommandError` ne connaît **ni `Server` ni `Client`** — découplage volontaire.

```
toMessage(server, nick)  renvoie   un Message numeric (donnée, pas une string)
                                   code "403", params ["bob","#x"], trailing "No such channel"
```

L'exception ne formate pas et ne préfixe pas : `toMessage` prend `server` + `nick`
en simples `std::string` et bâtit un `Message` ; c'est `Message::serialize()` (point
unique) qui pose le préfixe `:<server>`, le `:` du trailing et le `\r\n`. L'exception
reste une donnée pure.

```
commande lève NoSuchChannel("#x")
        │
        ▼
dispatch catch (ACommandError& e)
        │   nick = client (nick courant, ou "*" si pas encore enregistré)
        ▼
client.queueReply(e.toMessage(serverName, nick).serialize())
        │   → ":irc.server 403 bob #x :No such channel\r\n"
        ▼
boucle poll() : POLLOUT → send()   (envoi réel, géré par la couche réseau)
```

Même logique que `Client`, file d'octets passive : le formatage (préfixe compris)
est fait en amont — désormais centralisé dans `Message::serialize` — l'envoi en
aval. L'exception décrit l'erreur, elle ne l'envoie pas.

---

## 5. Pièges C++98 retenus

- Une exception qui stocke une `std::string` doit déclarer son destructeur `throw()`
  pour matcher `std::exception` sous `-std=c++98`. Toute la hiérarchie respecte ça.
- Exceptions levées **par valeur**, attrapées **par référence** (`catch (X& e)`),
  jamais par valeur (slicing).
- `toMessage` / `Message::serialize` ne sont **pas** `throw()` : ils allouent (`std::ostringstream`, `std::string`).
