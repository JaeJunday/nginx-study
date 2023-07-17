NAME = ./webserv
SRCS = main.cpp

OBJS = $(SRCS:.cpp=.o)
OBJ_DIR = obj/
OBJS_FILES = $(addprefix $(OBJ_DIR),$(OBJS))

CXX = c++
CXXFLAGS = -std=c++98 -Wall -Wextra -Werror 

all: $(NAME)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)%.o: %.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(NAME): $(OBJS_FILES)
	$(CXX) $(CXXFLAGS) $(OBJS_FILES) -o $(NAME)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean
	make all

.PHONY: all clean fclean re