#include "Data.hpp"

Data::Data(): id(0), label(""), value(0.0) {}

Data::Data(int id, const std::string &label, double value): id(id), label(label), value(value) {}

Data::Data(const Data &src) {
    *this = src;
}

Data &Data::operator= (const Data &other) {
    if (this != &other) {
        id = other.id;
        label = other.label;
        value = other.value;
    }
    return (*this);
}

Data::~Data() {}
