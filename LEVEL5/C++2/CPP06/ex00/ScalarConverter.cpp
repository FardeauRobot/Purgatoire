#include <iostream>
#include "ScalarConverter.hpp"
#include "utils.hpp"

ScalarConverter::ScalarConverter(std::string name): _name(name) {
    std::cout << BOLD_CYAN << "ScalarConverter Name constructor called" << endofline;
}

ScalarConverter::ScalarConverter(const ScalarConverter &src) {
    std::cout << BOLD_BLUE << "ScalarConverter Copy constructor called" << endofline;
    *this = src;
}

ScalarConverter& ScalarConverter::operator= (const ScalarConverter &other) {
    std::cout << BOLD_BLUE << "ScalarConverter Copy assignment operator called" << endofline;
    if (this != &other)
        _name = other._name;
    return (*this);
}

ScalarConverter::~ScalarConverter() {
    std::cout << BOLD_RED << "ScalarConverter Destructor called" << endofline;
}
