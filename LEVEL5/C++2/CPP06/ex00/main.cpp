#include "ScalarConverter.hpp"
#include "utils.hpp"

int main(int argc , char **argv) {

    if (argc != 2) {
            std::cout << "Usage example: ./convert 42" << endofline;
            return (1);
    }
    ScalarConverter::convert(argv[1]);

    return 0;
}
