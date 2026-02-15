NAME = libft.a

CC = cc -Wall -Wextra -Werror -Iincludes -I. 

# Terminal colors for a nicer build output
RESET = \033[0m
RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[0;33m
BLUE = \033[0;34m
CYAN = \033[0;36m
BOLD = \033[1m

SRCS_CONVERSION = \
		conversion/ft_atoi.c \
		conversion/ft_atol.c \
		conversion/ft_atof.c \
		conversion/ft_itoa.c \
		conversion/ft_overint.c \
		conversion/ft_get_max.c \
		conversion/ft_min_int.c \
		conversion/ft_max_int.c \

SRCS_MEMORY = \
		memory/ft_bzero.c \
		memory/ft_calloc.c \
		memory/ft_realloc.c \
		memory/ft_memchr.c \
		memory/ft_memcmp.c \
		memory/ft_memcpy.c \
		memory/ft_memmove.c \
		memory/ft_memset.c \
		memory/ft_free_array.c \
		memory/ft_free_int_arr.c \
		memory/ft_memdel.c \
		memory/ft_free_matrix.c \
		memory/ft_arr_swap.c \
		memory/ft_memswap.c \
		memory/ft_int_swap.c \
		memory/ft_int_cmp.c \

SRCS_CHAR = \
		char/ft_isalnum.c \
		char/ft_isalpha.c \
		char/ft_isascii.c \
		char/ft_ischarset.c \
		char/ft_isdigit.c \
		char/ft_isprint.c \
		char/ft_isspace.c \
		char/ft_tolower.c \
		char/ft_toupper.c \
		char/ft_is_only.c 

SRCS_OUTPUT = \
		output/ft_putchar_fd.c \
		output/ft_putendl_fd.c \
		output/ft_putnbr_fd.c \
		output/ft_putnbr_base.c \
		output/ft_putstr_fd.c \
		output/ft_printf.c \
		output/ft_printf_nbr.c \
		output/ft_print_int_arr.c \

SRCS_STRINGS = \
		strings/ft_split_sep.c \
		strings/ft_split_charset.c \
		strings/ft_strchr.c \
		strings/ft_strcheck.c \
		strings/ft_strcmp.c \
		strings/ft_strdup.c \
		strings/ft_strndup.c \
		strings/ft_striteri.c \
		strings/ft_strjoin.c \
		strings/ft_strjoin_free.c \
		strings/ft_strlcat.c \
		strings/ft_strlcpy.c \
		strings/ft_strlen.c \
		strings/ft_strmapi.c \
		strings/ft_strncmp.c \
		strings/ft_strnstr.c \
		strings/ft_strrchr.c \
		strings/ft_strtrim.c \
		strings/ft_substr.c \
		strings/ft_matrix_len.c

SRCS_LIST = \
		list/ft_lstadd_back.c \
		list/ft_lstadd_front.c \
		list/ft_lstclear.c \
		list/ft_lstdelone.c \
		list/ft_lstiter.c \
		list/ft_lstlast.c \
		list/ft_lstmap.c \
		list/ft_lstnew.c \
		list/ft_lstsize.c \
		list/ft_lstindex.c \
		list/ft_lstat_index.c \

SRCS_GNL = \
		gnl/get_next_line.c \
		gnl/get_next_line_utils.c

SRCS_SORTING = \
		sorting/ft_bubble_sort.c \
		sorting/ft_bucket_sort.c \
		sorting/ft_counting_sort.c \
		sorting/ft_heap_sort.c \
		sorting/ft_insertion_sort.c \
		sorting/ft_merge_sort.c \
		sorting/ft_qsort.c \
		sorting/ft_radix_sort.c \
		sorting/ft_selection_sort.c \
		sorting/ft_shell_sort.c \

SRCS_GC = \
		gc/ft_gc_malloc.c \
		gc/ft_gc_add_node.c \
		gc/ft_gc_free_all.c \
		gc/ft_strdup_gc.c \
		gc/ft_substr_gc.c \
		gc/ft_strjoin_gc.c \
		gc/ft_strtrim_gc.c \
		gc/ft_itoa_gc.c \
		gc/ft_calloc_gc.c \
		gc/ft_strmapi_gc.c \
		gc/ft_strndup_gc.c \
		gc/ft_split_charset_gc.c \
		gc/ft_split_sep_gc.c \
		gc/ft_lstnew_gc.c \
		gc/ft_lstmap_gc.c \

SRCS = $(SRCS_CONVERSION) $(SRCS_MEMORY) $(SRCS_CHAR) $(SRCS_OUTPUT) $(SRCS_STRINGS) $(SRCS_LIST) $(SRCS_GNL) $(SRCS_SORTING) $(SRCS_GC)

OBJS = $(SRCS:.c=.o)


all : bannerbuild $(NAME) 

$(NAME) : $(OBJS)
	@printf "$(CYAN)$(BOLD)Archiving: $(NAME)$(RESET)\n"
	@ar rcs $(NAME) $(OBJS)
	@printf "$(GREEN)Build finished: $(NAME) created.\n$(RESET)"
	@printf "\n$(CYAN)=====================================\n$(RESET)"
	@printf "     $(BOLD)LIBFT - CMD DONE (all targets)$(RESET)\n"
	@printf "$(CYAN)=====================================$(RESET)\n\n"

%.o : %.c
#	@printf "$(YELLOW)Compiling: %s -> %s$(RESET)\n" "$<" "$@"
	@$(CC) -c $< -o $@

bannerbuild:
	@printf "\n$(CYAN)=====================================\n$(RESET)"
	@printf "     $(BOLD)LIBFT - Building (all targets)$(RESET)\n"
	@printf "$(CYAN)=====================================$(RESET)\n\n"

clean : 
	@printf "$(BLUE)Cleaning object files...$(RESET)\n"
	@rm -f $(OBJS)
	@printf "$(GREEN)Objects removed: $(words $(OBJS))$(RESET)\n"


fclean : bannerdelete clean 
	@printf "$(BLUE)Removing archive $(NAME)...$(RESET)\n"
	@rm -f $(NAME)
	@rm -f lib/$(NAME) include/libft.h || true
	@printf "$(GREEN)Archive removed: $(NAME)$(RESET)\n"
	@printf "\n$(CYAN)=====================================\n$(RESET)"
	@printf "     $(BOLD)LIBFT - CMD DONE (all targets)$(RESET)\n"
	@printf "$(CYAN)=====================================$(RESET)\n\n"

bannerdelete:
	@printf "\n$(CYAN)=====================================\n$(RESET)"
	@printf "     $(BOLD)LIBFT - DELETING (all targets)$(RESET)\n"
	@printf "$(CYAN)=====================================$(RESET)\n\n"

re : fclean 
	make all
	@printf "$(CYAN)Rebuild complete: $(NAME)$(RESET)\n"

test: $(NAME)
	@printf "\n$(CYAN)=====================================\n$(RESET)"
	@printf "     $(BOLD)LIBFT - RUNNING TESTS$(RESET)\n"
	@printf "$(CYAN)=====================================$(RESET)\n"
	@$(CC) -g3 -I. test_main.c $(NAME) -o test_runner
	@./test_runner
	@rm -f test_runner


.PHONY : clean fclean all re test
