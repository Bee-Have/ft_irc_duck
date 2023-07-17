NAME = ircserv

CC = c++

CFLAGS = -Wall -Wextra -Werror -std=c++98

DEBUG =
ifdef DEBUG
CFLAGS += -g3 -fsanitize=address
endif

OBJ_DIR = objs
SRC_DIR = $(shell find srcs -type d)

IFLAGS = $(foreach dir, $(SRC_DIR), -I$(dir))

vpath %.cpp $(foreach dir, $(SRC_DIR), $(dir):)

SERVER = Server.cpp server_loop.cpp Client.cpp

CHANNEL = Channel.cpp

MESSAGES = Message.cpp receive_messages.cpp send_messages.cpp

CMDS = parsing.cpp

SRC = main.cpp $(SERVER) $(CHANNEL) $(CMDS) $(MESSAGES)

OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ_DIR):
	mkdir -p $@

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

re: clean all

fclean: clean
	rm -f $(NAME)

.PHONY: all clean re fclean