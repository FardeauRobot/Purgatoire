#include "utils.hpp"

std::ostream& endofline(std::ostream& os) {
    return os << RESET << std::endl;
}

void title(const std::string &name) {
    std::cout << std::endl << BOLD_CYAN << "--- " << name << " ---" << endofline;
}

