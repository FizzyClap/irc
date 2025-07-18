#include "parsing.hpp"

void	parseCommands(client &client, int fd, const std::string &msg)
{
	std::string cmds[] = {"NICK", "JOIN"};
	std::string cmd = msg.substr(0, msg.find_first_of(" "));
	for (int i = 0; i < 2; i++)
	{
		if (cmd == cmds[i])
			switch (i)
			{
				case 0:
					cmdNick(client, fd, msg);
					return;
				case 1:
					cmdJoin(client, fd, msg);
					return;
			}
	}
}

void cmdNick(client &client, int fd, const std::string &msg)
{
	std::string oldNickname = client.getNickname(fd);
	std::string nickname = msg.substr(5);
	client.setNickname(fd, nickname);
	std::string nickMsg;
	if (oldNickname.empty())
		nickMsg = "Introducing new nick \"" + nickname + "\"\r\n";
	else
		nickMsg = oldNickname + " changed is nickname to " + nickname + "\r\n";
	std::cout << nickMsg << std::endl;
	client.broadcast(fd, nickMsg);
}

void cmdJoin(client &client, int fd, const std::string &msg)
{
	std::string	channelName = msg.substr(5);
	client.joinChannel(fd, channelName);
	std::string	joinMsg = client.getNickname(fd) + " has joined " + channelName + ".\n";
	client.broadcast(fd, joinMsg);
}
