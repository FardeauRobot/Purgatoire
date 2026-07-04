# `netinet/in.h`

> *"The vocabulary layer. It doesn't let you do anything — it just names the things you'll talk about."*

Defines the structures and constants needed to describe an IPv4 (or IPv6) endpoint. `sys/socket.h` gives you the verbs; this header gives you the nouns.

---

## Types

```c
in_port_t       // uint16_t — a port number
in_addr_t       // uint32_t — an IPv4 address as a 32-bit integer

struct in_addr {
    in_addr_t s_addr;   // the address itself, in network byte order
};
```

### `struct sockaddr_in` — the IPv4 address

```c
struct sockaddr_in {
    sa_family_t     sin_family;   // always AF_INET
    in_port_t       sin_port;     // port in network byte order
    struct in_addr  sin_addr;     // IP address in network byte order
    char            sin_zero[8];  // padding — always zero
};
```

**`sin_family`** — always `AF_INET`. The kernel reads this first to know how to interpret the rest of the struct.

**`sin_port`** — the port wrapped in `htons()`. Forget the conversion and you silently bind to the wrong port.

**`sin_addr.s_addr`** — the IP address as a 32-bit integer. Use `INADDR_ANY` or `INADDR_LOOPBACK`, or convert a string with `inet_pton()`.

**`sin_zero[8]`** — padding to match the size of `struct sockaddr`. Must be zeroed — always `memset` the whole struct before filling it.

Full setup pattern:

```cpp
struct sockaddr_in addr;
std::memset(&addr, 0, sizeof(addr));
addr.sin_family      = AF_INET;
addr.sin_port        = htons(8080);
addr.sin_addr.s_addr = htonl(INADDR_ANY);
```

---

## Constants

### Special addresses

```c
INADDR_ANY       // 0.0.0.0 — bind to all network interfaces
INADDR_LOOPBACK  // 127.0.0.1 — loopback only, same machine
```

`INADDR_ANY` is literally `0x00000000`. Zero in any byte order, so `htonl(INADDR_ANY)` == `INADDR_ANY` — but wrapping it is still the correct idiom.

`INADDR_LOOPBACK` is `0x7f000001` (127.0.0.1 packed into a 32-bit integer). Useful for client code connecting to a local server.

---

## Byte-order functions

Network byte order is big-endian. Your CPU (Apple Silicon, x86) is little-endian. These functions convert between them.

```c
uint16_t htons(uint16_t host_short);   // host → network, 16-bit (ports)
uint16_t ntohs(uint16_t net_short);    // network → host, 16-bit
uint32_t htonl(uint32_t host_long);    // host → network, 32-bit (addresses)
uint32_t ntohl(uint32_t net_long);     // network → host, 32-bit
```

**h** = host, **n** = network, **s** = short (16-bit), **l** = long (32-bit).

Under the hood on a little-endian machine:

```c
uint16_t htons(uint16_t x) {
    return (x >> 8) | (x << 8);   // swap the two bytes
}
```

On a big-endian machine, these compile to no-ops.

| Value | Without `htons` | With `htons` |
|---|---|---|
| Port 8080 (`0x1F90`) | stored as `90 1F` | stored as `1F 90` ✓ |

---

## `struct sockaddr_in6` — IPv6 (for reference)

```c
struct sockaddr_in6 {
    sa_family_t      sin6_family;   // AF_INET6
    in_port_t        sin6_port;
    uint32_t         sin6_flowinfo;
    struct in6_addr  sin6_addr;
    uint32_t         sin6_scope_id;
};
```

Out of scope for the 42 webserv project, but the pattern is identical to `sockaddr_in`.

---

## Summary

```
Types      in_addr, sockaddr_in, in_port_t, in_addr_t
Constants  INADDR_ANY (0.0.0.0), INADDR_LOOPBACK (127.0.0.1)
Functions  htons, ntohs, htonl, ntohl
```

---

## Related

- [`SYS_SOCKET_H.md`](SYS_SOCKET_H.md) — the grammar layer: socket, bind, listen, accept
- [`../functions/01_SOCKET_LIFECYCLE.md`](../functions/01_SOCKET_LIFECYCLE.md) — full bind() usage with sockaddr_in
- [`../functions/04_ADDRESS_CONVERSION.md`](../functions/04_ADDRESS_CONVERSION.md) — getaddrinfo, inet_pton for config-driven addresses
