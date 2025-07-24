#include "parsing.hpp"

void	parseCommands(server &srv, client &client, int fd, const std::string &msg)
{
	(void)client;
	std::string cmds[] = {"NICK", "JOIN", "KICK", "PASS", "USER"};
	std::string cmd = msg.substr(0, msg.find_first_of(" "));
	for (int i = 0; i < 10; i++)
	{
		if (cmd == cmds[i])
			switch (i)
			{
				case 0:
					cmdNick(srv, fd, msg);
					return;
				case 1:
					cmdJoin(srv, fd, msg);
					return;
				case 2:
					cmdKick(srv, fd, msg);
					return;
				case 3:
					cmdPass(srv, fd, msg);
					return ;

			}
	}
}

void cmdNick(server &srv, int fd, const std::string &msg)
{
	std::string nickname = msg.substr(5);
	if (errorNick(srv, fd, nickname))
		return ;
	std::string oldNickname = srv.getClientNickname(fd);
	srv.setClientNickname(fd, nickname);
	std::string nickMsg;
	if (oldNickname.empty())
		nickMsg = "Introducing new nick \"" + nickname + "\"\r\n";
	else
		nickMsg = oldNickname + " changed is nickname to " + nickname + "\r\n";
	srv.broadcast(fd, nickMsg);
}

void cmdJoin(server &srv, int fd, const std::string &msg)
{
	std::string	channelName = msg.substr(5);
	srv.joinChannel(fd, channelName);
	std::string	joinMsg = srv.getClientNickname(fd) + " has joined " + channelName + ".\n";
	srv.broadcast(fd, joinMsg);
}

void cmdKick(server &srv, int kickerFd, const std::string &msg)
{
	std::string leftover = msg.substr(5);
	std::string channelName = leftover.substr(0, leftover.find_first_of(" "));
	std::string clientName = leftover.substr(leftover.find_first_of(" ") + 1);
	int targetFd = srv.getClientFd(clientName);
	if (errorKick(srv, kickerFd, targetFd, channelName, clientName))
		return ;
	srv.kickClient(srv, targetFd, channelName, clientName);
}

void cmdPass(server &srv, int fd, const std::string &msg)
{
	(void)fd;
	std::string password = msg.substr(5);
	std::map<int, ClientInfos> clientsList = srv.getClientsList();
	if (password == srv.getPassword())
	{

	}
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
