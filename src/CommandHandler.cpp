#include "CommandHandler.hpp"

void parseCommands(Server &srv, int fd, const std::string &msg)
{
	const std::string newMessage = cleanMessage(msg);
	std::string cmds[] = {"PASS", "NICK", "USER", "JOIN", "KICK", "PRIVMSG", "INVITE", "TOPIC", "MODE", "LIST"};
	CommandHandler handlers[] = {
		cmdPass, cmdNick, cmdUser, cmdJoin, cmdKick, cmdPrivMsg, cmdInvite, cmdTopic, cmdMode, cmdList
	};
	std::vector<std::string> lines = split(newMessage, '\n');
	for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it)
	{
		std::string line = cleanMessage(*it);
		std::vector<std::string> tokens = split(line, ' ');
		for (int i = 0; i < 10; i++)
		{
			if (tokens[0] == cmds[i])
			{
				handlers[i](srv, fd, tokens);
				break ;
			}
		}
	}
}

void cmdPass(Server &srv, int fd, const std::vector<std::string> &tokens)
{
	if (errorPass(srv, fd, tokens))
		return ;
	srv.getClient(fd).setAuthenticated(true);
}

void cmdUser(Server &srv, int fd, const std::vector<std::string> &tokens)
{
	if (!isAuthenticated(srv, fd, tokens[0]) || isRegistered(srv, fd, tokens[0]) || errorParams(srv, fd, tokens, 4, 5) || errorUser(srv, fd, tokens))
		return ;
	Client &client = srv.getClient(fd);
	client.setUsername(tokens[1]);
	client.setHostname(tokens[2]);
	client.setServername(tokens[3]);
	std::string realname = eraseColon(tokens, 5);
	if (!realname.empty())
		client.setRealname(tokens[4]);
	welcomeMessage(srv, fd);
}

void cmdNick(Server &srv, int fd, const std::vector<std::string> &tokens)
{
	if (errorNick(srv, fd, tokens))
		return ;
	Client &client = srv.getClient(fd);
	std::string nickname = tokens[1];
	std::string oldNickname = client.getNickname();
	client.setNickname(nickname);
	welcomeMessage(srv, fd);
	std::string nickMsg;
	nickMsg = oldNickname + " NICK :" + nickname + "\r\n";
	srv.broadcast(fd, nickMsg, true);
}

void cmdJoin(Server &srv, int fd, const std::vector<std::string> &tokens)
{
	if (errorJoin(srv, fd, tokens))
		return ;
	std::vector<std::string> channelsName = split(tokens[1], ',');
	std::vector<std::string> keysPass;
	if (tokens.size() == 3)
		keysPass = split(tokens[2], ',');
	for (size_t i = 0; i < channelsName.size(); ++i)
	{
		const std::string &channel = channelsName[i];
		std::string key = "";
		if (i < keysPass.size())
			key = keysPass[i];
		if (!srv.joinChannel(fd, channel, key))
			return ;
		srv.broadcastForJoin(fd, channel, key);
	}
}

void cmdKick(Server &srv, int kickerFd, const std::vector<std::string> &tokens)
{
	if (errorKick(srv, kickerFd, tokens))
		return ;
	std::string channelName = tokens[1];
	std::string targetName = tokens[2];
	int targetFd = srv.getClientFd(targetName);
	std::string comment = eraseColon(tokens, 4);
	std::string kickerName = srv.getClient(kickerFd).getNickname();
	srv.kickClient(kickerName, targetFd, channelName, targetName, comment);
}

void cmdPrivMsg(Server &srv, int senderFd, const std::vector<std::string> &tokens)
{
	if (errorPrivMsg(srv, senderFd, tokens))
		return ;
	std::string target = tokens[1];
	bool isChannel = false;
	if (target[0] == '#' || target[0] == '&')
		isChannel = true;
	std::string message = eraseColon(tokens, tokens.size());
	int targetFd = srv.getClientFd(target);
	std::string toSend = ":" + srv.getClient(senderFd).getNickname() + " PRIVMSG " + target + " :" + message + "\r\n";
	srv.sendPrivMsg(senderFd, targetFd, target, toSend, isChannel);
}

void cmdInvite(Server &srv, int senderFd, const std::vector<std::string> &tokens)
{
	if (errorInvite(srv, senderFd, tokens))
		return ;
	std::string targetNickname = tokens[1];
	std::string channelName = tokens[2];
	int targetFd = srv.getClientFd(targetNickname);
	srv.inviteClient(senderFd, targetFd, targetNickname, channelName);
}

void cmdTopic(Server &srv, int fd, const std::vector<std::string> &tokens)
{
	if (errorTopic(srv, fd, tokens))
		return ;
	std::string channelName = tokens[1];
	std::string topic = eraseColon(tokens, 3);
	srv.printTopic(fd, channelName, topic);
}

void cmdMode(Server &srv, int fd, const std::vector<std::string> &tokens)
{
	if (errorMode(srv, fd, tokens))
		return ;
	std::string channelName = tokens[1];
	if (tokens.size() == 2)
	{
		srv.getModes(fd, channelName);
		return ;
	}
	std::string mode = tokens[2];
	int params = 0;
	bool sign = false;
	if (mode[0] == '+')
		sign = true;
	for (size_t i = 1; i < mode.length(); i++)
	{
		char c = mode[i];
		switch (c)
		{
			case 'i':
				srv.changeInviteOnly(channelName, sign);
				break ;
			case 't':
				srv.changeTopicRestriction(channelName, sign);
				break ;
			case 'k':
			{
				std::string key = "";
				if (!fillArg(srv, fd, tokens, key, params))
					return ;
				srv.changeKey(channelName, key, sign);
				break ;
			}
			case 'o':
			{
				std::string user = "";
				if (!fillArg(srv, fd, tokens, user, params))
					return ;
				int targetFd = srv.getClientFd(user);
				srv.changeOperator(channelName, targetFd, sign);
				break ;
			}
			case 'l':
			{
				std::string limit = "";
				if (!fillArg(srv, fd, tokens, limit, params))
					return ;
				srv.changeUserLimit(fd, channelName, limit, sign);
				break ;
			}
			default :
				srv.sendError(fd, "461", tokens[0], "Invalid parameter");
				return ;
		}
	}
}

void cmdList(Server &srv, int fd, const std::vector<std::string> &tokens)
{
	if (errorParams(srv, fd, tokens, 1, 1))
		return ;
	std::map<std::string, Channel> chanList = srv.getChannelList();
	std::string toSend;
	for (std::map<std::string, Channel>::iterator it = chanList.begin(); it != chanList.end(); ++it)
		toSend += it->second.getChannelName() + " ";
	toSend += "\r\n";
	send(fd, toSend.c_str(), toSend.length(), 0);
}

bool fillArg(Server &srv, int fd, const std::vector<std::string> tokens, std::string &arg, int &params)
{
	if (!params && tokens.size() >= 4)
		arg = tokens[3];
	else if (params == 1 && tokens.size() >= 5)
		arg = tokens[4];
	else if (params == 2 && tokens.size() == 6)
		arg = tokens[5];
	else
	{
		srv.sendError(fd, "461", tokens[0], "Not enough parameters");
		return (false);
	}
	params++;
	return (true);
}

bool errorPass(Server &srv, int fd, const std::vector<std::string> tokens)
{
	if (errorParams(srv, fd, tokens, 2, 2) || isAuthenticated(srv, fd, tokens[0]))
		return (true);
	std::string password = tokens[1];
	if (password != srv.getPassword())
	{
		srv.sendError(fd, "464", tokens[0], "Password incorrect");
		return (true);
	}
	return (false);
}

bool errorUser(Server &srv, int fd, const std::vector<std::string> tokens)
{
	if (tokens.size() == 5 && tokens[4][0] != ':')
		srv.sendError(fd, "461", tokens[0], "Invalid parameter");
	else if (tokens[1].length() > 10)
		srv.sendError(fd, "468", tokens[1], "Erroneous username");
	else if (tokens.size() == 5 && tokens[4]. length() > 100)
		srv.sendError(fd, "468", tokens[4], "Erroneous realname");
	else
		return (false);
	return (true);
}

bool errorNick(Server &srv, int fd, const std::vector<std::string> tokens)
{
	if (!isAuthenticated(srv, fd, tokens[0]) || errorParams(srv, fd, tokens, 2, 2))
		return (true);
	std::string nickname = tokens[1];
	if (nickname.length() > 9)
	{
		srv.sendError(fd, "432", nickname, "Erroneous nickname");
		return (true);
	}
	std::map<int, Client> clientsList = srv.getClientsList();
	for (std::map<int, Client>::iterator it = clientsList.begin(); it != clientsList.end(); ++it)
	{
		if (nickname == it->second.getNickname())
		{
			srv.sendError(fd, "433", nickname, "Nickname is already in use");
			return (true);
		}
	}
	return (false);
}

bool errorJoin(Server &srv, int fd, const std::vector<std::string> tokens)
{
	if (!isAuthenticated(srv, fd, tokens[0]) || !isRegistered(srv, fd, tokens[0]) || errorParams(srv, fd, tokens, 2, 3))
		return (true);
	std::vector<std::string> channelsName = split(tokens[1], ',');
	for (std::vector<std::string>::iterator it = channelsName.begin(); it != channelsName.end(); ++it)
	{
		const std::string &channel = *it;
		if (channel.empty() || (channel[0] != '#' && channel[0] != '&') || channel.length() > 50)
		{
			srv.sendError(fd, "476", "JOIN" + channel, "Bad Channel Mask");
			return (true);
		}
		for (size_t i = 1; i < channel.length(); ++i)
		{(void)tokens;
			if (channel[i] == ' ' || channel[i] == ',' || channel[i] < 0x20)
			{
				srv.sendError(fd, "476", "JOIN" + channel, "Bad Channel Mask");
				return (true);
			}
		}
	}
	return (false);
}

bool errorKick(Server &srv, int kickerFd, const std::vector<std::string> tokens)
{
	if (!isAuthenticated(srv, kickerFd, tokens[0]) || !isRegistered(srv, kickerFd, tokens[0]) || errorParams(srv, kickerFd, tokens, 3, 4))
		return (true);
	std::string channelName = tokens[1];
	std::string targetName = tokens[2];
	int targetFd = srv.getClientFd(targetName);
	Channel &chan = srv.getChannel(channelName);
	if (tokens.size() == 4 && tokens[3][0] != ':')
		srv.sendError(kickerFd, "461", tokens[0], "Invalid parameter");
	else if (!srv.isChannelExist(channelName))
		srv.sendError(kickerFd, "403", channelName, "No such channel");
	else if (!chan.isMember(kickerFd))
		srv.sendError(kickerFd, "442", channelName, "You're not on that channel");
	else if (!chan.isOperator(kickerFd))
		srv.sendError(kickerFd, "482", srv.getClient(kickerFd).getNickname() + " " + channelName, "You're not channel operator");
	else if (!chan.isMember(targetFd))
		srv.sendError(kickerFd, "441", targetName + " " + channelName, "They aren't on that channel");
	else
		return (false);
	return (true);
}

bool errorPrivMsg(Server &srv, int senderFd, const std::vector<std::string> tokens)
{
	if (!isAuthenticated(srv, senderFd, tokens[0]) || !isRegistered(srv, senderFd, tokens[0]) || errorParams(srv, senderFd, tokens, 3, 3))
		return (true);
	std::string target = tokens[1];
	bool isChannel = false;
	if (target[0] == '#' || target[0] == '&')
		isChannel = true;
	if (tokens[2][0] != ':')
		srv.sendError(senderFd, "412", "PRIVMSG", "No text to send");
	else if (isChannel)
	{
		if (srv.isChannelExist(target))
			return (false);
		srv.sendError(senderFd, "403", target, "No such channel");
	}
	else if (!isChannel)
	{
		if (srv.isNicknameExist(target))
			return (false);
		srv.sendError(senderFd, "401", target, "No such nick");
	}
	return (true);
}

bool errorInvite(Server &srv, int senderFd, const std::vector<std::string> tokens)
{
	if (!isAuthenticated(srv, senderFd, tokens[0]) || !isRegistered(srv, senderFd, tokens[0]) || errorParams(srv, senderFd, tokens, 3, 3))
		return (true);
	std::string targetNickname = tokens[1];
	std::string channelName = tokens[2];
	if (!srv.isNicknameExist(targetNickname))
		srv.sendError(senderFd, "401", targetNickname, "No such nick");
	else if (!srv.isChannelExist(channelName))
		srv.sendError(senderFd, "403", channelName, "No such channel");
	else if (!srv.getChannel(channelName).isMember(senderFd))
		srv.sendError(senderFd, "442", channelName, "You're not on that channel");
	else if (srv.getChannel(channelName).isMember(srv.getClientFd(targetNickname)))
		srv.sendError(senderFd, "443", targetNickname, "is already on channel");
	else
		return (false);
	return (true);
}

bool errorTopic(Server &srv, int fd, const std::vector<std::string> tokens)
{
	if (!isAuthenticated(srv, fd, tokens[0]) || !isRegistered(srv, fd, tokens[0]) || errorParams(srv, fd, tokens, 2, 3))
		return (true);
	std::string channelName = tokens[1];
	if (tokens.size() == 3 && tokens[2][0] != ':')
		srv.sendError(fd, "461", tokens[0], "Invalid parameter");
	else if (!srv.isChannelExist(channelName))
		srv.sendError(fd, "403", channelName, "No such channel");
	else if (!srv.getChannel(channelName).isMember(fd))
		srv.sendError(fd, "442", channelName, "You're not on that channel");
	else
		return (false);
	return (true);
}

bool errorMode(Server &srv, int fd, const std::vector<std::string> tokens)
{
	if (!isAuthenticated(srv, fd, tokens[0]) || !isRegistered(srv, fd, tokens[0]))
		return (true);
	if (tokens.size() < 2)
	{
		srv.sendError(fd, "461", tokens[0], "Not enough parameters");
		return (true);
	}
	if (tokens.size() == 2)
		return (false);
	std::string channelName = tokens[1];
	if (!srv.getChannel(channelName).isOperator(fd))
		srv.sendError(fd, "482", srv.getClient(fd).getNickname() + " " + channelName, "You're not channel operator");
	else if (tokens[2][0] != '+' && tokens[2][0] != '-')
		srv.sendError(fd, "461", tokens[0], "Invalid parameter");
	else
		return (false);
	return (true);
}

bool errorParams(Server &srv, int fd, std::vector<std::string> tokens, size_t min_params, size_t max_param)
{
	if (tokens.size() < min_params)
		srv.sendError(fd, "461", tokens[0], "Not enough parameters");
	else if (tokens.size() > max_param)
		srv.sendError(fd, "461", tokens[0], "Too many parameters");
	else
		return (false);
	return (true);
}

bool isRegistered(Server &srv, int fd, std::string cmd)
{
	Client client = srv.getClient(fd);
	if (client.isUserSet() && cmd == "USER")
	{
		srv.sendError(fd, "462", cmd, "You may not reregister");
		return (true);
	}
	else if (!client.isUserSet() && cmd == "USER")
		return (false);
	else if ((!client.isUserSet() || !client.isNickSet()) && cmd != "USER" && cmd != "NICK")
	{
		srv.sendError(fd, "461", cmd, "You are not registered");
		return (false);
	}
	return (true);
}

bool isAuthenticated(Server &srv, int fd, std::string cmd)
{
	bool authenticated = srv.getClient(fd).isAuthenticated();
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


