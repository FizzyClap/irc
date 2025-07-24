#include "parsing.hpp"

std::vector<std::string> split(const std::string &msg);

void	parseCommands(server &srv, client &client, int fd, const std::string &msg)
{
	(void)client;
	std::string cmds[] = {"NICK", "JOIN", "KICK", "PASS", "USER"};
	std::vector<std::string>tokens = split(msg);
	for (int i = 0; i < 10; i++)
	{
		if (tokens[0] == cmds[i])
			switch (i)
			{
				case 0:
					cmdNick(srv, fd, tokens);
					return;
				case 1:
					cmdJoin(srv, fd, tokens);
					return;
				case 2:
					cmdKick(srv, fd, tokens);
					return;
				case 3:
					cmdPass(srv, fd, tokens);
					return ;
				case 4:
					cmdUser(srv, fd, tokens);
					return ;

			}
	}
}

void cmdUser(server &srv, int fd, const std::vector<std::string> &tokens)
{
	if (!isAuthenticated(srv, fd, tokens[0]) || isRegistered(srv, fd, tokens[0]))
		return ;
	if (errorParams(srv, fd, tokens, 4, 5))
		return ;
	srv.setClientUsername(fd, tokens[1]);
	srv.setClientHostname(fd, tokens[2]);
	srv.setClientServername(fd, tokens[3]);
	if (tokens.size() == 5)
	{
		if (tokens[4][0] != ':')
		{
			srv.sendError(fd, "461", tokens[0], "Invalid parameter");
			return ;
		}
		std::string realname = tokens[4].substr(1);
		srv.setClientRealname(fd, realname);
	}
	srv.getClientInfos(fd).userSet = true;
	welcomeMessage(srv, fd);
}

void cmdNick(server &srv, int fd, const std::vector<std::string> &tokens)
{
	if (!isAuthenticated(srv, fd, tokens[0]))
		return ;
	if (errorParams(srv, fd, tokens, 2, 2))
		return ;
	std::string nickname = tokens[1];
	if (errorNick(srv, fd, nickname))
		return ;
	std::string oldNickname = srv.getClientNickname(fd);
	srv.setClientNickname(fd, nickname);
	srv.getClientInfos(fd).nickSet = true;
	welcomeMessage(srv, fd);
	std::string nickMsg;
	if (oldNickname.empty())
		nickMsg = "Introducing new nick \"" + nickname + "\"\r\n";
	else
		nickMsg = oldNickname + " changed is nickname to " + nickname + "\r\n";
	srv.broadcast(fd, nickMsg);
}

void cmdJoin(server &srv, int fd, const std::vector<std::string> &tokens)
{
	if (!isAuthenticated(srv, fd, tokens[0]) || !isRegistered(srv, fd, tokens[0]))
		return ;
	if (errorParams(srv, fd, tokens, 2, 2))
		return ;
	std::string	channelName = tokens[1];
	srv.joinChannel(fd, channelName);
	std::string	joinMsg = srv.getClientNickname(fd) + " has joined " + channelName + ".\n";
	srv.broadcast(fd, joinMsg);
}

void cmdKick(server &srv, int kickerFd, const std::vector<std::string> &tokens)
{
	if (!isAuthenticated(srv, kickerFd, tokens[0]) || !isRegistered(srv, kickerFd, tokens[0]))
		return ;
	if (errorParams(srv, kickerFd, tokens, 3, 4))
		return ;
	std::string channelName = tokens[1];
	std::string targetName = tokens[2];
	int targetFd = srv.getClientFd(targetName);
	if (errorKick(srv, kickerFd, targetFd, channelName, targetName))
		return ;
	srv.kickClient(targetFd, channelName, targetName);
}

void cmdPass(server &srv, int fd, const std::vector<std::string> &tokens)
{
	if (errorParams(srv, fd, tokens, 2, 2))
		return ;
	if (isAuthenticated(srv, fd, tokens[0]))
		return ;
	std::string password = tokens[1];
	if (password != srv.getPassword())
	{
		srv.sendError(fd, "464", tokens[0], "Password incorrect");
		//fermer le client
		return ;
	}
	srv.getClientInfos(fd).authenticated = true;
}

bool errorNick(server &srv, int fd, const std::string &nickname)
{
	if (nickname.length() > 9)
	{
		srv.sendError(fd, "432", nickname, "Erroneous nickname");
		return (true);
	}
	std::map<int, ClientInfos> clientsList = srv.getClientsList();
	for (std::map<int, ClientInfos>::iterator it = clientsList.begin(); it != clientsList.end(); ++it)
	{
		if (nickname == it->second.nickname)
		{
			srv.sendError(fd, "433", nickname, "Nickname is already in use");
			return (true);
		}
	}
	return (false);
}

bool errorKick(server &srv, int kickerFd, int targetFd, const std::string &channelName, const std::string &targetName)
{
	if (!srv.isMemberOfChannel(kickerFd, channelName))
	{
		srv.sendError(kickerFd, "442", channelName, "You're not on that channel");
		return (true);
	}
	else if (!srv.isMemberOfChannel(targetFd, channelName))
	{
		srv.sendError(kickerFd, "441", targetName + " " + channelName, "They aren't on that channel");
		return (true);
	}
	return (false);
}

std::vector<std::string> split(const std::string &msg)
{
	std::vector<std::string> result;
	std::istringstream iss(msg);
	std::string token;
	bool foundColon = false;
	while (iss >> token)
	{
		if (!foundColon && token[0] == ':')
		{
			foundColon = true;
			std::string leftover;
			std::getline(iss, leftover);
			if (!leftover.empty())
				token += " " + leftover;
			result.push_back(token);
			break;
		}
		result.push_back(token);
	}
	return (result);
}

bool errorParams(server &srv, int fd, std::vector<std::string> tokens, int min_params, int max_param)
{
	if (tokens.size() < (unsigned long)min_params)
	{
		srv.sendError(fd, "461", tokens[0], "Not enough parameters");
		return (true);
	}
	else if (tokens.size() > (unsigned long)max_param)
	{
		srv.sendError(fd, "461", tokens[0], "Too many parameters");
		return (true);
	}
	return (false);
}

bool isRegistered(server &srv, int fd, std::string cmd)
{
	ClientInfos infos = srv.getClientInfos(fd);
	if (infos.userSet && cmd == "USER")
	{
		srv.sendError(fd, "462", cmd, "You may not reregister");
		return (true);
	}
	else if (!infos.userSet && cmd == "USER")
		return (false);
	else if ((!infos.userSet || !infos.nickSet) && cmd != "USER" && cmd != "NICK")
	{
		srv.sendError(fd, "461", cmd, "You are not registered");
		return (false);
	}
	return (true);
}

bool isAuthenticated(server &srv, int fd, std::string cmd)
{
	bool authenticated = srv.getClientInfos(fd).authenticated;
	if (authenticated && cmd == "PASS")
	{
		srv.sendError(fd, "462", cmd, "You may not reregister");
		return (true);
	}
	else if (!authenticated && cmd != "PASS")
	{
		srv.sendError(fd, "461", cmd, "You are not registered");
		return (false);
	}
	else if (!authenticated)
		return (false);
	return (true);
}

void welcomeMessage(server &srv, int fd)
{
	ClientInfos infos = srv.getClientInfos(fd);
	if (infos.userSet && infos.nickSet && !infos.registered)
	{
		srv.getClientInfos(fd).registered = true;
		std::string message = ":ircserv 001 " + infos.nickname + " :Welcome to the IRC server " + infos.nickname + "!\r\n";
		send(fd, message.c_str(), message.length(), 0);
	}
	return ;
}
