#include "parsing.hpp"

std::vector<std::string> split(const std::string &msg);

typedef void (*CommandHandler)(server&, int, const std::vector<std::string>&);

void parseCommands(server &srv, int fd, const std::string &msg)
{
	std::string cmds[] = {"PASS", "NICK", "USER", "JOIN", "KICK", "PRIVMSG", "INVITE", "TOPIC", "MODE"};
	CommandHandler handlers[] = {
		cmdPass, cmdNick, cmdUser, cmdJoin, cmdKick, cmdPrivMsg, cmdInvite, cmdTopic, cmdMode
	};
	std::vector<std::string> tokens = split(msg);
	for (int i = 0; i < 9; i++)
	{
		if (tokens[0] == cmds[i])
		{
			handlers[i](srv, fd, tokens);
			break;
		}
	}
}

void cmdPass(server &srv, int fd, const std::vector<std::string> &tokens)
{
	if (errorPass(srv, fd, tokens))
		return ;
	srv.getClientInfos(fd).authenticated = true;
}

void cmdUser(server &srv, int fd, const std::vector<std::string> &tokens)
{
	if (!isAuthenticated(srv, fd, tokens[0]) || isRegistered(srv, fd, tokens[0]) || errorParams(srv, fd, tokens, 4, 5) || errorUser(srv, fd, tokens))
		return ;
	srv.setClientUsername(fd, tokens[1]);
	srv.setClientHostname(fd, tokens[2]);
	srv.setClientServername(fd, tokens[3]);
	std::string realname = eraseColon(tokens, 5);
	if (!realname.empty())
		srv.setClientRealname(fd, realname);
	srv.getClientInfos(fd).userSet = true;
	welcomeMessage(srv, fd);
}

void cmdNick(server &srv, int fd, const std::vector<std::string> &tokens)
{
	if (errorNick(srv, fd, tokens))
		return ;
	std::string nickname = tokens[1];
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
	if (errorJoin(srv, fd, tokens))
		return ;
	std::string channelName = tokens[1];
	std::string key = eraseColon(tokens, 3);
	if (!srv.joinChannel(fd, channelName, key))
		return ;
	std::string	joinMsg = ":" + srv.getClientNickname(fd) + " JOIN :" + channelName + "\r\n";
	srv.broadcast(fd, joinMsg);
}

void cmdKick(server &srv, int kickerFd, const std::vector<std::string> &tokens)
{
	if (errorKick(srv, kickerFd, tokens))
		return ;
	std::string channelName = tokens[1];
	std::string targetName = tokens[2];
	int targetFd = srv.getClientFd(targetName);
	std::string comment = eraseColon(tokens, 4);
	srv.kickClient(targetFd, channelName, targetName, comment);
}

void cmdPrivMsg(server &srv, int senderFd, const std::vector<std::string> &tokens)
{
	if (errorPrivMsg(srv, senderFd, tokens))
		return ;
	std::string target = tokens[1];
	bool isChannel = false;
	if (target[0] == '#' || target[0] == '&')
		isChannel = true;
	std::string message = eraseColon(tokens, tokens.size());
	int targetFd = srv.getClientFd(target);
	std::string toSend = ":" + srv.getClientNickname(senderFd) + " PRIVMSG " + target + " :" + message + "\r\n";
	srv.sendPrivMsg(senderFd, targetFd, target, message, isChannel);
}

void cmdInvite(server &srv, int senderFd, const std::vector<std::string> &tokens)
{
	if (errorInvite(srv, senderFd, tokens))
		return ;
	std::string targetNickname = tokens[1];
	std::string channelName = tokens[2];
	int targetFd = srv.getClientFd(targetNickname);
	srv.inviteClient(senderFd, targetFd, targetNickname, channelName);
}

void cmdTopic(server &srv, int fd, const std::vector<std::string> &tokens)
{
	if (errorTopic(srv, fd, tokens))
		return ;
	std::string channelName = tokens[1];
	std::string topic = eraseColon(tokens, 3);
	srv.printTopic(fd, channelName, topic);
}

void cmdMode(server &srv, int fd, const std::vector<std::string> &tokens)
{
	if (errorMode(srv, fd, tokens))
		return ;
	std::string channelName = tokens[1];
	std::string mode = tokens[2];
	int params = 0;
	bool sign = false;
	if (mode[0] == '+')
		sign = true;
	for (size_t i = 1; i < mode.length(); i++)
	{
		char c = mode[i];
		if (c == 'i')
			srv.changeInviteOnly(channelName, sign);
		else if (c == 't')
				srv.changeTopicRestriction(channelName, sign);
		else if (c == 'k')
		{
			std::string key = "";
			if (!fillArg(srv, fd, tokens, key, params))
				return ;
			srv.changeKey(channelName, key, sign);
		}
		else if (c == 'o')
		{
			std::string user = "";
			if (!fillArg(srv, fd, tokens, user, params))
				return ;
			int targetFd = srv.getClientFd(user);
			srv.changeOperator(channelName, targetFd, sign);
		}
		else if (c == 'l')
		{
			std::string limit = "";
			if (!fillArg(srv, fd, tokens, limit, params))
				return ;
			srv.changeUserLimit(fd, channelName, limit, sign);
		}
		else
		{
			srv.sendError(fd, "461", tokens[0], "Invalid parameter");
			return ;
		}
	}
}

bool fillArg(server &srv, int fd, const std::vector<std::string> tokens, std::string &arg, int &params)
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

bool errorPass(server &srv, int fd, const std::vector<std::string> tokens)
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

bool errorUser(server &srv, int fd, const std::vector<std::string> tokens)
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

bool errorNick(server &srv, int fd, const std::vector<std::string> tokens)
{
	if (!isAuthenticated(srv, fd, tokens[0]) || errorParams(srv, fd, tokens, 2, 2))
		return (true);
	std::string nickname = tokens[1];
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

bool errorJoin(server &srv, int fd, const std::vector<std::string> tokens)
{
	if (!isAuthenticated(srv, fd, tokens[0]) || !isRegistered(srv, fd, tokens[0]) || errorParams(srv, fd, tokens, 2, 3))
		return (true);
	std::string	channelName = tokens[1];
	if (tokens.size() == 3 && tokens[2][0] != ':')
		srv.sendError(fd, "461", tokens[0], "Invalid parameter");
	else if (channelName.empty() || (channelName[0] != '#' && channelName[0] != '&') || channelName.length() > 50)
	{
		srv.sendError(fd, "476", "JOIN", "Bad Channel Mask");
		return (true);
	}
	for (size_t i = 1; i < channelName.length(); ++i)
	{
		if (channelName[i] == ' ' || channelName[i] == ',' || channelName[i] < 0x20)
		{
			srv.sendError(fd, "476", "JOIN", "Bad Channel Mask");
			return (true);
		}
	}
	return (false);
}

bool errorKick(server &srv, int kickerFd, const std::vector<std::string> tokens)
{
	if (!isAuthenticated(srv, kickerFd, tokens[0]) || !isRegistered(srv, kickerFd, tokens[0]) || !srv.getChannel(tokens[1]).isOperator(kickerFd) \
		|| errorParams(srv, kickerFd, tokens, 3, 4))
		return (true);
	std::string channelName = tokens[1];
	std::string targetName = tokens[2];
	int targetFd = srv.getClientFd(targetName);
	if (tokens.size() == 4 && tokens[3][0] != ':')
		srv.sendError(kickerFd, "461", tokens[0], "Invalid parameter");
	else if (!srv.getChannel(channelName).isMember(kickerFd))
		srv.sendError(kickerFd, "442", channelName, "You're not on that channel");
	else if (!srv.getChannel(channelName).isMember(targetFd))
		srv.sendError(kickerFd, "441", targetName + " " + channelName, "They aren't on that channel");
	else
		return (false);
	return (true);
}

bool errorPrivMsg(server &srv, int senderFd, const std::vector<std::string> tokens)
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

bool errorInvite(server &srv, int senderFd, const std::vector<std::string> tokens)
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

bool errorTopic(server &srv, int fd, const std::vector<std::string> tokens)
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

bool errorMode(server &srv, int fd, const std::vector<std::string> tokens)
{
	if (!isAuthenticated(srv, fd, tokens[0]) || !isRegistered(srv, fd, tokens[0]))
		return (true);
	if (tokens.size() < 3)
	{
		srv.sendError(fd, "461", tokens[0], "Not enough parameters");
		return (true);
	}
	std::string channelName = tokens[1];
	if (!srv.getChannel(channelName).isOperator(fd))
		srv.sendError(fd, "482", srv.getClientNickname(fd) + " " + channelName, "You're not channel operator");
	else if (tokens[2][0] != '+' && tokens[2][0] != '-')
		srv.sendError(fd, "461", tokens[0], "Invalid parameter");
	else
		return (false);
	return (true);
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
				token += leftover;
			result.push_back(token);
			break;
		}
		result.push_back(token);
	}
	return (result);
}

bool errorParams(server &srv, int fd, std::vector<std::string> tokens, size_t min_params, size_t max_param)
{
	if (tokens.size() < min_params)
		srv.sendError(fd, "461", tokens[0], "Not enough parameters");
	else if (tokens.size() > max_param)
		srv.sendError(fd, "461", tokens[0], "Too many parameters");
	else
		return (false);
	return (true);
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

std::string eraseColon(std::vector<std::string> tokens, size_t size)
{
	std::string truncate = "";
	if (tokens.size() == size)
		truncate = tokens[size - 1].substr(1);
	return (truncate);
}
