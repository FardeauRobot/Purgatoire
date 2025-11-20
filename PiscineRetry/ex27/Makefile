NAME = ft_display_file

CCFLAGS = -Wall -Wextra -Werror

CC = cc

SRCS =	main.c \
		utils.c \
		exec.c \

OBJS = $(SRCS:.c=.o)

all : $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(CCFLAGS) -o $(NAME) $(OBJS)

%.o: %.c
	$(CC) $(CCFLAGS) -c $< -o $@

clean :
	rm -f $(OBJS)

fclean : clean
	rm -f $(NAME)

re : fclean all

.PHONY: all clean fclean re