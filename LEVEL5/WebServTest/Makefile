SERVER   = WebServ
CLIENT   = WebServ-client

CXX      = c++
CXXFLAGS = -Wall -Wextra -Werror -Wswitch \
           -Wpedantic -Wshadow -Wnon-virtual-dtor -Wold-style-cast \
           -std=c++98 -MMD -MP \
           -I server  -I client -I utils

LDFLAGS  =

# ============== SRC-FILES ===================
SERVER_SRC = \
    server/main.cpp \
    server/Server.cpp \
    server/Location.cpp \
    server/Socket.cpp \
    utils/utils.cpp

CLIENT_SRC = \
    client/main.cpp \
    client/Client.cpp \
    utils/utils.cpp

OBJDIR     = objs
SERVER_OBJ = $(patsubst %.cpp,$(OBJDIR)/%.o,$(SERVER_SRC))
CLIENT_OBJ = $(patsubst %.cpp,$(OBJDIR)/%.o,$(CLIENT_SRC))
DEPS       = $(SERVER_OBJ:.o=.d) $(CLIENT_OBJ:.o=.d)

# ============== DEBUG / SANITIZER FLAGS =====
DEBUG_FLAGS = -g3 -O0
ASAN_FLAGS  = -fsanitize=address   -fno-omit-frame-pointer
UBSAN_FLAGS = -fsanitize=undefined -fno-omit-frame-pointer

all: server client

# mkdir -p creates the per-subdir bucket on demand
$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

server: $(SERVER_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $(SERVER) $(LDFLAGS)

client: $(CLIENT_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $(CLIENT) $(LDFLAGS)

-include $(DEPS)

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(SERVER) $(CLIENT)

re: fclean all

# ============== MEMORY-CHECK TARGETS ========
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: re

asan: CXXFLAGS += $(DEBUG_FLAGS) $(ASAN_FLAGS) $(UBSAN_FLAGS)
asan: LDFLAGS  += $(ASAN_FLAGS) $(UBSAN_FLAGS)
asan: re
	@echo "→ run ./$(SERVER) or ./$(CLIENT) directly; ASan reports at exit"

leaks: debug
	@if [ "$$(uname)" = "Darwin" ]; then \
		echo "macOS: leaks --atExit — $(SERVER)"; \
		MallocStackLogging=1 leaks --atExit -- ./$(SERVER); \
	else \
		echo "Linux: valgrind --leak-check=full — $(SERVER)"; \
		valgrind --leak-check=full --show-leak-kinds=all \
		         --track-origins=yes --error-exitcode=1 ./$(SERVER); \
	fi

# ============== WATCH MODE ==================
watch-server:
	@command -v fswatch >/dev/null 2>&1 || { \
		echo "fswatch not found — install with: brew install fswatch"; \
		exit 1; \
	}
	@$(MAKE) --no-print-directory server && echo "── run ──" && ./$(SERVER); true
	@echo "watching server files — Ctrl-C to stop"
	@fswatch -o $(SERVER_SRC) $(wildcard server/*.hpp) $(wildcard utils/*.hpp) | while read -r _; do \
		clear; \
		printf '↻ %s — rebuilding\n' "$$(date +%H:%M:%S)"; \
		$(MAKE) --no-print-directory server && echo "── run ──" && ./$(SERVER); true; \
	done

watch-client:
	@command -v fswatch >/dev/null 2>&1 || { \
		echo "fswatch not found — install with: brew install fswatch"; \
		exit 1; \
	}
	@$(MAKE) --no-print-directory client && echo "── run ──" && ./$(CLIENT); true
	@echo "watching client files — Ctrl-C to stop"
	@fswatch -o $(CLIENT_SRC) $(wildcard client/*.hpp) $(wildcard utils/*.hpp) | while read -r _; do \
		clear; \
		printf '↻ %s — rebuilding\n' "$$(date +%H:%M:%S)"; \
		$(MAKE) --no-print-directory client && echo "── run ──" && ./$(CLIENT); true; \
	done

help:
	@echo "Targets:"
	@echo "  make               build both $(SERVER) and $(CLIENT)"
	@echo "  make server        build $(SERVER) only"
	@echo "  make client        build $(CLIENT) only"
	@echo "  make re            rebuild both from scratch"
	@echo "  make clean         remove objects"
	@echo "  make fclean        remove objects + binaries"
	@echo "  make debug         rebuild with -g3 -O0"
	@echo "  make asan          rebuild with AddressSanitizer + UBSan"
	@echo "  make leaks         rebuild debug, then run leaks (mac) / valgrind (linux)"
	@echo "  make watch-server  auto-rebuild server on change (needs fswatch)"
	@echo "  make watch-client  auto-rebuild client on change (needs fswatch)"

.PHONY: all server client clean fclean re debug asan leaks watch-server watch-client help
