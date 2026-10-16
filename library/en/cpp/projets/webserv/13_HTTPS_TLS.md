# 13 — HTTPS / TLS (overview only)

> *"HTTPS is HTTP wearing a coat."*

**Status for webserv: out of scope.** The subject does not require implementing TLS. This file exists for general understanding — you'll deal with HTTPS in `ft_transcendence`, `Inception`, and every backend role for the rest of your career.

Brief by design. Skim now; re-read when you actually need it.

---

## What HTTPS is

```
   HTTP                  HTTPS
   ────                  ─────
   App: HTTP             App:  HTTP
                         Sec:  TLS    ← the new layer
   Transport: TCP        Transport: TCP
   Network:   IP         Network:   IP
   Link:      Eth        Link:      Eth
```

HTTPS = **HTTP + TLS + (everything else unchanged)**.

TLS sits between TCP and HTTP. From HTTP's point of view, nothing changes — bytes go in, bytes come out. TLS handles:

1. **Encryption** — only the two parties can read the bytes (privacy).
2. **Integrity** — bytes can't be tampered with in transit without detection.
3. **Authentication** — the client can verify the server is who it claims to be (via certificates).

By convention HTTPS uses port **443** (vs HTTP's 80). The URL scheme is `https://` instead of `http://`.

---

## What TLS does, at a very high level

1. **Handshake** — client and server agree on a cipher suite, exchange keys, the server proves its identity with a certificate signed by a Certificate Authority (CA).
2. **Symmetric encryption** — after the handshake, both sides share a secret session key and use fast symmetric crypto (AES, ChaCha20) for actual data.
3. **MAC / AEAD** — every record carries an authentication tag so tampering is detected.

Modern TLS (1.3, 2018) has cut the handshake to a single round-trip (vs two in TLS 1.2). This matters for first-byte latency on HTTPS connections.

You don't need to implement any of this. Real-world: you use OpenSSL or rustls or BoringSSL and they do it for you.

---

## Why webserv doesn't require TLS

Three reasons:
1. **Implementing TLS from scratch is a months-long project.** Even using OpenSSL is significant work — picking ciphers, loading certs, handling renegotiation, error paths.
2. **The standard pattern is to terminate TLS at a reverse proxy.** In real deployments, you run your server on plain HTTP behind nginx/HAProxy/Caddy, and they handle TLS. Your application code stays simple.
3. **The educational goal of webserv is HTTP semantics, not TLS internals.** Mixing them would dilute the project.

If you want to expose your webserv over HTTPS for fun, run it behind nginx as a reverse proxy:

```nginx
server {
    listen 443 ssl;
    server_name example.com;
    ssl_certificate     /etc/letsencrypt/live/example.com/fullchain.pem;
    ssl_certificate_key /etc/letsencrypt/live/example.com/privkey.pem;

    location / {
        proxy_pass http://localhost:8080;   # ← your webserv
    }
}
```

nginx handles TLS, webserv handles HTTP. Clean separation.

---

## What HTTPS changes for your HTTP code

**Almost nothing.** From your server's perspective:

- The request you parse is the same.
- The response you generate is the same.
- The headers, methods, status codes are the same.

A few small differences:

| Aspect | HTTP | HTTPS |
|---|---|---|
| Port | 80 (default) | 443 (default) |
| URL scheme | `http://` | `https://` |
| `Set-Cookie` `Secure` attribute | Browsers may set if available | Required for sensitive cookies |
| HSTS (`Strict-Transport-Security`) | n/a | Useful — tells browser "only HTTPS for this domain from now on" |

---

## Certificates — the trust model

When your browser connects to `https://google.com`, the server presents a **certificate** containing:
- Google's public key
- The hostname(s) the cert is valid for (`*.google.com`, `google.com`, etc.)
- An expiry date
- A signature from a Certificate Authority (CA) — usually a chain of signatures up to a "root CA" your browser trusts

Your browser has a built-in list of trusted root CAs (~100 of them). If the server's cert chains up to one of those roots and the hostname matches and it hasn't expired, the connection is trusted. Otherwise the browser shows a scary warning.

For development, **self-signed certificates** are common — you generate a cert yourself, browsers don't trust it (warning page), you click through. Tools like `mkcert` automate this: they install a local CA in your system trust store and sign certs for `localhost` from it, so your browser silently accepts them.

---

## When you'll actually deal with this

| Project / Job | TLS exposure |
|---|---|
| webserv (now) | **None.** |
| ft_transcendence | Yes — needs HTTPS for OAuth, secure WebSockets. Likely behind nginx. |
| Inception | Yes — you'll configure nginx with TLS termination. |
| Any backend job | Yes, but usually as "deploy nginx in front of my code", not "implement TLS." |
| Security or infra role | Deep — you'll learn cipher suites, key exchange, OCSP, etc. |

---

## TL;DR mental model

> HTTPS is HTTP over TLS. **TLS is a layer below HTTP that handles encryption, integrity, and server authentication.** Your HTTP server code doesn't change — the TLS handshake happens before any HTTP bytes flow. **For webserv: out of scope.** Real-world: terminate TLS at a reverse proxy (nginx), keep your application on plain HTTP.

**Continue to [`14_CGI.md`](14_CGI.md)** — the heaviest topic in the library, and one of the most important for webserv.
