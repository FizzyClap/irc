#include "parsing.hpp"

void	parseCommands(client client, int fd, const std::string &msg)
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

void cmdNick(client client, int fd, const std::string &msg)
{
	std::string oldNickname = client.getNickname(fd);
	std::string nickname = msg.substr(5);
	client.setNickname(fd, nickname);
	std::string nickMsg;
	std::cout << oldNickname << std::endl;
	if (oldNickname.empty())
		nickMsg = "Introducing new nick \"" + nickname + "\"\r\n";
	else
		nickMsg = ":" + oldNickname + " NICK:" + nickname + "\r\n";
	std::cout << nickMsg << std::endl;
	client.broadcast(fd, nickMsg);
}

void cmdJoin(client client, int fd, const std::string &msg)
{
	(void)client;
	(void)fd;
	std::string channel = msg.substr(5);
	std::cout << "channel = " << channel << std::endl;
}
