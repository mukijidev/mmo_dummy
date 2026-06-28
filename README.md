# MMO Dummy Client (Load Tester)

A headless load-testing tool for the MMO server. It simulates many concurrent players without the Unreal client, to measure concurrent connections, throughput (TPS), and stability.

> Personal MMO project: [server](https://github.com/mukijidev/mmo_server) · [client](https://github.com/mukijidev/mmo_client) · **dummy (this)**

## Highlights

- **`select()`-based multiplexing** — one thread drives ~50 non-blocking sockets; total load scales by adding threads instead of one thread per connection.
- **Per-dummy state machine** — disconnected → connected → logged-in → character list → character select → in-field, advancing one step per tick.
- **Realistic traffic** — replays the real login → character select → field entry → movement flow against the live server.

## Tech Stack

C++ · Winsock2 (`select`) · Visual Studio 2022

## Related

- Server: https://github.com/mukijidev/mmo_server
- Client: https://github.com/mukijidev/mmo_client
