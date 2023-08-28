NAME = webserv
SRCS = main.cpp \
		Configuration.cpp \
		Server.cpp \
		Operation.cpp \
		Client/Client.cpp \
		Client/ClientGetter.cpp \
		Client/AddEvent.cpp \
		Client/DeleteEvent.cpp \
		Client/Get.cpp \
		Client/Post.cpp \
		Client/Delete.cpp \
		Client/Error.cpp\
		Request/Request.cpp \
		Request/RequestChunked.cpp \
		Request/RequestGetter.cpp \
		Request/RequestSetter.cpp \
		Util.cpp

OBJS = $(SRCS:.cpp=.o)
OBJ_DIR = build/
SRC_DIR = src/
SRC_CLIENT_DIR = src/client/
SRC_REQUEST_DIR = src/Request/
OBJS_FILES = $(addprefix $(OBJ_DIR),$(OBJS))

CXX = c++

CXXFLAGS += -Wall -Wextra -Werror 
CXXFLAGS += -std=c++98
CXXFLAGS += -O3 # -O2 -O1 -O -Os
#CXXFLAGS += -fsanitize=address -g3 

all: $(NAME)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_DIR)Client
	mkdir -p $(OBJ_DIR)Request

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@ -I ./include

$(OBJ_DIR)%.o: $(SRC_CLIENT_DIR)%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@ -I ./include

$(OBJ_DIR)%.o: $(SRC_REQUEST_DIR)%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@ -I ./include	

$(NAME): $(OBJS_FILES)
	$(CXX) $(CXXFLAGS) $(OBJS_FILES) -o $(NAME)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean
	make all

.PHONY: all clean fclean re
