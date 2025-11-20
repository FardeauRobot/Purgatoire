# Libft 🔧

A compact C utility library aligned with the 42 curriculum. `libft` exposes safe, familiar helpers from <ctype.h>, <string.h>, and <stdlib.h>. All routines live in `libft.h` and compile into the static `libft.a` archive so you can reuse them across projects without dragging in an entire system header set.

Each code block below uses a fixed-width layout so every line has the same number of characters, making the reference easy to scan in raw form.

## Function reference

### Character classification & case conversion
```text
| Function         | Signature                                  | Description                                                                      |
| ---------------- | ------------------------------------------ | -------------------------------------------------------------------------------- |
| ft_isalnum       | int ft_isalnum(int c)                      | Truthy for ASCII letters or digits.                                              |
| ft_isalpha       | int ft_isalpha(int c)                      | Truthy for ASCII letters only.                                                   |
| ft_isascii       | int ft_isascii(int c)                      | Limits checks to the 7-bit ASCII range.                                          |
| ft_isdigit       | int ft_isdigit(int c)                      | Truthy for ASCII digits '0'..'9'.                                                |
| ft_isprint       | int ft_isprint(int c)                      | Truthy for printable characters (space included).                                |
| ft_tolower       | int ft_tolower(int c)                      | Downcases ASCII letters; others stay unchanged.                                  |
| ft_toupper       | int ft_toupper(int c)                      | Upcases ASCII letters; others stay unchanged.                                    |
```

### String utilities
```text
| Function         | Signature                                  | Description                                                                      |
| ---------------- | ------------------------------------------ | -------------------------------------------------------------------------------- |
| ft_strlen        | size_t ft_strlen(const char *s)            | Returns the length of a NUL-terminated string.                                   |
| ft_strdup        | char *ft_strdup(const char *s1)            | Allocates and copies the provided string.                                        |
| ft_strchr        | char *ft_strchr(const char *s, int c)      | Finds the first occurrence of c.                                                 |
| ft_strrchr       | char *ft_strrchr(const char *s, int c)     | Finds the last occurrence of c.                                                  |
| ft_strncmp       | int ft_strncmp(const char *s1, const char *s2, size_t n) | Compares up to n bytes lexicographically.                                        |
| ft_strnstr       | char *ft_strnstr(const char *haystack, const char *needle, size_t len) | Searches needle within len bytes of haystack.                                    |
| ft_strlcpy       | size_t ft_strlcpy(char *dst, const char *src, size_t dstsize) | Safely copies src into dst and always NUL-terminates when dstsize > 0.           |
| ft_strlcat       | size_t ft_strlcat(char *dst, const char *src, size_t dstsize) | Appends src to dst while respecting dstsize and returning the total length.      |
| ft_substr        | char *ft_substr(char const *s, unsigned int start, size_t len) | Allocates a substring starting at start up to len characters.                    |
| ft_strjoin       | char *ft_strjoin(char const *s1, char const *s2) | Concatenates s1 and s2 into one allocation.                                      |
| ft_strtrim       | char *ft_strtrim(char const *s1, char const *set) | Trims characters present in set from both ends of s1.                            |
| ft_split         | char **ft_split(char const *s, char c)     | Splits s on delimiter c, returning a NULL-terminated array.                      |
| ft_strmapi       | char *ft_strmapi(char const *s, char (*f)(unsigned int, char)) | Builds a new string by applying f to each char with its index.                   |
| ft_striteri      | void ft_striteri(char *s, void (*f)(unsigned int, char *)) | Calls f in-place on each char while providing the index.                         |
```

### Memory management
```text
| Function         | Signature                                  | Description                                                                      |
| ---------------- | ------------------------------------------ | -------------------------------------------------------------------------------- |
| ft_memset        | void *ft_memset(void *b, int c, size_t len) | Fills len bytes of b with c.                                                     |
| ft_bzero         | void ft_bzero(void *s, size_t n)           | Zeroes n bytes starting at s.                                                    |
| ft_memcpy        | void *ft_memcpy(void *dest, const void *src, size_t n) | Copies n bytes; overlapping ranges have undefined behavior.                      |
| ft_memmove       | void *ft_memmove(void *dst, const void *src, size_t len) | Copies len bytes safely when ranges overlap.                                     |
| ft_memchr        | void *ft_memchr(const void *s, int c, size_t n) | Searches the first n bytes of s for c.                                           |
| ft_memcmp        | int ft_memcmp(const void *s1, const void *s2, size_t n) | Compares the first n bytes of s1 and s2.                                         |
| ft_calloc        | void *ft_calloc(size_t count, size_t size) | Allocates count * size bytes and zeroes them, guarding overflow.                 |
```

### Conversion utilities
```text
| Function         | Signature                                  | Description                                                                      |
| ---------------- | ------------------------------------------ | -------------------------------------------------------------------------------- |
| ft_atoi          | int ft_atoi(const char *nptr)              | Parses nptr as a signed decimal integer (whitespace/sign aware).                 |
| ft_itoa          | char *ft_itoa(int n)                       | Converts an integer to its decimal string representation.                        |
```

### File descriptor output helpers
```text
| Function         | Signature                                  | Description                                                                      |
| ---------------- | ------------------------------------------ | -------------------------------------------------------------------------------- |
| ft_putchar_fd    | void ft_putchar_fd(char c, int fd)         | Writes the character c to the provided file descriptor.                          |
| ft_putstr_fd     | void ft_putstr_fd(char *s, int fd)         | Writes the string s to fd.                                                       |
| ft_putendl_fd    | void ft_putendl_fd(char *s, int fd)        | Writes s followed by a newline to fd.                                            |
| ft_putnbr_fd     | void ft_putnbr_fd(int n, int fd)           | Writes the decimal digits of n to fd.                                            |
```

### Linked list helpers (bonus)
```text
| Function         | Signature                                  | Description                                                                      |
| ---------------- | ------------------------------------------ | -------------------------------------------------------------------------------- |
| ft_lstnew        | t_list *ft_lstnew(void *content)           | Creates a node whose content is content and next = NULL.                         |
| ft_lstsize       | int ft_lstsize(t_list *lst)                | Counts nodes in lst.                                                             |
| ft_lstlast       | t_list *ft_lstlast(t_list *lst)            | Returns the last node or NULL when empty.                                        |
| ft_lstadd_front  | void ft_lstadd_front(t_list **lst, t_list *new) | Prepends new to the list stored at *lst.                                         |
| ft_lstadd_back   | void ft_lstadd_back(t_list **lst, t_list *new) | Appends new to the end of *lst.                                                  |
| ft_lstdelone     | void ft_lstdelone(t_list *lst, void (*del)(void *)) | Frees lst and applies del to its content.                                        |
| ft_lstclear      | void ft_lstclear(t_list **lst, void (*del)(void *)) | Clears the entire list and NULLs *lst.                                           |
| ft_lstiter       | void ft_lstiter(t_list *lst, void (*f)(void *)) | Runs f on each node content.                                                     |
| ft_lstmap        | t_list *ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *)) | Maps f over lst, building a new list and using del on failure.                   |
```

## Building & usage

Use the provided Makefile with `cc -Wall -Wextra -Werror`:

```sh
make        # build mandatory helpers into libft.a
make bonus  # build bonus linked-list helpers as well
make clean  # remove *.o files
make fclean # delete *.o files and libft.a
make re     # fclean + make
```

> 💡 Link `libft.a` alongside your sources and include `libft.h` so the compiler resolves the helper declarations:

```sh
cc main.c libft.a -I. -o my_program
```
