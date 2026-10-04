#include <iostream>
#include <string>
#include <exception>
#include "errors.hpp"
#include "utils.hpp"
#include "Array.hpp"

static int g_failures = 0;

static void title(const std::string &name) {
    std::cout << std::endl << BOLD_CYAN << "--- " << name << " ---" << endofline;
}

static void check(bool ok, const std::string &what) {
    if (ok)
        std::cout << GREEN << "[PASS] " << RESET << what << std::endl;
    else {
        std::cout << RED << "[FAIL] " << RESET << what << std::endl;
        g_failures++;
    }
}

template <typename T>
static bool throwsOnIndex(Array<T> &arr, size_t i) {
    try {
        arr[i];
    }
    catch (const std::exception &) {
        return (true);
    }
    return (false);
}

template <typename T>
static void print(const Array<T> &arr) {
    std::cout << "{ ";
    for (size_t i = 0; i < arr.size(); i++)
        std::cout << arr[i] << " ";
    std::cout << "}" << std::endl;
}

static void testDefault() {
    title("default constructor: empty array");
    Array<int> empty;
    check(empty.size() == 0, "size() == 0");
    check(throwsOnIndex(empty, 0), "empty[0] throws");
}

static void testSized() {
    title("Array(n): n default-initialized elements");
    Array<int> ints(5);
    print(ints);
    check(ints.size() == 5, "size() == 5");

    bool allZero = true;
    for (size_t i = 0; i < ints.size(); i++)
        if (ints[i] != 0)
            allZero = false;
    check(allZero, "every int is 0 (value-initialized)");

    Array<std::string> strs(3);
    check(strs.size() == 3 && strs[0].empty(), "strings default to \"\"");
}

static void testReadWrite() {
    title("operator[]: read and write");
    Array<int> ints(5);
    for (size_t i = 0; i < ints.size(); i++)
        ints[i] = static_cast<int>(i * 10);
    print(ints);
    check(ints[0] == 0 && ints[4] == 40, "values written are read back");

    const Array<int> &ref = ints;
    check(ref[2] == 20, "const operator[] reads the same data");
}

static void testOutOfBounds() {
    title("operator[]: out of bounds throws std::exception");
    Array<int> ints(5);
    check(!throwsOnIndex(ints, 4), "ints[4] (last) does not throw");
    check(throwsOnIndex(ints, 5), "ints[5] (== size) throws");
    check(throwsOnIndex(ints, 1000), "ints[1000] throws");
    check(throwsOnIndex(ints, static_cast<size_t>(-1)), "ints[-1] (wraps to huge) throws");
}

static void testCopyConstructor() {
    title("copy constructor: deep copy");
    Array<int> original(3);
    original[0] = 1;
    original[1] = 2;
    original[2] = 3;

    Array<int> copy(original);
    copy[0] = 42;
    std::cout << "original: ";
    print(original);
    std::cout << "copy:     ";
    print(copy);
    check(copy.size() == original.size(), "same size");
    check(original[0] == 1, "modifying the copy leaves the original intact");
}

static void testAssignment() {
    title("assignment operator: deep copy");
    Array<std::string> a(2);
    a[0] = "hello";
    a[1] = "world";

    Array<std::string> b(5);
    b = a;
    b[1] = "42";
    std::cout << "a: ";
    print(a);
    std::cout << "b: ";
    print(b);
    check(b.size() == 2, "b takes a's size");
    check(a[1] == "world", "modifying b leaves a intact");

    Array<std::string> &alias = b;
    b = alias;
    check(b.size() == 2 && b[0] == "hello", "self-assignment keeps the data");
}

int main(void) {
    testDefault();
    testSized();
    testReadWrite();
    testOutOfBounds();
    testCopyConstructor();
    testAssignment();

    std::cout << std::endl;
    if (g_failures == 0)
        std::cout << BOLD_GREEN << "All tests passed" << endofline;
    else
        std::cout << BOLD_RED << g_failures << " test(s) failed" << endofline;
    return (g_failures == 0 ? SUCCESS : FAILURE);
}
