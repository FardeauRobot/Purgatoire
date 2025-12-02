NAME = push_swap

CC = cc -Wall -Wextra -Werror

FILES = $(SRCS) $(UTILS) #$(INCLUDES)

SRCS = 

UTILS = 

INCLUDES = 

OBJS = $(FILES:.c=.o)

all : $(NAME)

$(NAME) : $(FILES)
	$(CC) $(FILES) $(INCLUDES) $(NAME)

%.o : %.c
	$(CC) -c $< -o $@

clean : 
	rm -rf $(OBJS)

fclean : clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re