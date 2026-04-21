NAME     = ex04

CXX      = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -MMD -MP
INCLUDES = -I include

# ============== DIRECTORIES ===================
SRCDIR   = src
OBJDIR   = objs

# ============== SRC-FILES ===================
SRC_FILES = main.cpp errors.cpp Sed.cpp

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

# ============== TESTS ===================
# test1: basic single-word replacement
test1: all
	@echo "\n--- TEST 1: basic replacement (Hello -> Goodbye) ---"
	./$(NAME) tests/test1.txt Hello Goodbye
	@echo "--- output file ---"
	@cat tests/test1.txt.replace

# test2: multiple occurrences of same word on each line
test2: all
	@echo "\n--- TEST 2: multiple occurrences (cat -> dog) ---"
	./$(NAME) tests/test2.txt cat dog
	@echo "--- output file ---"
	@cat tests/test2.txt.replace

# test3: no match found (pattern not in file)
test3: all
	@echo "\n--- TEST 3: no match (pattern 'xyz' not in file) ---"
	./$(NAME) tests/test3.txt xyz abc
	@echo "--- output file ---"
	@cat tests/test3.txt.replace

# test4: empty replacement string (deletion)
test4: all
	@echo "\n--- TEST 4: delete pattern (replace 'word' with empty string) ---"
	./$(NAME) tests/test4.txt word ""
	@echo "--- output file ---"
	@cat tests/test4.txt.replace

# test5: pattern at edges + empty lines
test5: all
	@echo "\n--- TEST 5: edge cases (replace 'pattern' with 'FOUND') ---"
	./$(NAME) tests/test5.txt pattern FOUND
	@echo "--- output file ---"
	@cat tests/test5.txt.replace

# run all tests sequentially
tests: test1 test2 test3 test4 test5

# clean generated .replace files
tclean:
	@rm -f tests/*.replace

.PHONY: all clean fclean re test1 test2 test3 test4 test5 tests tclean
