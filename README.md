# OS Socket Programming — Semester Project

A small C++ networking project I worked on for my **Operating Systems semester project**. I explored how processes communicate over TCP sockets, first through a multi-client broadcast server and then through a simple client/server exchange between machines.

I have kept the original coursework in `docs/original/` and organized a cleaned-up version under `src/`. The terminal interface has also been made easier to follow without turning this into a different project.

## What I built

| Part | Description | Default port |
| --- | --- | --- |
| Broadcast chat | A server that accepts multiple clients, receives messages, and broadcasts them to connected users | `8094` |
| Distributed client/server | A basic two-way messaging exercise using TCP sockets | `8084` |

### Concepts practiced

- POSIX sockets (`socket`, `bind`, `listen`, `accept`, `connect`, `send`, `recv`)
- Client/server communication over TCP
- Threads for concurrent client handling
- Shared resources and mutexes
- Sending messages between two computers on a local network

## Terminal UI preview

The project runs in the terminal rather than a graphical window. These examples show the cleaned-up console interface and the basic client/server flow.

**Broadcast server**

```text
+----------------------------------------+
|  OS LAB  /  BROADCAST SERVER            |
+----------------------------------------+

[ready] Listening on port 8094
Type a message to broadcast, or /quit to stop.

[+] Client connected: 127.0.0.1
[client 4] Hello from the client!
```

**Broadcast client (illustrative conversation)**

```text
+----------------------------------------+
|  OS LAB  /  BROADCAST CLIENT            |
+----------------------------------------+

Connected to server: 127.0.0.1:8094
You: Hello everyone!
[server] Message received from another client
You: /quit
```

*The terminal blocks are representative previews for the README, not screenshots or verified recordings of a live multi-client session. Actual output can differ slightly.*

## Getting started

**Requirements:** Linux (or WSL on Windows), `g++`, `make`, and a terminal. These examples use POSIX sockets, so they are not directly compatible with native Windows builds.

```bash
make
```

For the **broadcast chat**, open separate terminals:

```bash
./bin/broadcast-server
```

```bash
./bin/broadcast-client
```

Run the client command again in another terminal to try multiple participants. Type a message and press Enter. Use `/quit` to leave.

For the **distributed client/server exercise**, start the server and then the client:

```bash
./bin/distributed-server
./bin/distributed-client
```

The distributed exercise is a turn-based exchange: send a message from the client, then reply from the server. Start each command in a separate terminal. Use `/quit` to exit.

### Connecting from another computer

Pass the server machine's local IPv4 address to the client, for example:

```bash
./bin/broadcast-client 192.168.1.10
./bin/distributed-client 192.168.1.10
```

Both computers must be on a network where the chosen TCP port is reachable. Adjust the firewall if needed. The default `127.0.0.1` connects to the same computer.

## Repository structure

```text
OS-Socket-Programming-Semester-Project/
├── src/
│   ├── broadcast/
│   │   ├── server.cpp
│   │   └── client.cpp
│   └── distributed/
│       ├── server.cpp
│       └── client.cpp
├── docs/
│   └── original/          # Original submission files and LaTeX report
├── bin/                   # Compiled programs (not tracked)
├── Makefile
├── .gitignore
└── README.md
```

## Changes since the coursework version

The original source files are preserved for reference. In the updated code I aligned the distributed client/server ports (the original used different ports), added clearer terminal output, improved disconnect handling, and used a mutex to protect the broadcast server's shared client list. The broadcast messages are now newline-delimited so the receiver can display them more consistently.

## Limitations

This is an educational networking exercise, not a production messaging service. It does not include accounts, encryption, persistent message history, or a graphical desktop interface. The broadcast server uses detached client threads and its shutdown is intended for short lab sessions. The distributed exchange is single-client and turn-based.

## Project information

**Course:** Operating Systems  
**Project type:** University semester project  
**Language:** C++  
**Original work:** 2024  

I kept the original LaTeX report and source code in the repository so the coursework remains easy to review alongside the cleaned-up version.
