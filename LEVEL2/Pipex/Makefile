# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tibras <tibras@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/12/16 16:02:09 by tibras            #+#    #+#              #
#    Updated: 2025/12/16 16:42:38 by tibras           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

######################### GENERAL RULES ######################### 
#################################################################
NAME		= pipex
CC			= cc
CFLAGS		= -Wall -Wextra -Werror
INCLUDES	= -I./includes -I./includes/libft -I.

LIBFT_PATH	= includes/libft
LIBFT		= $(LIBFT_PATH)/libft.a

######################### SRCS ######################### 
########################################################
SRCS_PATH	= srcs
SRCS_FILES	= main.c 

SRCS		= $(addprefix $(SRCS_PATH)/, $(SRCS_FILES))
OBJS		= $(SRCS:.c=.o)

######################### MAKE #########################
########################################################

$(LIBFT):
	make -C $(LIBFT_PATH)

all: $(LIBFT) $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS)
	make -C $(LIBFT_PATH) clean

fclean: clean
	rm -f $(NAME)
	make -C $(LIBFT_PATH) fclean

re: fclean all

.PHONY: all clean fclean re
