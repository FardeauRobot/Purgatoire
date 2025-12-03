NAME = push_swap

CC = cc -Wall -Wextra -Werror

FILES = $(SRCS) $(UTILS) #$(INCLUDES)

SRCS_DIR = srcs

SRCS_FILES = main.c \

SRCS = $(addprefix $(SRCS_DIR)/, $(SRCS_FILES))

UTILS_DIR = utils

UTILS_FILES = 	parsing.c \
				parsing_utils.c \

UTILS = $(addprefix $(UTILS_DIR)/, $(UTILS_FILES))

INCLUDES =  -I. -Iincludes

OBJS = $(FILES:.c=.o)

all : $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(OBJS) -o $(NAME)

%.o : %.c
	$(CC) $(INCLUDES) -c $< -o $@

clean : 
	rm -rf $(OBJS)

fclean : clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re