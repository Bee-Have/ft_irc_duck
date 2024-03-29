NAME = ircserv

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98

DEBUG =
ifdef DEBUG
CXXFLAGS += -g3 -fsanitize=address
endif

OBJ_DIR = objs
SRC_DIR = $(shell find srcs -type d)

IFLAGS = $(foreach dir, $(SRC_DIR), -I$(dir))

vpath %.cpp $(foreach dir, $(SRC_DIR), $(dir):)

SERVER = Server.cpp server_loop.cpp Client.cpp

CHANNEL = Channel.cpp

MESSAGES = Message.cpp receive_messages.cpp send_messages.cpp parsing.cpp

CMD_CONNECTION = Pass.cpp Nick.cpp User.cpp
CMD_MESSAGE = Privmsg.cpp
CMD_MANAGEMENT = Oper.cpp Mode.cpp Quit.cpp
CMD_CHANNEL = Join.cpp Part.cpp Invite.cpp Topic.cpp Kick.cpp
CMD_MISC = Ping.cpp Who.cpp

SRC = main.cpp Logger.cpp $(SERVER) $(CHANNEL) $(MESSAGES) \
	$(CMD_CONNECTION) $(CMD_MESSAGE) $(CMD_MANAGEMENT) $(CMD_CHANNEL) $(CMD_MISC)

OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ_DIR):
	mkdir -p $@

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(IFLAGS) -c $< -o $@

test: $(NAME)
	./test/test.sh

clean:
	rm -rf $(OBJ_DIR)

re: clean all

fclean: clean
	rm -f $(NAME)
	rm -rf logs

.PHONY: all clean re fclean test