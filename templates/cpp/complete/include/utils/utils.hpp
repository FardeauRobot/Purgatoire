#ifndef COLORS_HPP
# define COLORS_HPP

std::ostream& endofline(std::ostream& os);

// Regular colors
# define BLACK          "\033[30m"
# define RED            "\033[31m"
# define GREEN          "\033[32m"
# define YELLOW         "\033[33m"
# define BLUE           "\033[34m"
# define MAGENTA        "\033[35m"
# define CYAN           "\033[36m"
# define WHITE          "\033[37m"

// Bold colors
# define BOLD_BLACK     "\033[1;30m"
# define BOLD_RED       "\033[1;31m"
# define BOLD_GREEN     "\033[1;32m"
# define BOLD_YELLOW    "\033[1;33m"
# define BOLD_BLUE      "\033[1;34m"
# define BOLD_MAGENTA   "\033[1;35m"
# define BOLD_CYAN      "\033[1;36m"
# define BOLD_WHITE     "\033[1;37m"

// Underline colors
# define UL_BLACK       "\033[4;30m"
# define UL_RED         "\033[4;31m"
# define UL_GREEN       "\033[4;32m"
# define UL_YELLOW      "\033[4;33m"
# define UL_BLUE        "\033[4;34m"
# define UL_MAGENTA     "\033[4;35m"
# define UL_CYAN        "\033[4;36m"
# define UL_WHITE       "\033[4;37m"

// Background colors
# define BG_BLACK       "\033[40m"
# define BG_RED         "\033[41m"
# define BG_GREEN       "\033[42m"
# define BG_YELLOW      "\033[43m"
# define BG_BLUE        "\033[44m"
# define BG_MAGENTA     "\033[45m"
# define BG_CYAN        "\033[46m"
# define BG_WHITE       "\033[47m"

// Text styles
# define BOLD            "\033[1m"
# define DIM             "\033[2m"
# define ITALIC          "\033[3m"
# define UNDERLINE       "\033[4m"
# define BLINK           "\033[5m"
# define REVERSE         "\033[7m"
# define STRIKETHROUGH   "\033[9m"

// Reset
# define RESET           "\033[0m"

#endif
