# ============================================================
#                      PROJECT CONFIG
# ============================================================
# Change NAME to your binary name
NAME		= philo

# ============== COLORS ====================================
RESET		= \033[0m
RED			= \033[0;31m
GREEN		= \033[0;32m
YELLOW		= \033[0;33m
BLUE		= \033[0;34m
CYAN		= \033[0;36m
WHITE		= \033[0;37m
BOLD		= \033[1m

# ============== DIRECTORIES ===============================
SRCS_PATH	= src
INC_PATH	= includes

# ============== SUB-DIRECTORIES ===========================
CORE_PATH		= $(SRCS_PATH)/core
PARSING_PATH	= $(SRCS_PATH)/parsing
UTILS_PATH		= $(SRCS_PATH)/utils

# ============== SRC-FILES =================================
# Root src files
SKELETON_FILES	= main.c

# Core module
CORE_FILES		= start.c actions.c actions_utils.c

# Parsing module
PARSING_FILES	= parsing.c

# Utility module
UTILS_FILES		= error.c output.c parsing_utils.c time.c reaper.c utils.c

# Assemble all sources
SRCS =	$(addprefix $(SRCS_PATH)/, $(SKELETON_FILES)) \
		$(addprefix $(CORE_PATH)/, $(CORE_FILES)) \
		$(addprefix $(PARSING_PATH)/, $(PARSING_FILES)) \
		$(addprefix $(UTILS_PATH)/, $(UTILS_FILES))

OBJ_DIR	= objs
OBJS	= $(SRCS:%.c=$(OBJ_DIR)/%.o)

# ============== COMPILATION ===============================
CC		= cc
CFLAGS	= -Wall -Wextra -Werror -g3
INCS	= -I. -I$(INC_PATH)

# ============================================================
#                        RULES
# ============================================================

all: $(NAME)


$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) $(INCS) $(LFLAGS) -o $(NAME)
	@printf "\n$(WHITE)=====================================\n$(RESET)"
	@printf "	   $(BOLD)$(GREEN)BUILD COMPLETE$(RESET)\n"
	@printf "$(WHITE)=====================================$(RESET)\n\n"

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -MMD -MP $(INCS) -c $< -o $@
	@printf "$(CYAN)  Compiling: $(RESET)$<\n"

-include $(OBJS:.o=.d)

# ============== CLEAN =====================================

clean:
	@rm -rf $(OBJ_DIR)
	@printf "\n$(WHITE)=====================================\n$(RESET)"
	@printf "	   $(BOLD)$(YELLOW)CLEAN COMPLETE$(RESET)\n"
	@printf "$(WHITE)=====================================$(RESET)\n\n"

fclean:
	@rm -rf $(OBJ_DIR)
	@rm -f $(NAME)
	@printf "\n$(WHITE)=====================================\n$(RESET)"
	@printf "	   $(BOLD)$(YELLOW)FCLEAN COMPLETE$(RESET)\n"
	@printf "$(WHITE)=====================================$(RESET)\n\n"

re: fclean all

# ============== DEBUG & ANALYSIS ==========================

ARGS		?= 100 305 150 150 100

test: $(NAME)
	./$(NAME) $(ARGS)

leaks: $(NAME)
	valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes \
		--track-origins=yes ./$(NAME) $(ARGS)

# Detects data races and lock-order violations (most useful for philo)
helgrind: $(NAME)
	valgrind --tool=helgrind --history-level=approx \
		--conflict-cache-size=1000000 \
		--log-file=helgrind.log ./$(NAME) $(ARGS)
	@printf "$(CYAN)\nHelgrind report saved to: $(BOLD)helgrind.log$(RESET)\n"
	@cat helgrind.log

# ============== NORMINETTE ================================

checknorm:
	@printf "\n$(CYAN)=====================================\n$(RESET)"
	@printf "	   $(BOLD)CHECK NORM INCLUDES$(RESET)\n"
	@printf "$(CYAN)=====================================$(RESET)\n\n"
	@norminette $(INC_PATH)
	@printf "\n$(CYAN)=====================================\n$(RESET)"
	@printf "	   $(BOLD)CHECK NORM CORE$(RESET)\n"
	@printf "$(CYAN)=====================================$(RESET)\n\n"
	@-norminette $(CORE_PATH)
	@printf "\n$(CYAN)=====================================\n$(RESET)"
	@printf "	   $(BOLD)CHECK NORM PARSING$(RESET)\n"
	@printf "$(CYAN)=====================================$(RESET)\n\n"
	@-norminette $(PARSING_PATH)
	@printf "\n$(CYAN)=====================================\n$(RESET)"
	@printf "	   $(BOLD)CHECK NORM UTILS$(RESET)\n"
	@printf "$(CYAN)=====================================$(RESET)\n\n"
	@-norminette $(UTILS_PATH)
	@printf "\n$(CYAN)=====================================\n$(RESET)"
	@printf "	   $(BOLD)CHECK NORM MAIN$(RESET)\n"
	@printf "$(CYAN)=====================================$(RESET)\n\n"
	@norminette $(SRCS_PATH)/main.c

.PHONY: all clean fclean re leaks leaks_supp helgrind drd sanitize thread checknorm count todo test testbokou testopt
