#include <iostream>
#include "Serializer.hpp"
#include "Data.hpp"
#include "utils.hpp"

int main(void) {
    Data original(42, "answer", 3.14);
    Data *originalPtr = &original;

    uintptr_t raw = Serializer::serialize(originalPtr);
    Data *restoredPtr = Serializer::deserialize(raw);

    std::cout << BOLD_CYAN << "Original pointer:  " << originalPtr << endofline;
    std::cout << BOLD_YELLOW << "Serialized value:  " << raw << endofline;
    std::cout << BOLD_BLUE << "Restored pointer:  " << restoredPtr << endofline;

    return 0;
}
