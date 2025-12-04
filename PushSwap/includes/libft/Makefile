NAME = libft.a

CC = cc -Wall -Wextra -Werror -I. 

# Terminal colors for a nicer build output
RESET = \033[0m
RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[0;33m
BLUE = \033[0;34m
CYAN = \033[0;36m
BOLD = \033[1m

SRCS_CONVERSION = \
		Conversion/ft_atoi.c \
		Conversion/ft_atol.c \
		Conversion/ft_itoa.c \
		Conversion/ft_overint.c

SRCS_MEMORY = \
		Memory/ft_bzero.c \
		Memory/ft_calloc.c \
		Memory/ft_memchr.c \
		Memory/ft_memcmp.c \
		Memory/ft_memcpy.c \
		Memory/ft_memmove.c \
		Memory/ft_memset.c \
		Memory/ft_free_array.c

SRCS_CHAR = \
		Char/ft_isalnum.c \
		Char/ft_isalpha.c \
		Char/ft_isascii.c \
		Char/ft_ischarset.c \
		Char/ft_isdigit.c \
		Char/ft_isprint.c \
		Char/ft_isspace.c \
		Char/ft_tolower.c \
		Char/ft_toupper.c \
		Char/ft_is_only.c 

SRCS_OUTPUT = \
		Output/ft_putchar_fd.c \
		Output/ft_putendl_fd.c \
		Output/ft_putnbr_fd.c \
		Output/ft_putstr_fd.c \
		Output/ft_printf.c \
		Output/ft_printf_nbr.c

SRCS_STRINGS = \
		Strings/ft_split_sep.c \
		Strings/ft_split_charset.c \
		Strings/ft_strchr.c \
		Strings/ft_strcheck.c \
		Strings/ft_strdup.c \
		Strings/ft_striteri.c \
		Strings/ft_strjoin.c \
		Strings/ft_strlcat.c \
		Strings/ft_strlcpy.c \
		Strings/ft_strlen.c \
		Strings/ft_strmapi.c \
		Strings/ft_strncmp.c \
		Strings/ft_strnstr.c \
		Strings/ft_strrchr.c \
		Strings/ft_strtrim.c \
		Strings/ft_substr.c

SRCS_LIST = \
		List/ft_lstadd_back.c \
		List/ft_lstadd_front.c \
		List/ft_lstclear.c \
		List/ft_lstdelone.c \
		List/ft_lstiter.c \
		List/ft_lstlast.c \
		List/ft_lstmap.c \
		List/ft_lstnew.c \
		List/ft_lstsize.c

SRCS_GNL = \
		GNL/get_next_line.c \
		GNL/get_next_line_utils.c

SRCS = $(SRCS_CONVERSION) $(SRCS_MEMORY) $(SRCS_CHAR) $(SRCS_OUTPUT) $(SRCS_STRINGS) $(SRCS_LIST) $(SRCS_GNL)

OBJS = $(SRCS:.c=.o)


all : bannerbuild $(NAME) 

$(NAME) : $(OBJS)
	@printf "$(CYAN)$(BOLD)Archiving: $(NAME)$(RESET)\n"
	@ar rcs $(NAME) $(OBJS)
	@printf "$(GREEN)Build finished: $(NAME) created.\n$(RESET)"
	@printf "\n$(CYAN)=====================================\n$(RESET)"
	@printf "     $(BOLD)LIBFT - CMD DONE (all targets)$(RESET)\n"
	@printf "$(CYAN)=====================================$(RESET)\n\n"

%.o : %.c
#	@printf "$(YELLOW)Compiling: %s -> %s$(RESET)\n" "$<" "$@"
	@$(CC) -c $< -o $@

bannerbuild:
	@printf "\n$(CYAN)=====================================\n$(RESET)"
	@printf "     $(BOLD)LIBFT - Building (all targets)$(RESET)\n"
	@printf "$(CYAN)=====================================$(RESET)\n\n"

clean : 
	@printf "$(BLUE)Cleaning object files...$(RESET)\n"
	@rm -f $(OBJS)
	@printf "$(GREEN)Objects removed: $(words $(OBJS))$(RESET)\n"


fclean : bannerdelete clean 
	@printf "$(BLUE)Removing archive $(NAME)...$(RESET)\n"
	@rm -f $(NAME)
	@rm -f lib/$(NAME) include/libft.h || true
	@printf "$(GREEN)Archive removed: $(NAME)$(RESET)\n"
	@printf "\n$(CYAN)=====================================\n$(RESET)"
	@printf "     $(BOLD)LIBFT - CMD DONE (all targets)$(RESET)\n"
	@printf "$(CYAN)=====================================$(RESET)\n\n"

bannerdelete:
	@printf "\n$(CYAN)=====================================\n$(RESET)"
	@printf "     $(BOLD)LIBFT - DELETING (all targets)$(RESET)\n"
	@printf "$(CYAN)=====================================$(RESET)\n\n"

re : fclean all
	@printf "$(CYAN)Rebuild complete: $(NAME)$(RESET)\n"

.PHONY : clean fclean all re

# no test/examples in this libft - keep the Makefile small and focused