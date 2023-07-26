NAME = ./webserv
SRCS = main.cpp \
		Configuration.cpp \
		Server.cpp \
		Operation.cpp \
		Request.cpp \
		AResponse.cpp \
		Get.cpp \
		Util.cpp

OBJS = $(SRCS:.cpp=.o)
OBJ_DIR = obj/
OBJS_FILES = $(addprefix $(OBJ_DIR),$(OBJS))

CXX = c++

# CXXFLAGS += -std=c++98 
CXXFLAGS += -Wall -Wextra #-Werror 

CXXFLAGS += -fsanitize=address -g3 

CXXFLAGS += -O3 # -O2 -O1 -O -Os
# CXXFLAGS += -g

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