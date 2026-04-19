NAME     = PROJECT_NAME

CXX      = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -MMD -MP
INCLUDES = -I include

# ============== DIRECTORIES ===================
SRCDIR   = src
OBJDIR   = objs

# ============== SUB-DIRECTORIES ===================
CORE_PATH  = $(SRCDIR)/core
UTILS_PATH = $(SRCDIR)/utils

# ============== SRC-FILES ===================
SKELETON_FILES = main.cpp
CORE_FILES     =
UTILS_FILES    = errors.cpp

SRCS = $(addprefix $(SRCDIR)/, $(SKELETON_FILES)) \
       $(addprefix $(CORE_PATH)/, $(CORE_FILES)) \
       $(addprefix $(UTILS_PATH)/, $(UTILS_FILES))

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

test: all
	./$(NAME)

.PHONY: all clean fclean re test
