# Filesystem — `open`, `close`, `stat`, `access`, `opendir`, `readdir`, `closedir`

> *"Every GET request for a static file is a filesystem journey: check it exists, check you can read it, figure out its type, open it, stream it, close it."*

These functions are the bridge between your HTTP router and the actual files on disk. They cover three workflows in webserv:
1. **Serving a static file** — `stat`, `access`, `open`, `close`
2. **Directory listing** — `opendir`, `readdir`, `closedir`
3. **Checking existence before DELETE** — `stat`, `access`

---

## `stat()` — inspect a file without opening it

```c
#include <sys/stat.h>

int stat(const char *pathname, struct stat *statbuf);
// Returns: 0 on success, -1 on error (errno set)
```

**Parameters:**
- `pathname` — path to the file or directory to inspect; can be absolute or relative; follows symbolic links (use `lstat()` to inspect the link itself)
- `statbuf` — output: pointer to a `struct stat` that the kernel fills with file metadata (size, type, permissions, timestamps)

**What it does:** fills a `struct stat` with metadata about the file at `pathname`. Does not open the file. Does not read any content. The kernel reads the file's inode (a fixed-size metadata block on disk) and copies it to your struct.

**The `struct stat` fields you'll use:**

```c
struct stat {
    mode_t  st_mode;   // file type + permissions (S_IFREG, S_IFDIR, ...)
    off_t   st_size;   // file size in bytes (needed for Content-Length)
    time_t  st_mtime;  // last modification time (for Last-Modified / ETag)
    // ... more fields you won't use in basic webserv
};
```

**Webserv use — before serving a file:**

```cpp
struct stat file_stat;
std::string full_path = root + requested_path;  // e.g. "/var/www/index.html"

if (stat(full_path.c_str(), &file_stat) < 0) {
    if (errno == ENOENT)  send_error(client_fd, 404); // Not Found
    if (errno == EACCES)  send_error(client_fd, 403); // Forbidden
    return;
}

// Is it a directory?
if (S_ISDIR(file_stat.st_mode)) {
    // Try index file first, then autoindex if enabled
    handle_directory(client_fd, full_path, config);
    return;
}

// Is it a regular file?
if (!S_ISREG(file_stat.st_mode)) {
    send_error(client_fd, 403);   // device file, socket, etc.
    return;
}

// Use size for Content-Length header
std::string content_length = to_string(file_stat.st_size);

// Use mtime for Last-Modified header
char mtime_buf[64];
struct tm *tm_info = gmtime(&file_stat.st_mtime);
strftime(mtime_buf, sizeof(mtime_buf), "%a, %d %b %Y %H:%M:%S GMT", tm_info);
```

**`st_mode` bitmask helpers:**

```cpp
S_ISREG(st_mode)   // true if regular file
S_ISDIR(st_mode)   // true if directory
S_ISLNK(st_mode)   // true if symbolic link (stat() follows symlinks; lstat() doesn't)

// Permission bits:
st_mode & S_IRUSR  // owner read
st_mode & S_IROTH  // other read — is the file world-readable?
```

**Under the hood:** `stat()` triggers a filesystem lookup. On disk, each file has an **inode** — a block of metadata (size, timestamps, owner, permissions, the list of data blocks). `stat()` reads this inode. It does not read any data blocks. On a fast SSD this is microseconds; over NFS it could be milliseconds. Point is: it's an I/O operation, not just memory.

---

## `access()` — check file permissions

```c
#include <unistd.h>

int access(const char *pathname, int mode);
// Returns: 0 if access is permitted, -1 otherwise
```

**Parameters:**
- `pathname` — path to the file or directory to check; follows symbolic links
- `mode` — permission flags to test, combined with `|`: `F_OK` (exists), `R_OK` (readable), `W_OK` (writable), `X_OK` (executable by the current process's effective user)

**Mode flags:**

| Flag | Meaning |
|---|---|
| `F_OK` | file exists |
| `R_OK` | readable by current process |
| `W_OK` | writable |
| `X_OK` | executable |

**In webserv:**

```cpp
// Before serving: quick check
if (access(full_path.c_str(), F_OK | R_OK) < 0) {
    send_error(client_fd, errno == ENOENT ? 404 : 403);
    return;
}

// Before executing CGI interpreter
if (access(cgi_interpreter.c_str(), X_OK) < 0) {
    send_error(client_fd, 500);   // misconfigured CGI
    return;
}

// Before DELETE
if (access(target_path.c_str(), F_OK | W_OK) < 0) {
    send_error(client_fd, errno == ENOENT ? 404 : 403);
    return;
}
```

**`stat()` vs `access()`:** `stat()` gives you full metadata; `access()` just checks permission. For webserv: use `stat()` first (you need size and type anyway), then derive permission from `st_mode`. Use `access()` for quick checks where you don't need the other metadata — e.g., checking CGI interpreter exists and is executable.

**TOCTOU warning (Time Of Check, Time Of Use):** there's a classic race condition between checking and using:

```
access()  → returns "file readable"
              [attacker replaces file with symlink to /etc/passwd]
open()    → opens /etc/passwd
```

For a 42 webserv in a controlled environment, this is not a concern. But in production: always use the POSIX `openat()`/`faccessat()` family.

---

## `open()` — get a file descriptor for a file

```c
#include <fcntl.h>

int open(const char *pathname, int flags, ...);
// Returns: a new fd on success, -1 on error
```

**Parameters:**
- `pathname` — path to the file to open
- `flags` — how to open the file, combined with `|`: `O_RDONLY` (read), `O_WRONLY` (write), `O_CREAT` (create if absent), `O_TRUNC` (truncate to zero), `O_CLOEXEC` (auto-close in child processes)
- `...` — optional `mode_t` argument (e.g., `0644`); required when `O_CREAT` is set to specify permission bits for the new file; ignored for existing files

**All `open()` flags:**

| Flag | Use |
|---|---|
| `O_RDONLY` | Open for reading only; incompatible with `O_WRONLY` and `O_RDWR` |
| `O_WRONLY` | Open for writing only |
| `O_RDWR` | Open for both reading and writing |
| `O_CREAT` | Create the file if it doesn't exist; requires the third `mode` argument (e.g. `0644`) |
| `O_TRUNC` | If the file exists and is writable, truncate it to zero length on open; used for file uploads that overwrite |
| `O_APPEND` | All writes go to the end of the file atomically; useful for log files |
| `O_EXCL` | Combined with `O_CREAT`: fail with `EEXIST` if the file already exists; guarantees atomic create-or-fail with no race condition |
| `O_CLOEXEC` | Close this fd automatically in child processes after `fork()` + `exec()`; prevents file descriptors leaking into CGI children |
| `O_NONBLOCK` | For FIFOs and device files: don't block on `open()` if no reader/writer is present yet; for regular files has no effect |
| `O_DIRECTORY` | Fail with `ENOTDIR` if the path is not a directory; use when you intend to open a directory (e.g. for `readdir`) and want to reject regular files |
| `O_NOFOLLOW` | Fail with `ELOOP` if the final path component is a symbolic link; prevents symlink traversal attacks where an attacker replaces a file with a symlink to `/etc/passwd` |
| `O_SYNC` | Every `write()` blocks until data is physically written to disk (not just kernel buffer); very slow; used for crash-safe file writes |
| `O_DSYNC` | Like `O_SYNC` but only waits for data, not metadata (timestamps etc.) — slightly faster |

**Serving a static file:**

```cpp
int file_fd = open(full_path.c_str(), O_RDONLY | O_CLOEXEC);
if (file_fd < 0) {
    send_error(client_fd, errno == EACCES ? 403 : 404);
    return;
}

// Build response headers first
std::ostringstream headers;
headers << "HTTP/1.1 200 OK\r\n"
        << "Content-Type: "   << get_mime_type(full_path) << "\r\n"
        << "Content-Length: " << file_stat.st_size         << "\r\n"
        << "Connection: "     << (keep_alive ? "keep-alive" : "close") << "\r\n"
        << "\r\n";
send(client_fd, headers.str().c_str(), headers.str().size(), 0);

// Stream file content directly
char buf[65536];
ssize_t n;
while ((n = read(file_fd, buf, sizeof(buf))) > 0)
    send(client_fd, buf, n, 0);

close(file_fd);
```

**For file uploads (POST to upload route):**

```cpp
// Create/overwrite the uploaded file
int out_fd = open(upload_path.c_str(),
                  O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC,
                  0644);   // mode: rw-r--r--
if (out_fd < 0) {
    send_error(client_fd, 500);
    return;
}
write(out_fd, request_body.c_str(), request_body.size());
close(out_fd);
send_response(client_fd, 201, "Created");
```

**Under the hood:** `open()` creates an entry in the kernel's **open-file table** (distinct from the per-process fd table). The open-file entry tracks the file offset (how far you've read), flags, and a reference to the inode. Your per-process fd table gets an index pointing to this entry. `dup()` creates a second index to the same entry (shared offset). `open()` a second time creates a separate entry (independent offset).

---

## `close()` — release a file descriptor

```c
#include <unistd.h>

int close(int fd);
// Returns: 0 on success, -1 on error
```

**Parameters:**
- `fd` — the file descriptor to release; decrements the reference count on the underlying open-file description; when the count reaches zero the kernel flushes and frees it; passing an already-closed or invalid fd is undefined behavior

**The resource lifecycle rule:** every fd you `open()`, `socket()`, `accept()`, `pipe()`, or `dup()` must be `close()`d exactly once. Failing to close is a file descriptor leak. Under stress (thousands of requests), this exhausts the per-process fd limit (typically 1024 on macOS unless raised) and subsequent `open()`/`accept()` calls fail with `EMFILE`.

**For webserv, the fd checklist:**
- `close(listen_fd)` at server shutdown
- `close(client_fd)` when connection done or error
- `close(file_fd)` after serving a file
- `close(pipefd_in[0])` and `close(pipefd_in[1])` after CGI (whichever end each process holds)
- `close(pipefd_out[0])` and `close(pipefd_out[1])` after CGI
- `close(opendir_fd)` — handled by `closedir()` for directories

**Common close mistakes:**

```cpp
// Double-close — undefined behavior, could close a different fd that got the same number
close(fd);
close(fd);  // WRONG

// Forgetting to close on error paths
int fd = open(path, O_RDONLY);
if (fd < 0) { send_error(client_fd, 404); return; }
// ... do work ...
if (some_error) {
    send_error(client_fd, 500);
    return;  // WRONG — fd is leaked. Should: close(fd); return;
}
close(fd);  // only reached on the happy path
```

**Safe pattern:** close as soon as you no longer need the fd, and use a local variable style that makes it obvious when the fd goes out of scope.

---

## `opendir()`, `readdir()`, `closedir()` — directory listing (autoindex)

```c
#include <dirent.h>

DIR *opendir(const char *name);
// Returns: directory stream pointer, or NULL on error

struct dirent *readdir(DIR *dirp);
// Returns: next entry, or NULL when done (no more entries) or on error

int closedir(DIR *dirp);
// Returns: 0 on success, -1 on error
```

**`opendir` parameters:**
- `name` — path to the directory to open; returns `NULL` on error (not a directory, permission denied, path not found)

**`readdir` parameters:**
- `dirp` — the directory stream returned by `opendir()`; each call advances the internal position by one entry and returns a pointer to a static `struct dirent` (overwritten on the next call — copy `d_name` if you need to keep it)

**`closedir` parameters:**
- `dirp` — the directory stream to close; also releases the underlying fd; do not use `dirp` after this call

**The `struct dirent`:**

```c
struct dirent {
    ino_t  d_ino;        // inode number
    char   d_name[256];  // filename (null-terminated)
    // more fields on some systems
};
```

**Autoindex implementation:**

When a request targets a directory, autoindex is enabled in the config, and no index file exists, you generate an HTML directory listing. `readdir()` gives you the raw filenames; you combine with `stat()` to get sizes and types.

```cpp
std::string generate_directory_listing(const std::string& dir_path,
                                       const std::string& url_path) {
    DIR *dir = opendir(dir_path.c_str());
    if (!dir)
        return "";  // caller should send 403

    std::ostringstream html;
    html << "<!DOCTYPE html><html><head><title>Index of " << url_path << "</title></head>\n"
         << "<body><h1>Index of " << url_path << "</h1><hr><pre>\n";

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;
        if (name == ".")
            continue;  // skip current dir entry

        std::string full = dir_path + "/" + name;
        struct stat st;
        stat(full.c_str(), &st);

        if (S_ISDIR(st.st_mode))
            name += "/";  // visual cue for directories

        html << "<a href=\"" << url_path;
        if (url_path.back() != '/') html << "/";
        html << name << "\">" << name << "</a>";

        if (S_ISREG(st.st_mode))
            html << "\t" << st.st_size << " bytes";

        html << "\n";
    }

    html << "</pre><hr></body></html>";
    closedir(dir);
    return html.str();
}
```

**Under the hood:** `opendir()` opens the directory as a stream (a special file descriptor managed inside the `DIR` struct). `readdir()` reads directory entries one at a time, which on disk is sequential reads of the directory's data blocks (a directory is just a file mapping names to inode numbers). `closedir()` closes the underlying fd.

**Important:** `readdir()` is not guaranteed to return entries in alphabetical order. If you want sorted output (nginx-style), collect all names into a vector, sort it, then generate HTML.

**`readdir()` thread safety:** `readdir()` returns a pointer to an internal struct that is overwritten on the next call. In a single-threaded server (which webserv is), this is fine. In a multi-threaded environment you'd use `readdir_r()`.

**`.` and `..` entries:** `readdir()` always returns `.` (current directory) and `..` (parent directory) as the first two entries. Filter them out explicitly or your listing looks ugly.

---

## The static file serving pipeline — assembled

```
GET /images/photo.jpg HTTP/1.1

1. Parse request → path = "/images/photo.jpg"
2. full_path = root + "/" + path   → "/var/www/images/photo.jpg"
3. stat(full_path) → get size, check type (S_ISREG? S_ISDIR?)
4. S_ISREG → continue; S_ISDIR → handle_directory(); else → 403
5. access(full_path, R_OK) → check permissions → 403 if denied
6. open(full_path, O_RDONLY) → get file_fd
7. Determine Content-Type from extension  → "image/jpeg"
8. Build and send HTTP response headers (200, Content-Type, Content-Length)
9. read(file_fd) in a loop → send(client_fd) each chunk
10. close(file_fd)
```

---

## Related
- [`03_DATA_TRANSFER.md`](03_DATA_TRANSFER.md) — `read` and `send` for streaming the file bytes
- [`07_ERRORS.md`](07_ERRORS.md) — `strerror(errno)` for logging open/stat failures
- [`../03_METHODS.md`](../03_METHODS.md) — GET (serves files), DELETE (removes them), POST (creates them)
- [`../08_URLS.md`](../08_URLS.md) — URL-to-path mapping (root + path, ../ traversal protection)
