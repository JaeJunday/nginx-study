NAME = webserv
SRCS = main.cpp \
		Configuration.cpp \
		Server.cpp \
		Operation.cpp \
		Request.cpp \
		Client/Client.cpp \
		Client/ClientGetter.cpp \
		Client/AddEvent.cpp \
		Client/DeleteEvent.cpp \
		Client/Get.cpp \
		Client/Post.cpp \
		Client/Delete.cpp \
		Client/Error.cpp\
		Util.cpp

OBJS = $(SRCS:.cpp=.o)
OBJ_DIR = build/
SRC_DIR = src/
SRC_CLIENT_DIR = src/client/
# SRCS_FILES = $(addprefix $(SRC_DIR),$(SRCS))
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
	mkdir -p $(OBJ_DIR)Client

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@ -I ./src/include

$(OBJ_DIR)%.o: $(SRC_CLIENT_DIR)%.cpp | $(OBJ_DIR)
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
