CXX      = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -MMD -MP

NAME     = phonebook
OBJDIR   = objs

SRCS     = phonebook.cpp Contact.cpp PhoneBook.cpp
OBJS     = $(addprefix $(OBJDIR)/, $(SRCS:.cpp=.o))
DEPS     = $(OBJS:.o=.d)

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

test: all
	./phonebook

.PHONY: all clean fclean re
