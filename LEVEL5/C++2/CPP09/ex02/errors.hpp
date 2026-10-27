#ifndef ERRORS_HPP
# define ERRORS_HPP

# include <string>

// --- Return codes ---
enum E_Errors
{
    SUCCESS = 0,
    FAILURE = 1
};

// --- Log an error to stderr and return FAILURE ---
int  F_ErrMsg(const std::string& msg);

// --- Log a warning to stderr (non-fatal) ---
void F_WarnMsg(const std::string& msg);

// --- Throw a std::runtime_error (unwinds stack, calls destructors) ---
void F_ErrExit(const std::string& msg);

#endif
