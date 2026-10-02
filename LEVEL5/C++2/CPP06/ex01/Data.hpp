#ifndef DATA_HPP
# define DATA_HPP

# include <string>

class Data {
    public:
        int         id;
        std::string label;
        double      value;

        Data();
        Data(int id, const std::string &label, double value);
        Data(const Data &src);
        Data &operator= (const Data &other);
        ~Data();
};

#endif
