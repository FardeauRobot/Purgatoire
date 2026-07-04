# Address Conversion — `htons`, `htonl`, `ntohs`, `ntohl`, `getaddrinfo`, `freeaddrinfo`, `getsockname`, `getprotobyname`

> *"Bytes traveling over the network always go in big-endian order. Your CPU thinks in little-endian. These functions are the translators between the two worlds."*

---

## The byte order problem

Every multi-byte value (port numbers, IP addresses) stored in a `sockaddr_in` must be in **network byte order** (big-endian). Most CPUs (x86, ARM in typical mode) are little-endian. If you forget to convert, the kernel reads garbage port numbers silently.

**Example with port 8080:**

```
8080 decimal = 0x1F90

Little-endian (your CPU):  [0x90] [0x1F]  ← byte at lower address first
Big-endian (network):      [0x1F] [0x90]  ← byte at higher address first

If you put 0x1F90 into sin_port without converting:
  The kernel sees 0x901F = port 36895. Your server is NOT listening on 8080.
```

---

## `htons()` and `htonl()` — host to network

```c
#include <arpa/inet.h>

uint16_t htons(uint16_t hostshort);   // host-to-network short (16-bit) — for ports
uint32_t htonl(uint32_t hostlong);    // host-to-network long  (32-bit) — for IP addrs
```

**`htons` parameters:**
- `hostshort` — a 16-bit port number in host byte order (e.g., `8080`); the return value is the same number in network (big-endian) order, ready to assign to `sin_port`

**`htonl` parameters:**
- `hostlong` — a 32-bit IP address in host byte order (e.g., `INADDR_ANY = 0`, `INADDR_LOOPBACK = 0x7F000001`); the return value goes into `sin_addr.s_addr`

**`htons` — always use for port numbers:**

```cpp
addr.sin_port = htons(8080);    // correct: 8080 in network byte order
addr.sin_port = 8080;           // WRONG: 8080 in host byte order → garbage port
```

**`htonl` — use for 32-bit IP addresses:**

```cpp
addr.sin_addr.s_addr = htonl(INADDR_ANY);    // 0.0.0.0 — all interfaces
addr.sin_addr.s_addr = htonl(0x7F000001);    // 127.0.0.1 — loopback
```

`INADDR_ANY` is `0` and `INADDR_LOOPBACK` is `0x7F000001`. These are host-byte-order constants that need `htonl()` before going into `sockaddr_in`.

**Under the hood:** on a little-endian system, `htons(8080)` just byte-swaps. On a big-endian system, it's a no-op. This is intentional — the code is always correct regardless of platform.

---

## `ntohs()` and `ntohl()` — network to host

```c
#include <arpa/inet.h>

uint16_t ntohs(uint16_t netshort);    // network-to-host short
uint32_t ntohl(uint32_t netlong);     // network-to-host long
```

**`ntohs` parameters:**
- `netshort` — a 16-bit value in network byte order, typically `sin_port` from a `sockaddr_in` filled by `accept()` or `getsockname()`; returns the port as a readable integer

**`ntohl` parameters:**
- `netlong` — a 32-bit value in network byte order, typically `sin_addr.s_addr`; returns the IP address in host byte order

Use these when *reading back* values from `sockaddr_in` — e.g., after `accept()` or `getsockname()`:

```cpp
struct sockaddr_in client_addr;
socklen_t len = sizeof(client_addr);
accept(listen_fd, (struct sockaddr*)&client_addr, &len);

int    client_port = ntohs(client_addr.sin_port);       // readable port
char   client_ip[INET_ADDRSTRLEN];
inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
// client_ip is now a human-readable string like "192.168.1.42"

// For CGI env vars:
// REMOTE_ADDR = client_ip
// REMOTE_PORT = client_port (as string)
```

**`inet_ntop` vs `inet_ntoa`:** `inet_ntoa` is not thread-safe (returns a pointer to a static buffer). Use `inet_ntop` — it writes into your buffer. Required pattern for webserv where you're converting many client addresses.

---

## `getaddrinfo()` and `freeaddrinfo()`

```c
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int getaddrinfo(const char *node, const char *service,
                const struct addrinfo *hints,
                struct addrinfo **res);

void freeaddrinfo(struct addrinfo *res);
```

**`getaddrinfo` parameters:**
- `node` — hostname or IP string to resolve (e.g., `"localhost"`, `"192.168.1.10"`); pass `NULL` with `AI_PASSIVE` in hints to get the wildcard address (`INADDR_ANY`)
- `service` — port as a string (e.g., `"8080"`) or service name (e.g., `"http"`); the function fills in `sin_port` for you
- `hints` — pointer to a partially filled `struct addrinfo` that filters results; set `ai_family` (`AF_INET`), `ai_socktype` (`SOCK_STREAM`), and `ai_flags` (`AI_PASSIVE` for servers); pass `NULL` to accept any result
- `res` — output: pointer to a `struct addrinfo *` set to the head of a dynamically allocated linked list of results; free with `freeaddrinfo()` when done

**`freeaddrinfo` parameters:**
- `res` — the `struct addrinfo *` returned by `getaddrinfo()`; frees the entire linked list; do not dereference `res` after this call

**What it does:** converts a hostname or IP string + port/service string into one or more `sockaddr` structures, handling DNS resolution and address family selection for you. The modern, thread-safe, IPv4+IPv6-capable replacement for the old `gethostbyname()`.

**The result struct:**

```c
struct addrinfo {
    int              ai_flags;
    int              ai_family;     // AF_INET or AF_INET6
    int              ai_socktype;   // SOCK_STREAM
    int              ai_protocol;
    socklen_t        ai_addrlen;
    struct sockaddr *ai_addr;       // ready to use with bind()/connect()
    char            *ai_canonname;
    struct addrinfo *ai_next;       // linked list — may have multiple results
};
```

**Webserv: using `getaddrinfo` to bind to a config-specified address:**

```cpp
// Config says: listen 192.168.1.10:8080
// Instead of manually building sockaddr_in, use getaddrinfo:

struct addrinfo hints, *res;
std::memset(&hints, 0, sizeof(hints));
hints.ai_family   = AF_INET;        // IPv4 only
hints.ai_socktype = SOCK_STREAM;    // TCP
hints.ai_flags    = AI_PASSIVE;     // fill in local address for bind()

int status = getaddrinfo("192.168.1.10", "8080", &hints, &res);
if (status != 0) {
    // Use gai_strerror(status) — NOT strerror(errno) — for getaddrinfo errors
    throw std::runtime_error(std::string("getaddrinfo: ") + gai_strerror(status));
}

// res is now a linked list of valid addresses to try.
// For a server: just use the first one.
int listen_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
bind(listen_fd, res->ai_addr, res->ai_addrlen);

// ALWAYS free after use:
freeaddrinfo(res);
```

**All `ai_flags` values for the hints struct:**

| Flag | What it does |
|---|---|
| `AI_PASSIVE` | If `node` is NULL, return the wildcard address (`INADDR_ANY` / `::`) suitable for `bind()`; ignored when `node` is a specific address |
| `AI_CANONNAME` | Fill `ai_canonname` with the canonical (fully-qualified) hostname after DNS resolution; useful for logging |
| `AI_NUMERICHOST` | Treat `node` as a numeric IP string only — skip DNS lookup; returns `EAI_NONAME` if it isn't a valid IP; avoids latency when you already have an IP |
| `AI_NUMERICSERV` | Treat `service` as a decimal port number only — skip `/etc/services` lookup; use when you know the port is numeric |
| `AI_ADDRCONFIG` | Only return IPv4 results if the host has an IPv4 interface configured; same for IPv6; avoids returning unreachable address families |
| `AI_V4MAPPED` | If no IPv6 addresses found, return IPv4 addresses mapped to IPv6 (`::ffff:a.b.c.d`) so IPv6-only code can still reach IPv4 hosts |
| `AI_ALL` | Combined with `AI_V4MAPPED`: return both native IPv6 addresses and IPv4-mapped ones even if IPv6 results exist |

**`AI_PASSIVE` flag:** when `node` is NULL and `AI_PASSIVE` is set, `getaddrinfo` fills in the wildcard address (`INADDR_ANY` for IPv4). This means "listen on all interfaces." When `node` is a specific IP, `AI_PASSIVE` is ignored.

**Why `getaddrinfo` instead of manual `sockaddr_in`:**
- Handles both IPv4 and IPv6 transparently.
- Performs DNS resolution (hostname → IP) for you.
- Returns a linked list: if the first address fails to bind, try the next.
- The hints struct makes your intent explicit.

**`freeaddrinfo` is mandatory:** `getaddrinfo` allocates a heap-linked list. Not calling `freeaddrinfo` is a memory leak. Valgrind will catch it.

---

## `gai_strerror()` — error messages for `getaddrinfo`

```c
#include <netdb.h>

const char *gai_strerror(int errcode);
```

**Parameters:**
- `errcode` — the non-zero integer returned directly by `getaddrinfo()` on failure; this is **not** an `errno` value — passing `errno` here gives nonsense

`getaddrinfo()` does **not** set `errno` on failure. It returns a non-zero error code. Convert it to a human-readable string with `gai_strerror()`, not `strerror()`.

```cpp
int status = getaddrinfo(node, service, &hints, &res);
if (status != 0) {
    std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
    // Common errors:
    // EAI_NONAME: node or service not known (DNS failure or typo)
    // EAI_SERVICE: service not supported for socket type
    // EAI_AGAIN:  DNS temporarily unavailable — retry later
}
```

---

## `getprotobyname()`

```c
#include <netdb.h>

struct protoent *getprotobyname(const char *name);

struct protoent {
    char  *p_name;      // official name, e.g. "tcp"
    char **p_aliases;   // alias list
    int    p_proto;     // protocol number (6 for TCP)
};
```

**Parameters:**
- `name` — protocol name string: `"tcp"` returns the entry with `p_proto = 6`, `"udp"` returns `p_proto = 17`

**What it does:** looks up the protocol number for a protocol name. `TCP` is always `6`, `UDP` is always `17` — but `getprotobyname("tcp")` is the correct way to obtain this portably.

**In webserv:** this is on the allowed list but you'll rarely use it directly. It matters for `socket()` when you want to be explicit:

```cpp
struct protoent *tcp = getprotobyname("tcp");
int fd = socket(AF_INET, SOCK_STREAM, tcp->p_proto);
// Equivalent to: socket(AF_INET, SOCK_STREAM, 0)
// The 0 already means "let kernel pick" which defaults to IPPROTO_TCP
```

**Where it's more useful:** if you're parsing a config file that allows `protocol tcp/udp` and you want to look it up dynamically.

**Thread safety note:** `getprotobyname()` returns a pointer to a static internal struct. In a multi-threaded program, use `getprotobyname_r()` instead. For single-threaded webserv, it's fine.

---

## `getsockname()` — find what address a socket is bound to

```c
#include <sys/socket.h>

int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
// Returns: 0 on success, -1 on error
```

**Parameters:**
- `sockfd` — any bound or connected socket fd; use the client fd (not the listen fd) to get the specific interface a connection arrived on — the listen fd returns `0.0.0.0` if bound to `INADDR_ANY`
- `addr` — output: pointer to a `struct sockaddr_in` (cast to `struct sockaddr *`); the kernel fills it with the local IP and port
- `addrlen` — in/out: set to `sizeof(struct sockaddr_in)` before calling; updated with the actual number of bytes written into `addr`

**Primary webserv use: CGI environment variables.**

The CGI spec requires `SERVER_NAME` and `SERVER_PORT` env vars that identify which server address the request arrived on. `getsockname()` on the client fd gives you that:

```cpp
struct sockaddr_in local_addr;
socklen_t len = sizeof(local_addr);
getsockname(client_fd, (struct sockaddr*)&local_addr, &len);

char server_ip[INET_ADDRSTRLEN];
inet_ntop(AF_INET, &local_addr.sin_addr, server_ip, sizeof(server_ip));

std::string server_port_str;
{
    std::ostringstream oss;
    oss << ntohs(local_addr.sin_port);
    server_port_str = oss.str();
}

setenv("SERVER_NAME", server_ip, 1);
setenv("SERVER_PORT", server_port_str.c_str(), 1);
```

**Why `getsockname` on the client fd, not the listen fd:** if you listen on `0.0.0.0` (all interfaces), the listen fd's address is `0.0.0.0` — not useful for `SERVER_NAME`. But the client fd's local address is the specific interface the TCP connection came in on (e.g., `192.168.1.10`). That's what you want.

---

## Quick reference

| Function | Input | Output | Use for |
|---|---|---|---|
| `htons(port)` | host-order `uint16_t` | network-order | Filling `sin_port` |
| `htonl(addr)` | host-order `uint32_t` | network-order | Filling `sin_addr.s_addr` |
| `ntohs(port)` | network-order `uint16_t` | host-order | Reading port from `sockaddr_in` |
| `ntohl(addr)` | network-order `uint32_t` | host-order | Reading IP from `sockaddr_in` |
| `getaddrinfo()` | hostname + port string | `struct addrinfo*` | Resolving config addresses |
| `freeaddrinfo()` | `struct addrinfo*` | — | Freeing result of above |
| `gai_strerror()` | `getaddrinfo` return code | error string | Error reporting |
| `getsockname()` | fd | `sockaddr` | Server IP/port for CGI env |
| `getprotobyname()` | `"tcp"` | protocol number | Explicit socket() protocol arg |

---

## Related
- [`01_SOCKET_LIFECYCLE.md`](01_SOCKET_LIFECYCLE.md) — where these values go (`bind`, `accept`)
- [`05_PROCESS_AND_CGI.md`](05_PROCESS_AND_CGI.md) — CGI env vars that use `getsockname` and `ntohs`
- [`07_ERRORS.md`](07_ERRORS.md) — `gai_strerror` vs `strerror` distinction
