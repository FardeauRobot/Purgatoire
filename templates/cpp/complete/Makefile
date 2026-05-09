NAME     = EXEC_NAME

CXX      = c++
CXXFLAGS = -Wall -Wextra -Werror -Wswitch -std=c++98 -MMD -MP
LDFLAGS  =
INCLUDES = -I include

# ============== DIRECTORIES ===================
SRCDIR   = src
OBJDIR   = objs

# ============== SUB-DIRECTORIES ===================
CORE_PATH  = $(SRCDIR)/core
UTILS_PATH = $(SRCDIR)/utils

# ============== SRC-FILES ===================
SKELETON_FILES = main.cpp
CORE_FILES     = CLASS_NAME.cpp
UTILS_FILES    = errors.cpp utils.cpp

SRCS = $(addprefix $(SRCDIR)/, $(SKELETON_FILES)) \
       $(addprefix $(CORE_PATH)/, $(CORE_FILES)) \
       $(addprefix $(UTILS_PATH)/, $(UTILS_FILES))

OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)
DEPS = $(OBJS:.o=.d)

# ============== DEBUG / SANITIZER FLAGS =======
# -g3 -O0  : full debug info, no optimisation -> readable backtraces
DEBUG_FLAGS = -g3 -O0

# AddressSanitizer  : heap-use-after-free, double-free, OOB, leaks (linux+mac).
# UBSanitizer       : signed overflow, null deref, bad casts, ...
# -fno-omit-frame-pointer : keep stack traces accurate.
ASAN_FLAGS  = -fsanitize=address  -fno-omit-frame-pointer
UBSAN_FLAGS = -fsanitize=undefined -fno-omit-frame-pointer

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

-include $(DEPS)

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

test: all
	./$(NAME)

# ============== MEMORY-CHECK TARGETS ==========
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: re

asan: CXXFLAGS += $(DEBUG_FLAGS) $(ASAN_FLAGS) $(UBSAN_FLAGS)
asan: LDFLAGS  += $(ASAN_FLAGS) $(UBSAN_FLAGS)
asan: re
	@echo "→ run ./$(NAME); ASan reports at exit"

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
	@echo "  make test       build then run"
	@echo "  make debug      rebuild with -g3 -O0"
	@echo "  make asan       rebuild with AddressSanitizer + UBSan"
	@echo "  make leaks      rebuild with -g, run leaks (mac) / valgrind (linux)"

.PHONY: all clean fclean re test debug asan leaks help
