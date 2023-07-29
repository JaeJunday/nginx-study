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
SRC_DIR = src/
SRCS_FILES = $(addprefix $(SRC_DIR),$(SRCS))
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

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@ -I ./src/include

$(NAME): $(OBJS_FILES)
	$(CXX) $(CXXFLAGS) $(OBJS_FILES) -o $(NAME)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean
	make all

.PHONY: all clean fclean re

#color code 
#use with $(Reset)
# ===================
Black  	= \033[0;30m
Red    	= \033[0;31m
Green   = \033[0;32m
Yellow 	= \033[0;33m
Blue   	= \033[0;34m
Purple 	= \033[0;35m
Cyan   	= \033[0;36m
White  	= \033[0;37m
B_Black =  \033[0;90m
B_Red	= \033[0;91m
B_Green	= \033[0;92m
B_Yellow= \033[0;93m
B_Blue	= \033[0;94m
B_Purple= \033[0;95m
B_Cyan	= \033[0;96m
B_White	= \033[0;97m
Reset	= \033[0m
# ===================