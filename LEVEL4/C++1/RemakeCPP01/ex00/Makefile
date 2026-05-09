NAME     = braiiiiiinnnnnzzzzzz

CXX      = c++
CXXFLAGS = -Wall -Wextra -Werror -Wswitch -std=c++98 -MMD -MP

# ============== SRC-FILES ===================
SRC_FILES = main.cpp Zombie.cpp newZombie.cpp randomChump.cpp utils.cpp

OBJDIR   = objs
OBJS = $(SRC_FILES:%.cpp=$(OBJDIR)/%.o)
DEPS = $(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

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

.PHONY: all clean fclean re
