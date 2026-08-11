NAME     = 28b28c

CXX      = c++
CXXFLAGS = -Wall -Wextra -Werror -Wswitch \
           -Wpedantic -Wshadow -Wnon-virtual-dtor -Wold-style-cast \
           -std=c++98 -MMD -MP
LDFLAGS  =

# ============== SRC-FILES ===================
SRC_FILES = main.cpp Bureaucrat.cpp AForm.cpp \
            ShrubberyCreationForm.cpp RobotomyRequestForm.cpp PresidentialPardonForm.cpp \
            utils.cpp

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
	rm -f *_shrubbery
	rm -f ../tests/*_shruberry

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

# ============== TEST-CASE TARGETS ===========
# main.cpp splits its demo into six blocks, each behind `#if <MACRO> == 1`, and
# each macro defaults to 1 — so a plain `make` already builds everything.
# A single-part target therefore has to switch the other five OFF, and every
# macro must be named exactly once: clang rejects a repeated -D on the command
# line (-Wmacro-redefined), which -Werror turns into a build failure.
# `re` is mandatory: make only compares timestamps, it has no idea CXXFLAGS
# changed, so without a forced rebuild you'd re-run the previous binary.
full: CXXFLAGS += -DTEST=1 -DSHRUBBERY=1 -DROBOTOMY=1 -DPARDON=1 -DTOO_LOW=1 -DTOO_HIGH=1
full: re
	@echo "── run (FULL) ──"; ./$(NAME)

base: CXXFLAGS += -DTEST=1 -DSHRUBBERY=0 -DROBOTOMY=0 -DPARDON=0 -DTOO_LOW=0 -DTOO_HIGH=0
base: re
	@echo "── run (OCF / BUREAUCRAT / SIGN) ──"; ./$(NAME)

shrubbery: CXXFLAGS += -DTEST=0 -DSHRUBBERY=1 -DROBOTOMY=0 -DPARDON=0 -DTOO_LOW=0 -DTOO_HIGH=0
shrubbery: re
	@echo "── run (SHRUBBERY) ──"; ./$(NAME)

robotomy: CXXFLAGS += -DTEST=0 -DSHRUBBERY=0 -DROBOTOMY=1 -DPARDON=0 -DTOO_LOW=0 -DTOO_HIGH=0
robotomy: re
	@echo "── run (ROBOTOMY) ──"; ./$(NAME)

pardon: CXXFLAGS += -DTEST=0 -DSHRUBBERY=0 -DROBOTOMY=0 -DPARDON=1 -DTOO_LOW=0 -DTOO_HIGH=0
pardon: re
	@echo "── run (PARDON) ──"; ./$(NAME)

toolow: CXXFLAGS += -DTEST=0 -DSHRUBBERY=0 -DROBOTOMY=0 -DPARDON=0 -DTOO_LOW=1 -DTOO_HIGH=0
toolow: re
	@echo "── run (TOO_LOW) ──"; ./$(NAME)

toohigh: CXXFLAGS += -DTEST=0 -DSHRUBBERY=0 -DROBOTOMY=0 -DPARDON=0 -DTOO_LOW=0 -DTOO_HIGH=1
toohigh: re
	@echo "── run (TOO_HIGH) ──"; ./$(NAME)

# kept as aliases: both used to mean "everything", `full` is now the real target
tests: full
test: full

# ============== WATCH MODE ==================
# `make watch` -> rebuild automatically on every source change.
# Requires fswatch:   macOS: brew install fswatch   Linux: apt-get install fswatch
WATCH_PATHS = $(SRC_FILES) $(wildcard *.hpp)

watch:
	@command -v fswatch >/dev/null 2>&1 || { \
		echo "fswatch not found — install with: brew install fswatch"; \
		exit 1; \
	}
	@if $(MAKE) --no-print-directory all; then \
		echo "── run ──"; ./$(NAME); \
	fi
	@echo "👀 watching $(words $(WATCH_PATHS)) files — Ctrl-C to stop"
	@fswatch -o $(WATCH_PATHS) | while read -r _; do \
		clear; \
		printf '↻ %s — rebuilding\n' "$$(date +%H:%M:%S)"; \
		if $(MAKE) --no-print-directory all; then \
			echo "── run ──"; ./$(NAME); \
		fi; \
	done

help:
	@echo "Targets:"
	@echo "  make            build $(NAME)"
	@echo "  make re         rebuild from scratch"
	@echo "  make clean      remove objects"
	@echo "  make fclean     remove objects + binary"
	@echo "  make debug      rebuild with -g3 -O0"
	@echo "  make asan       rebuild with AddressSanitizer + UBSan"
	@echo "  make leaks      rebuild with -g, run leaks (mac) / valgrind (linux)"
	@echo "  make watch      auto-rebuild on source change (needs fswatch)"
	@echo "  make full       build+run every block (alias: tests, test)"
	@echo "  make base       build+run OCF / Bureaucrat / sign only"
	@echo "  make shrubbery  build+run the ShrubberyCreationForm block only"
	@echo "  make robotomy   build+run the RobotomyRequestForm block only"
	@echo "  make pardon     build+run the PresidentialPardonForm block only"
	@echo "  make toolow     build+run the GradeTooLow cases only"
	@echo "  make toohigh    build+run the GradeTooHigh cases only"

.PHONY: all clean fclean re debug asan leaks watch help \
        full base shrubbery robotomy pardon toolow toohigh tests test
