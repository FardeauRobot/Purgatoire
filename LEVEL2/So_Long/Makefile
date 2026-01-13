# Directories
SRCS_PATH			= srcs
UTILS_PATH			= utils
OBJS_PATH			= objs
INC_PATH			= includes
LIBFT_PATH			= $(INC_PATH)/libft
MLX_PATH			= $(INC_PATH)/minilibx-linux

# Source Files
SRCS_FILES			= main.c \
						parsing.c \
						game.c

SRCS				= $(addprefix $(SRCS_PATH)/, $(SRCS_FILES))

UTILS_FILES			= image_utils.c \
						errors.c \
						memory_utils.c 

UTILS				= $(addprefix $(UTILS_PATH)/, $(UTILS_FILES))

OBJS				= $(SRCS:.o=.c) $(UTILS:.o=.c)
# Include Files
INC_FILES			= so_long.h
INC				= $(addprefix $(INC_PATH)/, $(INC_FILES))
INC_FLAGS			= -I$(INC_PATH) -I$(LIBFT_PATH) -I$(MLX_PATH)

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

%.o:%.c
	@$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(OBJS_PATH):
	@mkdir -p $(OBJS_PATH)

$(LIBFT):
	@make -C $(LIBFT_PATH)

$(MLX):
	@make -C $(MLX_PATH) 

clean:
	$(RM) -r $(OBJS_PATH)
	make -C $(LIBFT_PATH) clean
	make -C $(MLX_PATH) clean

fclean: clean
	$(RM) $(NAME)
	make -C $(LIBFT_PATH) fclean

re: fclean all

.PHONY: all clean fclean re
