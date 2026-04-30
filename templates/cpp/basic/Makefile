NAME     = PROJECT_NAME

CXX      = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -MMD -MP
INCLUDES = -I include

# ============== DIRECTORIES ===================
SRCDIR   = src
OBJDIR   = objs

# ============== SRC-FILES ===================
SRC_FILES = main.cpp errors.cpp utils.cpp

SRCS = $(addprefix $(SRCDIR)/, $(SRC_FILES))
OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)
DEPS = $(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

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

.PHONY: all clean fclean re
