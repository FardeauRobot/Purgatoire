#ifndef ERRORS_HPP
# define ERRORS_HPP

# include <string>

enum E_Errors
{
    SUCCESS = 0,
    FAILURE = 1
};

int  F_ErrMsg(const std::string& msg);

void F_WarnMsg(const std::string& msg);

void F_ErrExit(const std::string& msg);

#endif
