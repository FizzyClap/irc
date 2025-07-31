TARGET = ./ircserv
BOT_TARGET = ./bot

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g -fsanitize=address

SRC_MAIN = ./src/main.cpp
SRC_SRV = ./src/Server.cpp \
	./src/CommandHandler.cpp \
	./src/PollManager.cpp\
	./src/Channel.cpp \
	./src/Client.cpp \
	./src/Utils.cpp
SRC_BOT = ./src/Bot.cpp

OBJ_MAIN = $(SRC_MAIN:.cpp=.o)
OBJ_SRV = $(SRC_SRV:.cpp=.o)
OBJ_BOT = $(SRC_BOT:.cpp=.o)

all: $(TARGET) $(BOT_TARGET)

re: clean all

$(TARGET) : $(OBJ_MAIN) $(OBJ_SRV)
	$(CXX) $(CXXFLAGS) -o$(TARGET) $(OBJ_MAIN) $(OBJ_SRV)

$(BOT_TARGET): $(OBJ_SRV) $(OBJ_BOT)
	$(CXX) $(CXXFLAGS) -o$(BOT_TARGET) $(OBJ_SRV) $(OBJ_BOT)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_MAIN) $(OBJ_SRV) $(OBJ_BOT)

fclean: clean
	rm -rf $(TARGET) $(BOT_TARGET)
