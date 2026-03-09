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
CORE_FILES		= start.c actions.c

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
# Usage: make test ARGS="5 800 200 200"
ARGS		?= 5 800 200 200 10

test: $(NAME)
	./$(NAME) $(ARGS)

testbokou: $(NAME)
	./$(NAME) 100 1000 200 400

testopt: $(NAME)
	./$(NAME) 5 1000 200 400 10

leaks: $(NAME)
	valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes \
		--track-origins=yes ./$(NAME) $(ARGS)

leaks_supp: $(NAME)
	valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes \
		--suppressions=readline.supp --track-origins=yes ./$(NAME) $(ARGS)

# Detects data races and lock-order violations (most useful for philo)
helgrind: $(NAME)
	valgrind --tool=helgrind --history-level=approx \
		--conflict-cache-size=1000000 \
		--log-file=helgrind.log ./$(NAME) $(ARGS)
	@printf "$(CYAN)\nHelgrind report saved to: $(BOLD)helgrind.log$(RESET)\n"
	@cat helgrind.log

# Alternative thread error detector (faster than helgrind, less false positives)
drd: $(NAME)
	valgrind --tool=drd --check-stack-var=yes \
		--segment-merging=no \
		--log-file=drd.log ./$(NAME) $(ARGS)
	@printf "$(CYAN)\nDRD report saved to: $(BOLD)drd.log$(RESET)\n"
	@cat drd.log

sanitize: fclean
	$(MAKE) CFLAGS="$(CFLAGS) -fsanitize=address -fno-omit-frame-pointer" all

# Compile with thread sanitizer then run (fastest way to catch data races)
# If you get "unexpected memory mapping" error, run first:
#   sudo sysctl vm.mmap_rnd_bits=28  (kernel >= 5.18 ASLR incompatibility with TSan)
thread: fclean
	$(MAKE) CFLAGS="$(CFLAGS) -pthread" all
	env TSAN_OPTIONS="halt_on_error=1" ./$(NAME) $(ARGS)

test1: all
	@echo "\n--- Test 1: 1 Philo (Should die ~800ms) ---"
	./$(NAME) 1 800 200 200

test2: all
	@echo "\n--- Test 2: 5 Philos (Should survive, CTRL+C to stop) ---"
	$(EXEC) 5 800 200 200

test3: all
	@echo "\n--- Test 3: 4 Philos tight timing (Should survive, CTRL+C to stop) ---"
	$(EXEC) 4 410 200 200

test4: all
	@echo "\n--- Test 4: 5 Philos, 7 meals (Should exit cleanly after 7 meals) ---"
	$(EXEC) 5 800 200 200 7

test5: all
	@echo "\n--- Test 5: 4 Philos imminent death (Should die ~310ms) ---"
	./$(NAME) 4 310 200 100

test6: all
	@echo "\n--- Test 6: 200 Philos stress test (Should survive, CTRL+C to stop) ---"
	$(EXEC) 200 800 200 200

test7: all
	@echo "\n--- Test 7: 4 Philos tight timing + 5 meals (Should exit cleanly) ---"
	$(EXEC) 4 410 200 200 5

test8: all
	@echo "\n--- Test 8: 5 Philos tight timing odd (Should survive, CTRL+C to stop) ---"
	$(EXEC) 5 600 150 150

test9: all
	@echo "\n--- Test 9: 5 Philos, 1 meal fast exit (Should exit cleanly fast) ---"
	./philo 5 800 200 200 1

test10: all
	@echo "\n--- Test 10: 4 Philos instant death (Should die ~10ms) ---"
	./philo 4 10 200 200

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

# ============== UTILITIES =================================

count:
	@printf "$(CYAN)Lines of code:$(RESET) "
	@find $(SRCS_PATH) $(INC_PATH) -name '*.c' -o -name '*.h' | xargs wc -l | tail -1

todo:
	@printf "$(YELLOW)$(BOLD)  TODO / FIXME / HACK:$(RESET)\n"
	@grep -rn --color=always 'TODO\|FIXME\|HACK\|XXX' $(SRCS_PATH) $(INC_PATH) || \
		printf "$(GREEN)  None found!$(RESET)\n"

.PHONY: all clean fclean re leaks leaks_supp helgrind drd sanitize thread checknorm count todo test testbokou testopt
