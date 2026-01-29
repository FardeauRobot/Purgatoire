# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tibras <tibras@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/12/16 16:02:09 by tibras            #+#    #+#              #
#    Updated: 2026/01/29 14:14:27 by tibras           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

######################### GENERAL RULES ######################### 
#################################################################
NAME		= pipex

LIBFT_PATH	= includes/libft
LIBFT		= $(LIBFT_PATH)/libft.a

CC			= cc
CFLAGS		= -fsanitize=address -Wall -Wextra -Werror -I$(LIBFT_PATH)

######################### SRCS ######################### 
########################################################
SRCS_PATH	= srcs
SRCS_FILES	= main.c \
				parsing.c \
				errors.c \
				close.c

SRCS		= $(addprefix $(SRCS_PATH)/, $(SRCS_FILES))
OBJS		= $(SRCS:.c=.o)

######################### MAKE #########################
########################################################
all: $(NAME)

test: $(NAME)
	./pipex

$(LIBFT):
	make -C $(LIBFT_PATH)

$(NAME): $(LIBFT) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)
	make -C $(LIBFT_PATH) clean

fclean: clean
	rm -f $(NAME)
	make -C $(LIBFT_PATH) fclean

re: fclean all

.PHONY: all clean fclean re
