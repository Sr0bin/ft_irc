*This project has been created as part of the 42 curriculum by rorollin, prigaudi, vpirotti.*

# ft_irc

## Description

ft_irc is an **IRC server** written in **C++98**. It implements a basic,
**RFC-only** Internet Relay Chat server — following the original RFC 1459 / 2812
protocol. Real IRC has since grown well beyond this (IRCv3 and a stack of later
RFCs add capabilities, SASL, message tags, and more); this project deliberately
sticks to the historical core: authentication, channels, private and channel
messaging, and operator moderation.

It accepts connections from a real IRC client — our reference client is
**HexChat** — as well as raw `nc` sessions for low-level testing. Users can
authenticate, pick a nickname and username, join channels, exchange messages, and
operators get the usual moderation controls.

The whole server runs single-threaded around **one `poll()`** call: every socket
(the listening socket included) is non-blocking, and every read/write happens only
when `poll()` signals the fd is ready. It is built to **never crash and never
leak**, whatever the client does — partial data, abrupt disconnects, `Ctrl+D`,
flood.

## Architecture

The project is the team's first real dive into layered, polymorphic C++ design,
and a few choices shaped how everything else fell into place. Worth highlighting:

- **`AMultiplexer` / `PollMultiplexer`** — the entire network mechanism (the
  `poll()` call, the `pollfd` array, the `POLLIN`/`POLLOUT` flags) lives behind an
  abstract interface that only ever speaks in neutral `Event` structs. The IRC
  logic never sees a `pollfd`. Swapping `poll()` for `epoll`/`kqueue` would mean
  one new class and nothing else.

- **`ACommand` + `CommandDispatcher`** — instead of a giant `if (cmd == "JOIN")
  ... else if ...`, each command is its own small class registered in a
  `std::map<std::string, ACommand*>`. Adding a command is *one class + one
  registration line*, the dispatcher never changes.

- **Typed exception hierarchy** — in IRC almost every error *is* a numbered reply.
  So errors are thrown as typed exceptions (`NeedMoreParams` → 461,
  `ChanOPrivsNeeded` → 482, ...) that carry their own reply. There are only two
  `catch` sites in the whole program; command bodies stay declarative
  (`if (!chan) throw NoSuchChannel(name);`).

- **One wire formatter** — `Parser::parse()` turns bytes into a `Message`, and
  `Message::serialize()` turns it back into a line. The `:prefix`, the trailing
  `:`, and the `\r\n` exist in exactly one place.

The layers flow one way: `poll()` → network → protocol → commands → server state.
`Server` owns the `Client` and `Channel` objects; channels only hold
non-owning pointers, so disconnecting a client cleanly removes it from every
channel before the `delete`.

## Instructions

### Build

```sh
make            # build ./ircserv
make clean      # remove object files
make fclean     # remove objects + binary
make re         # rebuild from scratch
```

Compiled with `c++ -Wall -Wextra -Werror -std=c++98`.

### Run

```sh
./ircserv <port> <password>
```

- `<port>` — the TCP port the server listens on.
- `<password>` — the connection password every client must send via `PASS`.

### Connect

With HexChat: add a new network pointing at `localhost/<port>`, set the server
password to `<password>`, and connect.

Or with `nc` for raw testing:

```sh
nc -C localhost <port>
PASS <password>
NICK robin
USER robin 0 * :Robin
JOIN #42
PRIVMSG #42 :hello
```

## Features

Registration flow `PASS` → `NICK` → `USER`, then the full command set:

| Category      | Commands                                                   |
|---------------|------------------------------------------------------------|
| Registration  | `PASS`, `NICK`, `USER`                                      |
| Messaging     | `PRIVMSG` (user & channel), `JOIN`, `PART`, `QUIT`, `TOPIC` |
| Operators     | `INVITE`, `KICK`, `MODE`                                    |
| Keepalive     | `PING` / `PONG`                                             |

Supported channel modes via `MODE`: `+i` (invite-only), `+t` (topic restricted to
operators), `+k` (channel key), `+o` (grant/revoke operator), `+l` (user limit).

## Resources

- **RFC 1459** — Internet Relay Chat Protocol: https://datatracker.ietf.org/doc/html/rfc1459
- **RFC 2812** — IRC Client Protocol: https://datatracker.ietf.org/doc/html/rfc2812
- **Beej's Guide to Network Programming**: https://beej.us/guide/bgnet/
- `man poll`, `man 7 socket`, `man fcntl` — non-blocking I/O and event multiplexing.
- Reference client: **HexChat**, plus `nc` for low-level cases.

### Use of AI

AI (Claude) was used mainly as a **teaching and documentation aid**:

- **Explaining new concepts** and helping shape the overall architecture — the
  layered design, when an abstraction like `AMultiplexer` is justified versus when
  a plain concrete class is the right call, the command-pattern dispatch, and the
  exception-carries-the-reply idea.
- **Writing and maintaining the team's reference documents** (architecture notes,
  recorded design decisions, progress tracking) so the design stayed consistent as
  the project grew.
- **Reviewing diffs** for C++98 conformance, memory ownership, and leak/crash
  safety.

The IRC logic itself — parsing, the command implementations, the event loop — was
written and tested by the team.
