NAME = push_swap

CC = cc -Wall -Wextra -Werror -g3 #g3 A DELETE


SRCS_DIR = srcs
UTILS_DIR = utils
INCLUDES_DIR = includes
LIBFT_DIR = $(INCLUDES_DIR)/libft

LIBFT = $(LIBFT_DIR)/libft.a

SRCS_FILES = main.c \

UTILS_FILES = 	parsing.c \
				parsing_utils.c \
				free_utils.c \
				node_utils.c 

UTILS = $(addprefix $(UTILS_DIR)/, $(UTILS_FILES))
SRCS = $(addprefix $(SRCS_DIR)/, $(SRCS_FILES))

FILES = $(SRCS) $(UTILS) #$(INCLUDES)
OBJS = $(FILES:.c=.o)

INCLUDES    += -I. -I$(INCLUDES_DIR) -I$(LIBFT_DIR)

all : $(LIBFT) $(NAME)

$(NAME) : $(LIBFT) $(OBJS)
	$(CC) $(OBJS) $(LIBFT) -o $(NAME)

%.o : %.c
	$(CC) $(INCLUDES) -c $< -o $@

$(LIBFT):
	make -C $(LIBFT_DIR)

clean : 
	rm -rf $(OBJS)

fclean : clean
	rm -rf $(NAME)
	make -C $(LIBFT_DIR) fclean

re: fclean all

.PHONY: all clean fclean re