#include "Utils.hpp"

std::string getCurrentDate()
{
	time_t now = time(NULL);
	struct tm *tm_info = localtime(&now);
	char buffer[64];
	strftime(buffer, sizeof(buffer), "%a %b %d %Y", tm_info);
	return (std::string(buffer));
}

void welcomeMessage(Server &srv, int fd)
{
	Client client = srv.getClient(fd);
	if (client.isUserSet() && client.isNickSet() && !client.isRegistered())
	{
		std::string nickname = client.getNickname();
		std::string hostname = client.getHostname();
		srv.getClient(fd).setRegistered(true);
		std::string welcomeMsg = ":ircserv 001 " + nickname + " :Welcome to the IRC Server " + nickname + "!" + client.getUsername() + "@" + hostname + "\r\n";
		std::string hostMsg = ":ircserv 002 " + nickname + " :Your host is " + hostname + ", running version 1.0\r\n";
		std::string serverMsg = ":ircserv 003 " + nickname + " :This server was created " + getCurrentDate() + "\r\n";
		std::string otherMsg = ":ircserv 004 " + nickname + " " + hostname + " 1.0 o o\r\n";
		send(fd, welcomeMsg.c_str(), welcomeMsg.length(), 0);
		send(fd, hostMsg.c_str(), hostMsg.length(), 0);
		send(fd, serverMsg.c_str(), serverMsg.length(), 0);
		send(fd, otherMsg.c_str(), otherMsg.length(), 0);
	}
	return ;
}

std::string eraseColon(std::vector<std::string> tokens, size_t size)
{
	std::string truncate = "";
	if (tokens.size() == size)
		truncate = tokens[size - 1].substr(1);
	return (truncate);
}

std::vector<std::string> split(const std::string &msg, char separator)
{
	std::vector<std::string> result;
	std::string token;
	bool foundColon = false;
	for (size_t i = 0; i < msg.size();)
	{
		if (!foundColon && msg[i] == ':' && separator != '\n')
		{
			foundColon = true;
			token = msg.substr(i);
			result.push_back(token);
			return (result) ;
		}
		if (msg[i] == separator)
		{
			if (!token.empty())
			{
				result.push_back(token);
				token.clear();
			}
			++i;
		}
		else
			token += msg[i++];
	}
	if (!token.empty())
		result.push_back(token);
	return (result);
}

int atoi(const std::string &str)
{
	std::istringstream iss(str);
	int result;
	iss >> result;
	return (result);
}

const std::string cleanMessage(std::string message)
{
	return (message.erase(message.find_last_not_of(" \r\n") + 1));
}

bool isAlpha(int c)
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		return (true);
	return (false);
}

bool isNum(int c)
{
	if (c >= '0' && c <= '9')
		return (true);
	return (false);
}

bool isSpecial(int c)
{
	if (c == '[' || c == ']' || c == '\\' || c == '`' || c == '_' || c == '-' || c == '^' || c == '{' || c == '|' || c =='}')
		return (true);
	return (false);
}

bool isValid(int c)
{
	if (!isAlpha(c) && !isNum(c) && !isSpecial(c))
		return (false);
	return (true);
}
