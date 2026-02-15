# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/12/16 16:02:09 by tibras            #+#    #+#              #
#    Updated: 2026/02/15 16:59:25 by fardeau          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

######################### GENERAL RULES ######################### 
#################################################################
NAME		= pipex

LIBFT_PATH	= libft
LIBFT		= $(LIBFT_PATH)/libft.a

CC			= cc
CFLAGS		= -fsanitize=address -Wall -Wextra -Werror -I$(LIBFT_PATH) -I$(LIBFT_PATH)/includes -Iincludes

######################### SRCS ######################### 
########################################################
SRCS_PATH	= src
SRCS_FILES	= main.c \
				parsing.c \
				init.c \
				exec.c \
				errors.c 

SRCS		= $(addprefix $(SRCS_PATH)/, $(SRCS_FILES))
OBJS		= $(SRCS:.c=.o)
DEPS		= $(OBJS:.o=.d)

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
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -f $(OBJS) $(DEPS)
	make -C $(LIBFT_PATH) clean

fclean: clean
	rm -f $(NAME)
	make -C $(LIBFT_PATH) fclean

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re
