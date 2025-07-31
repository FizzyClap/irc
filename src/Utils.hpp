#include "Server.hpp"

class Server;

void welcomeMessage(Server &srv, int fd);
std::string eraseColon(std::vector<std::string> tokens, size_t size);
int atoi(const std::string &str);
const std::string cleanMessage(std::string message);
std::vector<std::string> split(const std::string &msg, char separator);
