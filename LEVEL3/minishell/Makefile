NAME= minishell

# ============== COLORS ===================
RESET = \033[0m
RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[0;33m
BLUE = \033[0;34m
CYAN = \033[0;36m
BOLD = \033[1m

# ============== DIRECTORIES ===================
SRCS_PATH = src
INC_PATH = includes
LIBFT_DIR = libft
LIBFT = $(LIBFT_DIR)/libft.a

# ============== SUB-DIRECTORIES ===================

PARSING_PATH = $(SRCS_PATH)/parsing
EXEC_PATH = $(SRCS_PATH)/core
BUILT_IN_PATH = $(SRCS_PATH)/built_in
UTILS_PATH = $(SRCS_PATH)/utils

# ============== SRC-FILES ===================

SKELETON_FILES = main.c display.c
PARSING_FILES = token.c token_dispatch.c cmds.c lexer.c lexer_utils.c \
	parser.c syntax_check.c path_resolve.c expander.c
EXEC_FILES = child.c child_utils.c redir.c exec.c pipe_handler.c \
	wait.c signal.c heredoc.c heredoc_utils.c
BUILT_IN_FILES = builtin.c cd.c cd_utils.c echo.c env.c exit.c \
	export.c pwd.c unset.c
UTILS_FILES = export_utils.c cmd_list.c env_setup.c env_print.c error.c \
				convert.c handlers_utils.c exec_utils.c
DEBUG_FILES = debug.c

SRCS =	$(addprefix $(SRCS_PATH)/, $(SKELETON_FILES)) \
		$(addprefix $(PARSING_PATH)/, $(PARSING_FILES)) \
		$(addprefix $(EXEC_PATH)/, $(EXEC_FILES)) \
		$(addprefix $(BUILT_IN_PATH)/, $(BUILT_IN_FILES)) \
		$(addprefix $(UTILS_PATH)/, $(UTILS_FILES)) \
		$(addprefix $(UTILS_PATH)/, $(DEBUG_FILES))

OBJ_DIR = objs
OBJS = $(SRCS:%.c=$(OBJ_DIR)/%.o)

# ============== COMPILATION ===================

CC = cc -Wall -Wextra -Werror -g3
INCS = -I. -Iincludes -I$(LIBFT_DIR) -I$(LIBFT_DIR)/includes
LFLAGS = -L$(LIBFT_DIR) -lft -lreadline

all: $(NAME)

$(LIBFT):
	make -C $(LIBFT_DIR)

$(NAME): $(LIBFT) $(OBJS)
	@$(CC) $(OBJS) $(LIBFT) $(INCS) $(LFLAGS) -o $(NAME)
	@printf "\n$(WHITE)=====================================\n$(RESET)"
	@printf "	   $(BOLD)$(GREEN)BUILD COMPLETE$(RESET)\n"
	@printf "$(WHITE)=====================================$(RESET)\n\n"

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@$(CC) -MMD -MP $(INCS) -c $< -o $@

-include $(OBJS:.o=.d)

clean:
	@rm -rf $(OBJ_DIR)
	make clean -C $(LIBFT_DIR)
	@printf "\n$(WHITE)=====================================\n$(RESET)"
	@printf "	   $(BOLD)$(YELLOW)CLEAN COMPLETE$(RESET)\n"
	@printf "$(WHITE)=====================================$(RESET)\n\n"

fclean:
	@rm -rf $(OBJ_DIR)
	@rm -f $(NAME)
	make fclean -C $(LIBFT_DIR)
	@printf "\n$(WHITE)=====================================\n$(RESET)"
	@printf "	   $(BOLD)$(YELLOW)FCLEAN COMPLETE$(RESET)\n"
	@printf "$(WHITE)=====================================$(RESET)\n\n"

re: fclean all

test: all
	cd minishell_tester && bash tester

testempty:	fclean
	cd minishell_tester && bash tester

leaks: $(NAME)
	valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes \
 		--suppressions=readline.supp --track-origins=yes ./$(NAME)

leaksnoenv: $(NAME)
	env -i valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes \
 		--suppressions=readline.supp --track-origins=yes ./$(NAME)

noenv: $(NAME)
	env -i ./$(NAME)

checknorm:
	@printf "\n$(CYAN)=====================================\n$(RESET)"
	@printf "	   $(BOLD)CHECK NORM LIBFT$(RESET)\n"
	@printf "$(CYAN)=====================================$(RESET)\n\n"
	@norminette $(LIBFT_DIR)
	@printf "\n$(CYAN)=====================================\n$(RESET)"
	@printf "	   $(BOLD)CHECK NORM INCLUDES$(RESET)\n"
	@printf "$(CYAN)=====================================$(RESET)\n\n"
	@norminette $(INC_PATH)
	@printf "\n$(CYAN)=====================================\n$(RESET)"
	@printf "	   $(BOLD)CHECK NORM SRCS$(RESET)\n"
	@printf "	   $(BOLD)CHECK BUILT_IN$(RESET)\n"
	@printf "$(CYAN)=====================================$(RESET)\n\n"
	@norminette $(BUILT_IN_PATH)
	@printf "\n$(CYAN)=====================================\n$(RESET)"
	@printf "	   $(BOLD)CHECK NORM SRCS$(RESET)\n"
	@printf "	   $(BOLD)CHECK EXEC$(RESET)\n"
	@printf "$(CYAN)=====================================$(RESET)\n\n"
	@norminette $(EXEC_PATH)
	@printf "\n$(CYAN)=====================================\n$(RESET)"
	@printf "	   $(BOLD)CHECK NORM SRCS$(RESET)\n"
	@printf "	   $(BOLD)CHECK PARSING$(RESET)\n"
	@printf "$(CYAN)=====================================$(RESET)\n\n"
	@norminette $(PARSING_PATH)
	@printf "\n$(CYAN)=====================================\n$(RESET)"
	@printf "	   $(BOLD)CHECK NORM SRCS$(RESET)\n"
	@printf "	   $(BOLD)CHECK UTILS$(RESET)\n"
	@printf "$(CYAN)=====================================$(RESET)\n\n"
	@norminette $(UTILS_PATH)
	@printf "\n$(CYAN)=====================================\n$(RESET)"
	@printf "	   $(BOLD)CHECK NORM SRCS$(RESET)\n"
	@printf "	   $(BOLD)CHECK OTHERS$(RESET)\n"
	@printf "$(CYAN)=====================================$(RESET)\n\n"
	@norminette $(SRCS_PATH)/main.c
	@norminette $(SRCS_PATH)/display.c

.PHONY: clean fclean re leaks leaksnoenv checknorm test testempty
