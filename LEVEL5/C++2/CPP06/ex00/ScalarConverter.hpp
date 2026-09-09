#ifndef SCALARCONVERTER_HPP
# define SCALARCONVERTER_HPP

#include <string>

class ScalarConverter {
    private:
        std::string _name;
        ScalarConverter(std::string name);
        ScalarConverter(const ScalarConverter &src);
        ScalarConverter& operator= (const ScalarConverter &other);
        ~ScalarConverter();

    public:
};

#endif
