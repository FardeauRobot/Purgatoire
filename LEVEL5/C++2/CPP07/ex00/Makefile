NAME     = basics

CXX      = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS     = main.cpp errors.cpp utils.cpp whatever.cpp
OBJS     = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

own: fclean
	$(MAKE) CXXFLAGS="$(CXXFLAGS) -DSUBJECT=0"

subject: fclean
	$(MAKE) CXXFLAGS="$(CXXFLAGS) -DSUBJECT=1"

.PHONY: all clean fclean re own subject
