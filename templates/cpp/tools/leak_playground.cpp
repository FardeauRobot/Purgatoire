// =============================================================================
//  leak_playground.cpp — hands-on memory bugs you should be able to spot
// -----------------------------------------------------------------------------
//  Each bug below is enclosed in `#ifdef BUG_<n> ... #endif`. Compile with the
//  bug you want to explore, then run it through the leak checker:
//
//      c++ -std=c++98 -Wall -Wextra -g -DBUG_1 leak_playground.cpp -o play
//      ../tools/leakcheck.sh -v ./play
//
//  Or with AddressSanitizer (faster feedback for heap-bug categories):
//
//      c++ -std=c++98 -g -fsanitize=address,undefined -DBUG_1 \
//          leak_playground.cpp -o play && ./play
//
//  Bugs are deliberately small and self-contained. Read the comment above
//  each one BEFORE running it, then verify the tool tells you what the
//  comment says it should. That's the whole point: build the mental link
//  between "this kind of mistake" -> "this kind of report".
// =============================================================================

#include <iostream>
#include <string>
#include <stdexcept>

// -----------------------------------------------------------------------------
//  BUG_1 — Forgotten delete (the canonical leak)
//  Tool says: "definitely lost" / "leaked block of N bytes".
//  Lesson:    every `new` needs a matching `delete`. No exceptions.
// -----------------------------------------------------------------------------
#ifdef BUG_1
int main(void) {
    int* p = new int(42);
    std::cout << "value = " << *p << std::endl;
    // forgot: delete p;
    return 0;
}
#endif

// -----------------------------------------------------------------------------
//  BUG_2 — Wrong delete form: `new[]` paired with plain `delete`
//  Tool says: "mismatched free/delete" (valgrind) or "incorrect checksum"
//             (macOS malloc) — sometimes silent corruption with ASan
//             reporting "alloc-dealloc-mismatch".
//  Lesson:    `new[]` MUST be matched with `delete[]`. The compiler can't
//             warn you because the type is the same (`int*`) either way.
// -----------------------------------------------------------------------------
#ifdef BUG_2
int main(void) {
    int* arr = new int[10];
    // wrong form:
    delete arr;        // should be: delete[] arr;
    return 0;
}
#endif

// -----------------------------------------------------------------------------
//  BUG_3 — Double-delete
//  Tool says: ASan "double-free", or glibc "double free or corruption".
//  Lesson:    once a pointer is deleted, the memory is no longer yours.
//             Set the pointer to NULL after deleting if you can't prove the
//             rest of the function won't touch it again.
// -----------------------------------------------------------------------------
#ifdef BUG_3
int main(void) {
    int* p = new int(7);
    delete p;
    delete p;          // boom
    return 0;
}
#endif

// -----------------------------------------------------------------------------
//  BUG_4 — Use-after-delete (dangling pointer)
//  Tool says: ASan "heap-use-after-free". Without ASan: silent garbage,
//             or a crash, depending on what the allocator did with the slot.
//  Lesson:    `delete` does not zero the pointer. The variable still holds
//             an address — but the address is no longer valid.
// -----------------------------------------------------------------------------
#ifdef BUG_4
int main(void) {
    int* p = new int(123);
    delete p;
    std::cout << *p << std::endl;   // reading freed memory
    return 0;
}
#endif

// -----------------------------------------------------------------------------
//  BUG_5 — Shallow copy + double-free (the Rule of Three lesson)
//  Tool says: double-free at program exit when both copies destruct.
//  Lesson:    If your class holds a raw pointer it owns, you MUST define
//             a copy constructor and copy assignment operator that copy
//             the *content*, not just the pointer. Otherwise the default
//             compiler-generated copy gives two objects sharing one
//             allocation — and they'll both try to delete it.
// -----------------------------------------------------------------------------
#ifdef BUG_5
class Buffer {
public:
    Buffer() : data(new int[4]) {}
    ~Buffer() { delete[] data; }
    // missing: Buffer(const Buffer&); operator=(const Buffer&);
private:
    int* data;
};

int main(void) {
    Buffer a;
    Buffer b = a;       // shallow copy: a.data == b.data
    return 0;           // both destructors run -> delete[] same pointer twice
}
#endif

// -----------------------------------------------------------------------------
//  BUG_6 — Returning a pointer to a local stack variable
//  Tool says: ASan "stack-use-after-return". Often crashes or prints garbage
//             without a sanitizer.
//  Lesson:    a local variable's storage ends with the function. Returning
//             its address gives the caller a pointer into reused stack space.
//             Either return by value, or allocate on the heap with `new`.
// -----------------------------------------------------------------------------
#ifdef BUG_6
int* makeNumber(void) {
    int x = 99;
    return &x;          // address of stack-local — invalid the moment we return
}

int main(void) {
    int* p = makeNumber();
    std::cout << *p << std::endl;
    return 0;
}
#endif

// -----------------------------------------------------------------------------
//  BUG_7 — Heap buffer overflow (off-by-one)
//  Tool says: ASan "heap-buffer-overflow"; valgrind "Invalid write of size 4".
//  Lesson:    `new int[N]` gives you indices 0..N-1. Index N is one past the
//             end, and writing there corrupts the allocator's bookkeeping
//             or the next allocation.
// -----------------------------------------------------------------------------
#ifdef BUG_7
int main(void) {
    int* arr = new int[10];
    for (int i = 0; i <= 10; i++)   // <= should be <
        arr[i] = i;
    delete[] arr;
    return 0;
}
#endif

// -----------------------------------------------------------------------------
//  BUG_8 — Uninitialised read
//  Tool says: valgrind "Conditional jump or move depends on uninitialised
//             value(s)" (with --track-origins=yes pointing at the malloc).
//             ASan only catches some forms via MSan; relies on UBSan for the
//             obvious ones.
//  Lesson:    `new int` (no parens, no initialiser) leaves the int with
//             indeterminate value. Use `new int(0)` or `new int()` for
//             value-initialisation, or assign before reading.
// -----------------------------------------------------------------------------
#ifdef BUG_8
int main(void) {
    int* p = new int;            // uninitialised
    if (*p > 0)                  // reads garbage
        std::cout << "positive" << std::endl;
    delete p;
    return 0;
}
#endif

// -----------------------------------------------------------------------------
//  BUG_9 — Leak when a constructor throws
//  Tool says: leaked block (the `int[100]` is never freed).
//  Lesson:    if a constructor throws after acquiring a resource, the
//             destructor will NOT run (the object was never fully
//             constructed). The fix is RAII: wrap each owned resource in
//             its own object whose destructor releases it. C++98 idiom is
//             a small dedicated holder; modern C++ uses smart pointers.
// -----------------------------------------------------------------------------
#ifdef BUG_9
class Risky {
public:
    Risky() : a(new int[100]), b(new int[100]) {
        throw std::runtime_error("boom");   // a leaks; b never allocated
    }
    ~Risky() { delete[] a; delete[] b; }    // never called for *this*
private:
    int* a;
    int* b;
};

int main(void) {
    try { Risky r; }
    catch (std::exception& e) { std::cout << "caught: " << e.what() << std::endl; }
    return 0;
}
#endif

// -----------------------------------------------------------------------------
//  BUG_10 — Self-assignment without check
//  Tool says: ASan "heap-use-after-free" inside operator=, OR garbage output.
//  Lesson:    in `a = a;`, naive operator= deletes the buffer THEN tries to
//             copy from it — but it just freed the source. Always:
//                 if (this == &other) return *this;
//             at the top of operator=. (Or use copy-and-swap.)
// -----------------------------------------------------------------------------
#ifdef BUG_10
class Str {
public:
    Str(const char* s) {
        len = 0; while (s[len]) len++;
        data = new char[len + 1];
        for (size_t i = 0; i <= len; i++) data[i] = s[i];
    }
    ~Str() { delete[] data; }

    Str& operator=(const Str& other) {
        // BUG: missing self-assignment check
        delete[] data;                      // if &other == this, we just freed other.data too
        len = other.len;
        data = new char[len + 1];
        for (size_t i = 0; i <= len; i++)
            data[i] = other.data[i];        // reading freed memory when self-assigning
        return *this;
    }
private:
    char*  data;
    size_t len;
    Str(const Str&);                        // disallow copy-ctor (focus the bug on operator=)
};

int main(void) {
    Str s("hello");
    s = s;                                  // the trap
    return 0;
}
#endif

// -----------------------------------------------------------------------------
//  No bug selected: print usage.
// -----------------------------------------------------------------------------
#if !defined(BUG_1)  && !defined(BUG_2) && !defined(BUG_3) && !defined(BUG_4) \
 && !defined(BUG_5)  && !defined(BUG_6) && !defined(BUG_7) && !defined(BUG_8) \
 && !defined(BUG_9)  && !defined(BUG_10)
int main(void) {
    std::cout <<
        "leak_playground: pick a bug with -DBUG_<n>, e.g.:\n"
        "    c++ -std=c++98 -g -DBUG_1 leak_playground.cpp -o play\n"
        "Then run it under a leak checker:\n"
        "    ../tools/leakcheck.sh -v ./play\n"
        "Available bugs: BUG_1 .. BUG_10 (see source comments)\n";
    return 0;
}
#endif
