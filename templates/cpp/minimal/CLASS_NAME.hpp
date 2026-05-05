#ifndef CLASS_NAME_HPP
# define CLASS_NAME_HPP

#include <string>

class CLASS_NAME {
    private:
        std::string m_name;

    public:
        CLASS_NAME(void);
        CLASS_NAME(std::string name);
        CLASS_NAME(const CLASS_NAME &src);
        CLASS_NAME& operator= (const CLASS_NAME &other);
        ~CLASS_NAME();
};

#endif
