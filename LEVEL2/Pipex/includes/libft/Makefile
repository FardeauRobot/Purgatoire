NAME = libft.a

CC = cc -Wall -Wextra -Werror -I. -Iincludes 

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
		conversion/ft_itoa.c \
		conversion/ft_overint.c \
		conversion/ft_get_max.c \
		conversion/ft_min_int.c \
		conversion/ft_atof.c \
		conversion/ft_max_int.c \

SRCS_MEMORY = \
		memory/ft_bzero.c \
		memory/ft_calloc.c \
		memory/ft_memchr.c \
		memory/ft_memcmp.c \
		memory/ft_memcpy.c \
		memory/ft_memmove.c \
		memory/ft_memset.c \
		memory/ft_free_array.c \
		memory/ft_free_int_arr.c \
		memory/ft_arr_swap.c \
		memory/ft_int_swap.c \
		memory/ft_int_cmp.c \
		memory/ft_free_matrix.c \
		memory/ft_memdel.c \
		memory/ft_memswap.c \
		memory/ft_realloc.c \

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
		output/ft_putstr_fd.c \
		output/ft_printf.c \
		output/ft_printf_nbr.c \
		output/ft_print_int_arr.c \
		output/ft_putnbr_base.c \

SRCS_STRINGS = \
		strings/ft_split_sep.c \
		strings/ft_split_charset.c \
		strings/ft_strchr.c \
		strings/ft_strcheck.c \
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
		strings/ft_matrix_len.c \
		strings/ft_strcmp.c

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

SRCS_GC = \
		gc/ft_calloc_gc.c \
		gc/ft_gc_add_node.c \
		gc/ft_gc_free_all.c \
		gc/ft_gc_malloc.c \
		gc/ft_itoa_gc.c \
		gc/ft_lstmap_gc.c \
		gc/ft_lstnew_gc.c \
		gc/ft_split_charset_gc.c \
		gc/ft_split_sep_gc.c \
		gc/ft_strdup_gc.c \
		gc/ft_strjoin_gc.c \
		gc/ft_strmapi_gc.c \
		gc/ft_strndup_gc.c \
		gc/ft_strtrim_gc.c \
		gc/ft_substr_gc.c \

SRCS_GNL = \
		gnl/get_next_line.c \
		gnl/get_next_line_utils.c

SRCS = $(SRCS_CONVERSION) $(SRCS_MEMORY) $(SRCS_CHAR) $(SRCS_OUTPUT) $(SRCS_STRINGS) $(SRCS_LIST) $(SRCS_GNL) $(SRCS_GC)

TOTAL_FILES := $(words $(SRCS))
COUNT_FILE = .make_count

OBJS = $(SRCS:.c=.o)
DEPS = $(OBJS:.o=.d)


all : bannerbuild $(NAME) 

$(NAME) : $(OBJS)
	@printf "\n$(CYAN)$(BOLD)Archiving: $(NAME)$(RESET)\n"
	@ar rcs $(NAME) $(OBJS)
	@printf "$(GREEN)Build finished: $(NAME) created.\n$(RESET)"
	@printf "\n$(CYAN)=====================================\n$(RESET)"
	@printf "     $(BOLD)LIBFT - CMD DONE (all targets)$(RESET)\n"
	@printf "$(CYAN)=====================================$(RESET)\n\n"

%.o : %.c
	@$(CC) -MMD -MP -c $< -o $@
	@curr=$$(cat $(COUNT_FILE) 2>/dev/null || echo 0); \
	curr=$$((curr + 1)); \
	echo $$curr > $(COUNT_FILE); \
	percent=$$((curr * 100 / $(TOTAL_FILES))); \
	printf "\r$(GREEN)[%-50s] %d%% %s$(RESET)\033[K" "$$(awk -v p=$$percent 'BEGIN {len=int(p*50/100); for(i=0;i<len;i++) printf "="}')" "$$percent" "$<"

bannerbuild:
	@printf "\n$(CYAN)=====================================\n$(RESET)"
	@printf "     $(BOLD)LIBFT - Building (all targets)$(RESET)\n"
	@printf "$(CYAN)=====================================$(RESET)\n\n"
	@echo 0 > $(COUNT_FILE)

clean : 
	@printf "$(BLUE)Cleaning object files...$(RESET)\n"
	@rm -f $(OBJS) $(DEPS)
	@rm -f $(COUNT_FILE)
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

.PHONY : clean fclean all re

-include $(DEPS)
