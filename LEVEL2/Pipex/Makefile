NAME = pipex

CC = cc -Wall -Wextra -Werror

INCS = -I. -Iincludes -I$(LIBFT_DIR) -I$(LIBFT_DIR)/includes

RESET = \033[0m
RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[0;33m
BLUE = \033[0;34m
CYAN = \033[0;36m
BOLD = \033[1m

SRCS = src/main.c \
		src/parsing.c \
		src/path.c \
		src/exec.c \
		src/utils.c \
		src/parsing_utils.c

OBJ_DIR = objs
OBJS = $(SRCS:%.c=$(OBJ_DIR)/%.o)
LIBFT_DIR = includes/libft
LIBFT = $(LIBFT_DIR)/libft.a

all: $(NAME)

$(LIBFT):
	make -C $(LIBFT_DIR)

$(NAME): $(LIBFT) $(OBJS)
	@$(CC) $(OBJS) $(LIBFT) $(INCS) -o $(NAME)
	@printf "\n$(CYAN)=====================================\n$(RESET)"
	@printf "  $(BOLD)$(GREEN)PIPEX - BUILD DONE (all targets)$(RESET)\n"
	@printf "$(CYAN)=====================================$(RESET)\n\n"

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@$(CC) $(INCS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)
	make clean -C $(LIBFT_DIR)

fclean: clean
	rm -f $(NAME)
	make fclean -C $(LIBFT_DIR)
	@printf "\n$(CYAN)=====================================\n$(RESET)"
	@printf "  $(BOLD)$(RED)PIPEX - DELETE DONE (all targets)$(RESET)\n"
	@printf "$(CYAN)=====================================$(RESET)\n\n"

re: fclean all

.PHONY: all clean fclean re
