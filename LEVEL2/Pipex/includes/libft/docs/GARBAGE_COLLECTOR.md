# Libft Garbage Collector

This library includes a simple garbage collection (GC) system to simplify memory management in C. It works by tracking allocated memory pointers in a linked list, allowing you to free all allocated memory with a single function call.

## How it Works

The garbage collector uses a linked list (`t_list`) to store pointers to allocated memory. When you allocate memory using a GC function (e.g., `ft_gc_malloc`), the pointer is automatically added to this list. When you're done, you call `ft_gc_free_all` to free every pointer in the list and the list nodes themselves.

## Basic Usage

### 1. Initialize the GC Head
You need a `t_list` pointer to serve as the head of your garbage collector. Initialize it to `NULL`.

```c
t_list *gc = NULL;
```

### 2. Allocate Memory
Use the `_gc` suffixed functions instead of standard allocation functions. Pass the address of your GC head (`&gc`) as the last argument.

```c
// Instead of malloc(100)
char *str = ft_gc_malloc(100, &gc);

// Instead of ft_strdup("hello")
char *dup = ft_strdup_gc("hello", &gc);
```

### 3. Cleanup
When the program ends or when you want to clear the memory scope, call `ft_gc_free_all`.

```c
ft_gc_free_all(&gc);
// gc is now NULL, and all memory allocated associated with it is freed.
```

## Available Functions

The following functions are available for GC-managed operations:

**Allocation:**
*   `void *ft_gc_malloc(size_t size, t_list **gc_head)`
*   `void *ft_calloc_gc(size_t count, size_t size, t_list **gc_head)`

**String Manipulation:**
*   `char *ft_strdup_gc(const char *str, t_list **gc_head)`
*   `char *ft_strndup_gc(const char *s, size_t n, t_list **gc_head)`
*   `char *ft_substr_gc(char const *s, unsigned int start, size_t len, t_list **gc_head)`
*   `char *ft_strjoin_gc(const char *s1, const char *s2, t_list **gc_head)`
*   `char *ft_strtrim_gc(char const *s1, char const *set, t_list **gc_head)`
*   `char *ft_strmapi_gc(char const *s, char (*f)(unsigned int, char), t_list **gc_head)`
*   `char **ft_split_charset_gc(char const *s, char *charset, t_list **gc_head)`
*   `char **ft_split_sep_gc(char const *s, char sep, t_list **gc_head)`

**Conversion:**
*   `char *ft_itoa_gc(int n, t_list **gc_head)`

**List Manipulation:**
*   `t_list *ft_lstnew_gc(void *content, t_list **gc_head)`
*   `t_list *ft_lstmap_gc(t_list *lst, void *(*f)(void *), void (*del)(void *), t_list **gc_head)`

## Manual Tracking

If you have a pointer allocated via standard `malloc` (or an external library) that you want the GC to manage, you can add it manually using `ft_gc_add_node`.

```c
char *ptr = malloc(100);
if (ft_gc_add_node(&gc, ptr) == 1)
{
    // Handle error (GC node allocation failed)
    free(ptr);
    ft_gc_free_all(&gc);
    return (NULL);
}
```

## Example

```c
#include "libft.h"

int main(void)
{
    t_list *gc = NULL; // Initialize GC

    // Allocation
    char *str = ft_gc_malloc(50, &gc);
    
    // Using GC-aware functions
    char *joined = ft_strjoin_gc("Hello", " World", &gc);
    
    // Splitting a string
    char **split = ft_split_sep_gc("one,two,three", ',', &gc);

    // ... use your variables ...

    // Free EVERYTHING at once
    ft_gc_free_all(&gc);

    return (0);
}
```
