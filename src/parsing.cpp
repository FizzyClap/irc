#include "parsing.hpp"

void	parseCommands(server &srv, client &client, int fd, const std::string &msg)
{
	(void)client;
	std::string cmds[] = {"NICK", "JOIN", "KICK"};
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
					cmdKick(srv, msg);
					return;
			}
	}
}

void cmdNick(server &srv, int fd, const std::string &msg)
{
	std::string oldNickname = srv.getClientNickname(fd);
	std::string nickname = msg.substr(5);
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

void cmdKick(server &srv, const std::string &msg)
{
	std::string leftover = msg.substr(5);
	std::string channelName = leftover.substr(0, leftover.find_first_of(" "));
	std::string clientName = leftover.substr(leftover.find_first_of(" ") + 1);
	srv.kickClient(srv, srv.getClientFd(clientName), channelName, clientName);
}
