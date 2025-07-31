#include "Utils.hpp"

void welcomeMessage(Server &srv, int fd)
{
	Client client = srv.getClient(fd);
	if (client.isUserSet() && client.isNickSet() && !client.isRegistered())
	{
		srv.getClient(fd).setRegistered(true);
		std::string message = ":ircserv 001 " + client.getNickname() + " :Welcome to the IRC Server " + client.getNickname() + "!\r\n";
		send(fd, message.c_str(), message.length(), 0);
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
