NAME = libft.a

CC = cc -Wall -Wextra -Werror 

# Terminal colors for a nicer build output
RESET = \033[0m
RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[0;33m
BLUE = \033[0;34m
CYAN = \033[0;36m
BOLD = \033[1m

SRCS =	ft_atoi.c \
		ft_bzero.c \
		ft_calloc.c \
		ft_isalnum.c \
		ft_isalpha.c \
		ft_isascii.c \
		ft_isdigit.c \
		ft_isprint.c \
		ft_itoa.c \
		ft_memchr.c \
		ft_memcmp.c \
		ft_memcpy.c \
		ft_memmove.c \
		ft_memset.c \
		ft_putchar_fd.c \
		ft_putendl_fd.c \
		ft_putnbr_fd.c \
		ft_putstr_fd.c \
		ft_split.c \
		ft_strchr.c \
		ft_strdup.c \
		ft_striteri.c \
		ft_strjoin.c \
		ft_strlcat.c \
		ft_strlcpy.c \
		ft_strlen.c \
		ft_strmapi.c \
		ft_strncmp.c \
		ft_strnstr.c \
		ft_strrchr.c \
		ft_strtrim.c \
		ft_substr.c \
		ft_tolower.c \
		ft_toupper.c \
		ft_lstadd_back.c \
		ft_lstadd_front.c \
		ft_lstclear.c \
		ft_lstdelone.c \
		ft_lstiter.c \
		ft_lstlast.c \
		ft_lstmap.c \
		ft_lstnew.c \
		ft_lstsize.c \
		get_next_line.c \
		get_next_line_utils.c \

OBJS = $(SRCS:.c=.o)


all : banner $(NAME)

$(NAME) : $(OBJS)
	@printf "$(CYAN)$(BOLD)Archiving: $(NAME)$(RESET)\n"
	@ar rcs $(NAME) $(OBJS)
	@printf "$(GREEN)Build finished: $(NAME) created.\n$(RESET)"

%.o : %.c
	@printf "$(YELLOW)Compiling: %s -> %s$(RESET)\n" "$<" "$@"
	@$(CC) -c $< -o $@

banner:
	@printf "\n$(BLUE)=====================================\n"
	@printf "     $(BOLD)LIBFT - Building (all targets)$(RESET)\n"
	@printf "=====================================$(RESET)\n\n"

clean : 
	@printf "$(BLUE)Cleaning object files...$(RESET)\n"
	@rm -f $(OBJS)
	@printf "$(GREEN)Objects removed: $(words $(OBJS))$(RESET)\n"

fclean : clean
	@printf "$(BLUE)Removing archive $(NAME)...$(RESET)\n"
	@rm -f $(NAME)
	@rm -f lib/$(NAME) include/libft.h || true
	@printf "$(GREEN)Archive removed: $(NAME)$(RESET)\n"

re : fclean all
	@printf "$(CYAN)Rebuild complete: $(NAME)$(RESET)\n"

.PHONY : clean fclean all re

# no test/examples in this libft - keep the Makefile small and focused