# Terminal colors for a nicer build output
RESET = \033[0m
RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[0;33m
BLUE = \033[0;34m
CYAN = \033[0;36m
BOLD = \033[1m

# Directories
SRCS_PATH			= src
INC_PATH			= includes
LIBFT_PATH			= $(INC_PATH)/libft
MLX_PATH			= $(INC_PATH)/minilibx-linux

# Subdirectories
CORE_PATH			= $(SRCS_PATH)/core
PARSING_PATH		= $(SRCS_PATH)/parsing
GRAPHICS_PATH		= $(SRCS_PATH)/graphics
UTILS_PATH			= $(SRCS_PATH)/utils

# Source Files
CORE_FILES			= main.c game.c
PARSING_FILES		= parsing.c checking.c exit.c
GRAPHICS_FILES		= display_utils.c image_utils_1.c image_utils_2.c image_utils_3.c
UTILS_FILES			= errors.c memory_utils.c

SRCS				= $(addprefix $(CORE_PATH)/, $(CORE_FILES)) \
						$(addprefix $(PARSING_PATH)/, $(PARSING_FILES)) \
						$(addprefix $(GRAPHICS_PATH)/, $(GRAPHICS_FILES)) \
						$(addprefix $(UTILS_PATH)/, $(UTILS_FILES))

OBJS				= $(SRCS:.c=.o)

# Include Files
INC_FLAGS			= -I$(INC_PATH) -I$(INC_PATH)/so_long -I$(LIBFT_PATH) -I$(MLX_PATH) -I$(LIBFT_PATH)/includes

# Libraries
LIBFT				= $(LIBFT_PATH)/libft.a
MLX				= $(MLX_PATH)/libmlx.a

# Compiler
CC				= cc
CFLAGS				= -Wall -Wextra -Werror $(INC_FLAGS)
LDFLAGS				= -L$(LIBFT_PATH) -lft -L$(MLX_PATH) -lmlx -lXext -lX11 -lm
RM				= rm -f

# Executable
NAME				= so_long

# ==================== RULES ====================

all: $(LIBFT) $(MLX) $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LDFLAGS)

%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

$(LIBFT):
	@make -C $(LIBFT_PATH)

$(MLX):
	@$(MAKE) -s --no-print-directory -C $(MLX_PATH)

clean:
	@$(RM) $(OBJS)
	@make clean -C $(LIBFT_PATH)
	@$(MAKE) -s --no-print-directory clean -C $(MLX_PATH)

fclean: clean
	@$(RM) $(NAME)
	@make fclean -C $(LIBFT_PATH)

re: fclean all

debug: CFLAGS += -fsanitize=address -g3
debug: re

checknorm:
	@printf "\n$(CYAN)=====================================\n$(RESET)"
	@printf "	   $(BOLD)CHECK NORM LIBFT$(RESET)\n"
	@printf "$(CYAN)=====================================$(RESET)\n\n"
	norminette includes/libft
	@printf "\n$(CYAN)=====================================\n$(RESET)"
	@printf "	   $(BOLD)CHECK NORM HEADERS$(RESET)\n"
	@printf "$(CYAN)=====================================$(RESET)\n\n"
	norminette includes/so_long
	@printf "\n$(CYAN)=====================================\n$(RESET)"
	@printf "	   $(BOLD)CHECK NORM SRC$(RESET)\n"
	@printf "$(CYAN)=====================================$(RESET)\n\n"
	norminette src


checkleaks: $(NAME)
	@status=0; \
	for map in maps/broken*.ber; do \
		echo "\n==> Checking $$map"; \
		valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --errors-for-leak-kinds=all --error-exitcode=200 ./$(NAME) $$map; \
		vg_status=$$?; \
		if [ $$vg_status -eq 200 ]; then \
			status=1; \
		fi; \
	done; \
	if [ $$status -ne 0 ]; then \
		echo "\nLeak check failed on at least one map."; \
		exit 1; \
	fi; \
	echo "\nAll broken maps checked with no leaks reported."

.PHONY: all clean fclean re debug checkleaks checknorm
