#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <string>
#include "utils.hpp"

class Location {
    private:
        std::string                 m_name;
        std::string                 m_root_path;
        std::string                 m_index_path;
        std::vector<e_methods>      m_allowed_methods;
        // size_t                      m_max_body_size;
        // bool                        m_autoindex;

    public:
        Location(void);
        Location(std::string name);
        Location(const Location &src);
        Location& operator= (const Location &other);
        ~Location();
};

#endif
