NAME     = diamondtrap

CXX      = c++
CXXFLAGS = -Wall -Wextra -Werror -Wswitch \
           -Wpedantic -Wshadow -Wnon-virtual-dtor -Wold-style-cast \
           -std=c++98 -MMD -MP
LDFLAGS  =

# ============== SRC-FILES ===================
SRC_FILES = main.cpp ClapTrap.cpp ScavTrap.cpp FragTrap.cpp DiamondTrap.cpp utils.cpp

OBJDIR   = objs
OBJS = $(SRC_FILES:%.cpp=$(OBJDIR)/%.o)
DEPS = $(OBJS:.o=.d)

# ============== DEBUG / SANITIZER FLAGS =====
DEBUG_FLAGS = -g3 -O0
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

debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: re

asan: CXXFLAGS += $(DEBUG_FLAGS) $(ASAN_FLAGS) $(UBSAN_FLAGS)
asan: LDFLAGS  += $(ASAN_FLAGS) $(UBSAN_FLAGS)
asan: re

.PHONY: all clean fclean re debug asan
