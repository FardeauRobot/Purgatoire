NAME     = violence

CXX      = c++
CXXFLAGS = -Wall -Wextra -Werror -Wswitch -std=c++98 -MMD -MP
LDFLAGS  =

# ============== SRC-FILES ===================
SRC_FILES = main.cpp Weapon.cpp HumanA.cpp HumanB.cpp utils.cpp

OBJDIR   = objs
OBJS = $(SRC_FILES:%.cpp=$(OBJDIR)/%.o)
DEPS = $(OBJS:.o=.d)

# ============== DEBUG / SANITIZER FLAGS =====
# -g3      : maximum debug info (var names, macros)
# -O0      : no optimisation -> stack frames stay where you'd expect them
DEBUG_FLAGS = -g3 -O0

# AddressSanitizer (ASan) catches: heap-use-after-free, double-free,
# heap-buffer-overflow, stack-use-after-return, leaks (Linux + recent macOS).
# UndefinedBehaviorSanitizer (UBSan): signed overflow, null deref, bad casts.
# -fno-omit-frame-pointer : keeps stack traces readable.
ASAN_FLAGS  = -fsanitize=address  -fno-omit-frame-pointer
UBSAN_FLAGS = -fsanitize=undefined -fno-omit-frame-pointer

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

-include $(DEPS)

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

# ============== MEMORY-CHECK TARGETS ========
# `make debug` -> rebuild with -g3 -O0 so leaks/valgrind backtraces are useful
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: re

# `make asan`  -> rebuild with AddressSanitizer + UBSan
asan: CXXFLAGS += $(DEBUG_FLAGS) $(ASAN_FLAGS) $(UBSAN_FLAGS)
asan: LDFLAGS  += $(ASAN_FLAGS) $(UBSAN_FLAGS)
asan: re
	@echo "→ run ./$(NAME) directly; ASan reports at exit"

# `make leaks` -> rebuild with -g, then auto-pick leaks (mac) or valgrind (linux)
leaks: debug
	@if [ "$$(uname)" = "Darwin" ]; then \
		echo "🔎 macOS: leaks --atExit"; \
		MallocStackLogging=1 leaks --atExit -- ./$(NAME); \
	else \
		echo "🔎 Linux: valgrind --leak-check=full"; \
		valgrind --leak-check=full --show-leak-kinds=all \
		         --track-origins=yes --error-exitcode=1 ./$(NAME); \
	fi

help:
	@echo "Targets:"
	@echo "  make            build $(NAME)"
	@echo "  make re         rebuild from scratch"
	@echo "  make clean      remove objects"
	@echo "  make fclean     remove objects + binary"
	@echo "  make debug      rebuild with -g3 -O0"
	@echo "  make asan       rebuild with AddressSanitizer + UBSan"
	@echo "  make leaks      rebuild with -g, run leaks (mac) / valgrind (linux)"

.PHONY: all clean fclean re debug asan leaks help
