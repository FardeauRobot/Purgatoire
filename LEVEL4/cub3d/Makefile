# ============================================================
#                      PROJECT CONFIG
# ============================================================
# Change NAME to your binary name
NAME		= cub3d

# ============== COLORS ====================================
RESET		= \033[0m
RED			= \033[0;31m
GREEN		= \033[0;32m
YELLOW		= \033[0;33m
BLUE		= \033[0;34m
CYAN		= \033[0;36m
WHITE		= \033[0;37m
BOLD		= \033[1m

# ============== DIRECTORIES ===============================
SRCS_PATH	= src
INC_PATH	= includes
LIBFT_DIR	= libft
LIBFT		= $(LIBFT_DIR)/libft.a
MLX_DIR		= minilibx-linux
MLX			= $(MLX_DIR)/libmlx.a

# ============== SUB-DIRECTORIES ===========================
CORE_PATH		= $(SRCS_PATH)/core
RENDER_PATH		= $(SRCS_PATH)/render
PARSING_PATH	= $(SRCS_PATH)/parsing
UTILS_PATH		= $(SRCS_PATH)/utils

# ============== SRC-FILES =================================
# Root src files
SKELETON_FILES	= main.c

# Core module
CORE_FILES		= game.c

# Parsing module
PARSING_FILES	= parsing.c parsing_textures.c parsing_utils.c

# Render module
RENDER_FILES	= render_map.c render_utils.c imgs_init.c

# Utility module
UTILS_FILES		= error.c output.c loading_utils.c vectors.c keys.c

# Assemble all sources
SRCS =	$(addprefix $(SRCS_PATH)/, $(SKELETON_FILES)) \
		$(addprefix $(CORE_PATH)/, $(CORE_FILES)) \
		$(addprefix $(PARSING_PATH)/, $(PARSING_FILES)) \
		$(addprefix $(RENDER_PATH)/, $(RENDER_FILES)) \
		$(addprefix $(UTILS_PATH)/, $(UTILS_FILES))

OBJ_DIR	= objs
OBJS	= $(SRCS:%.c=$(OBJ_DIR)/%.o)

# ============== COMPILATION ===============================
CC		= cc
CFLAGS	= -Wall -Wextra -Werror -g3
INCS	= -I. -I$(INC_PATH) -I$(LIBFT_DIR) -I$(LIBFT_DIR)/includes -I$(MLX_DIR)
LFLAGS	= -L$(LIBFT_DIR) -lft -L$(MLX_DIR) -lm -lmlx -lXext -lX11

# ============================================================
#                        RULES
# ============================================================

all: $(NAME)

$(LIBFT):
	@make -C $(LIBFT_DIR) --no-print-directory

$(MLX):
	@make -C $(MLX_DIR) --no-print-directory

$(NAME): $(LIBFT) $(MLX) $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) $(LIBFT) $(INCS) $(LFLAGS) -o $(NAME)
	@printf "\n$(WHITE)=====================================\n$(RESET)"
	@printf "	   $(BOLD)$(GREEN)BUILD COMPLETE$(RESET)\n"
	@printf "$(WHITE)=====================================$(RESET)\n\n"

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -MMD -MP $(INCS) -c $< -o $@
	@printf "$(CYAN)  Compiling: $(RESET)$<\n"

-include $(OBJS:.o=.d)

# ============== CLEAN =====================================

clean:
	@rm -rf $(OBJ_DIR)
	@make clean -C $(LIBFT_DIR) --no-print-directory
	@make clean -C $(MLX_DIR) --no-print-directory
	@printf "\n$(WHITE)=====================================\n$(RESET)"
	@printf "	   $(BOLD)$(YELLOW)CLEAN COMPLETE$(RESET)\n"
	@printf "$(WHITE)=====================================$(RESET)\n\n"

fclean:
	@rm -rf $(OBJ_DIR)
	@rm -f $(NAME)
	@make fclean -C $(LIBFT_DIR) --no-print-directory
	@make clean -C $(MLX_DIR) --no-print-directory
	@printf "\n$(WHITE)=====================================\n$(RESET)"
	@printf "	   $(BOLD)$(YELLOW)FCLEAN COMPLETE$(RESET)\n"
	@printf "$(WHITE)=====================================$(RESET)\n\n"

re: fclean all

# ============== DEBUG & ANALYSIS ==========================

TEST= ./$(NAME) ./maps/working/basic.cub

test: $(NAME)
	$(TEST)

testformat: $(NAME)
	./$(NAME) ./maps/broken/broken_format.ber

testopen: $(NAME)
	./$(NAME) ./maps/broken/broken_access.cub

testwalls: $(NAME)
	./$(NAME) ./maps/broken/broken_walls.cub

leaks: $(NAME)
	valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes \
		--track-origins=yes $(TEST)

leaks_supp: $(NAME)
	valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes \
		--suppressions=readline.supp --track-origins=yes $(TEST)

sanitize: fclean
	$(MAKE) CFLAGS="$(CFLAGS) -fsanitize=address -fno-omit-frame-pointer" all

thread: fclean
	$(MAKE) CFLAGS="$(CFLAGS) -fsanitize=thread" all

# ============== NORMINETTE ================================

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
	@printf "	   $(BOLD)CHECK NORM CORE$(RESET)\n"
	@printf "$(CYAN)=====================================$(RESET)\n\n"
	@-norminette $(CORE_PATH)
	@printf "\n$(CYAN)=====================================\n$(RESET)"
	@printf "	   $(BOLD)CHECK NORM PARSING$(RESET)\n"
	@printf "$(CYAN)=====================================$(RESET)\n\n"
	@-norminette $(PARSING_PATH)
	@printf "\n$(CYAN)=====================================\n$(RESET)"
	@printf "	   $(BOLD)CHECK NORM UTILS$(RESET)\n"
	@printf "$(CYAN)=====================================$(RESET)\n\n"
	@-norminette $(UTILS_PATH)
	@printf "\n$(CYAN)=====================================\n$(RESET)"
	@printf "	   $(BOLD)CHECK NORM MAIN$(RESET)\n"
	@printf "$(CYAN)=====================================$(RESET)\n\n"
	@norminette $(SRCS_PATH)/main.c

# ============== UTILITIES =================================

count:
	@printf "$(CYAN)Lines of code:$(RESET) "
	@find $(SRCS_PATH) $(INC_PATH) -name '*.c' -o -name '*.h' | xargs wc -l | tail -1

todo:
	@printf "$(YELLOW)$(BOLD)  TODO / FIXME / HACK:$(RESET)\n"
	@grep -rn --color=always 'TODO\|FIXME\|HACK\|XXX' $(SRCS_PATH) $(INC_PATH) || \
		printf "$(GREEN)  None found!$(RESET)\n"

.PHONY: all clean fclean re leaks leaks_supp sanitize thread checknorm count todo
