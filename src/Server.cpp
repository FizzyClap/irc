/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Servercpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: roespici <roespici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 16:03:33 by peli              #+#    #+#             */
/*   Updated: 2025/07/29 12:07:02 by roespici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::parsing(const std::string &argv1, const std::string &argv2)
{
	if (argv1.empty())
		throw std::runtime_error("port doesn't exist");
	for (size_t i = 0; i < argv1.length(); i++)
	{
		if (!isdigit(static_cast<unsigned char>(argv1[i])))
			throw std::runtime_error("Port is not numeric");
	}
	_port = atoi(argv1);
	if (_port > 65535 || _port < 1024)
		throw std::runtime_error("Port no utilisable");
	if (!argv2.empty())
		_password = argv2;
	createSocket();
};

void Server::createSocket()
{
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
		throw std::runtime_error ("Create socket error");
	int yes = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
		throw std::runtime_error("setsockopt(SO_REUSEADDR) failed");
	_socket_fd = server_fd;
	fcntl(_socket_fd, F_SETFL, O_NONBLOCK);
	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(_port);
	_addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(_addr.sin_zero), 0, 8);
	if (bind(server_fd, (const sockaddr*)&_addr, (socklen_t)sizeof(_addr)) != 0)
		throw std::runtime_error("bind() fail");
	if (listen(server_fd, SOMAXCONN) == -1)
		throw std::runtime_error("listen error");
};

void Server::run()
{
	PollManager client_;
	client_.addClient(_socket_fd);
	while (true)
	{
		std::vector<pollfd>& poll_fds = client_.getPollFds();
		int activity = poll(poll_fds.data(), poll_fds.size(), 100);
		if (activity < 0)
		{
			std::cerr << "poll fail: " << strerror(errno) << std::endl;
			continue;
		}
		for (size_t i = 0; i < poll_fds.size(); ++i)
		{
			if (poll_fds[i].fd == _socket_fd && (poll_fds[i].revents & POLLIN))
			{
					struct sockaddr_in client_addr;
					socklen_t client_len = sizeof(client_addr);
					int new_client = accept(_socket_fd, (struct sockaddr*)&client_addr, &client_len);
					if (new_client < 0)
					{
						std::cerr << "Client connect fail: " << strerror(errno) << std::endl;
						continue;
					}
					fcntl(new_client, F_SETFL, O_NONBLOCK);
					std::cout << "Client connected: fd = " << new_client << std::endl;
					client_.addClient(new_client);
					_clientsMap[new_client] = Client(new_client);
					continue;
			}
			if (poll_fds[i].revents & POLLIN)
			{
				char buffer[1024];
				ssize_t j = recv(poll_fds[i].fd, buffer, sizeof(buffer), 0);
				if (j > 0)
				{
					buffer[j] = '\0';
					std::string msg(buffer);
					parseCommands(*this, poll_fds[i].fd, msg);
				}
				else
				{
					close(poll_fds[i].fd);
					std::cerr << "Client " << poll_fds[i].fd << " disconnected." << std::endl;
					poll_fds.erase(poll_fds.begin() + i);
					--i;
				}
			}
		}
	}
};

bool Server::joinChannel(int fd, const std::string &channelName, const std::string &key)
{
	if (_channelsMap.find(channelName) == _channelsMap.end())
	{
		_channelsMap[channelName] = Channel(channelName);
		_channelsMap[channelName].addOperator(fd);
	}
	Channel &chan = _channelsMap[channelName];
	if (chan.getModeInvite() && !chan.getIsInvited(fd))
		sendError(fd, "473", getClient(fd).getNickname() + " " + channelName, "Cannot join channel (+i)");
	else if (chan.getModeKey() && chan.getKeyPass() != key)
		sendError(fd, "475", getClient(fd).getNickname() + " " + channelName, "Cannot join channel (+k)");
	else if (chan.getModeLimit() && chan.getLimitUser() == chan.getNbUser())
		sendError(fd, "471", getClient(fd).getNickname() + " " + channelName, "Cannot join channel (+l)");
	else
	{
		chan.addMembers(fd);
		return (true);
	}
	return (false);
}

void Server::broadcast(int senderFd, const std::string &message, bool toOthers)
{
	for (std::map<int, Client>::iterator it = _clientsMap.begin(); it != _clientsMap.end(); ++it)
	{
		int receiverFd = it->first;
		if (!getClient(receiverFd).isRegistered() || (!toOthers && receiverFd != senderFd))
			continue ;
		for (std::map<std::string, Channel>::iterator chanIt = _channelsMap.begin(); chanIt != _channelsMap.end(); ++chanIt)
		{
			Channel &channel = chanIt->second;
			if (channel.isMember(senderFd) && channel.isMember(receiverFd))
			{
				send(receiverFd, message.c_str(), message.length(), 0);
				break ;
			}
		}
	}
}

void Server::broadcastForJoin(int fd, const std::string &channel, const std::string &key)
{
	Client client = getClient(fd);
	std::string nickname = client.getNickname();
	std::string	joinMsg = ":" + nickname + " JOIN :" + channel + "\r\n";
	if (!key.empty())
		joinMsg = ":" + nickname + " JOIN :" + channel + " using key '" + key + "'\r\n";
	std::string aboutTopic = ":ircserv 332 " + nickname + " " + channel + " :" + getChannel(channel).getTopic() + "\r\n";
	std::map<int, Client> clients = getClientsList();
	std::string clientsInChan;
	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
		if (getChannel(channel).isMember(it->first))
			clientsInChan += it->second.getNickname() + " ";
	std::string alreadyIn = ":ircserv 353 " + nickname + " = " + channel + " :" + clientsInChan  + "\r\n";
	std::string endBroadcast = ":ircserv 366 " + nickname + " " + channel + " :End of /NAMES list\r\n";
	broadcast(fd, joinMsg, true);
	broadcast(fd, aboutTopic, false);
	broadcast(fd, alreadyIn, false);
	broadcast(fd, endBroadcast, false);
}

void Server::kickClient(const std::string &kickerName, int fd, const std::string &channelName, const std::string &nickname, const std::string &comment)
{
	std::map<std::string, Channel>::iterator it = _channelsMap.find(channelName);
	if (it != _channelsMap.end())
	{
		Channel &channel = it->second;
		channel.removeMembers(fd);
		std::string kickMsg;
		if (comment.empty())
			kickMsg = ":" + kickerName + " KICK " + channelName + " " + nickname + "\r\n";
		else
			kickMsg = ":" + kickerName + " KICK " + channelName + " " + nickname + " :" + comment + "\r\n";
		for (std::map<int, Client>::iterator clientIt = _clientsMap.begin(); clientIt != _clientsMap.end(); ++clientIt)
		{
			if (!clientIt->second.isRegistered())
				continue ;
			int clientFd = clientIt->first;
			if (channel.isMember(clientFd) || clientFd == fd)
				send(clientFd, kickMsg.c_str(), kickMsg.length(), 0);
		}
	}
}

void Server::sendError(int fd, const std::string &code, const std::string &arg, const std::string &msg)
{
	std::ostringstream oss;
	oss << ":ircserv " << code << " * " << arg << " :" << msg << "\r\n";
	send(fd, oss.str().c_str(), oss.str().size(), 0);
}

void Server::sendPrivMsg(int senderFd, int receiverFd, const std::string &channelName, const std::string &message, bool isChannel)
{
	for (std::map<int, Client>::iterator it = _clientsMap.begin(); it != _clientsMap.end(); ++it)
	{
		int fd = it->second.getFd();
		if ((fd == receiverFd && !isChannel) || (fd != senderFd && getChannel(channelName).isMember(fd) && isChannel))
			send(fd, message.c_str(), message.length(), 0);
	}
}

bool Server::isChannelExist(const std::string &channelName)
{
	std::map<std::string, Channel> channelsList = getChannelList();
	for (std::map<std::string, Channel>::iterator it = channelsList.begin(); it != channelsList.end(); ++it)
		if (it->second.getChannelName() == channelName)
			return (true);
	return (false);
}

bool Server::isNicknameExist(const std::string &nickname)
{
	std::map<int, Client> clientsList = getClientsList();
	for (std::map<int, Client>::iterator it = clientsList.begin(); it != clientsList.end(); ++it)
		if (it->second.getNickname() == nickname)
			return (true);
	return (false);
}

void Server::inviteClient(int senderFd, int targetFd, const std::string &targetNickname, const std::string &channelName)
{
	std::string senderNickname = getClient(senderFd).getNickname();
	std::string toSend = ":" + senderNickname + " INVITE " + targetNickname + " :" + channelName + "\r\n";
	send(targetFd, toSend.c_str(), toSend.length(), 0);
	_channelsMap[channelName].addInvited(targetFd);
}

bool Server::setTopic(int fd, const std::string &topic, const std::string &channelName)
{
	if (_channelsMap[channelName].getModeTopic() && !_channelsMap[channelName].isOperator(fd))
	{
		sendError(fd, "482", channelName, "You're not channel operator");
		return (false);
	}
	_channelsMap[channelName].setTopic(topic);
	return (true);
}

void Server::printTopic(int fd, const std::string &channelName, const std::string &newTopic)
{
	if (!newTopic.empty())
		if (!setTopic(fd, newTopic, channelName))
			return ;
	std::string topic = _channelsMap[channelName].getTopic();
	for (std::map<int, Client>::iterator it = _clientsMap.begin(); it != _clientsMap.end(); ++it)
	{
		std::string message = "";
		if (!topic.empty())
			message = ":" + it->second.getNickname() + " TOPIC " + channelName + " :" + topic + "\r\n";
		else
			message = ":" + it->second.getNickname() + " TOPIC " + channelName + " :No topic is set\r\n";
		if (_channelsMap[channelName].isMember(fd))
			send(it->second.getFd(), message.c_str(), message.length(), 0);
	}
}

void Server::changeInviteOnly(const std::string &channelName, const bool mode)
{
	_channelsMap[channelName].setInviteMode(mode);
}

void Server::changeTopicRestriction(const std::string &channelName, const bool mode)
{
	_channelsMap[channelName].setTopicMode(mode);
}

void Server::changeKey(const std::string &channelName, const std::string &key, const bool mode)
{
	_channelsMap[channelName].setKeyMode(mode, key);
}

void Server::changeUserLimit(int fd, const std::string &channelName, std::string &limit, const bool mode)
{
	if (!_channelsMap[channelName].setLimitMode(mode, limit))
		sendError(fd, "696", getClient(fd).getNickname() + " " + channelName + " l", "Invalid limit");
}

void Server::changeOperator(const std::string &channelName, int fd, const bool mode)
{
	if (mode == true)
		_channelsMap[channelName].addOperator(fd);
	else
		_channelsMap[channelName].removeOperator(fd);
}

int Server::getClientFd(const std::string &name)
{
	for (std::map<int, Client>::iterator it = _clientsMap.begin(); it != _clientsMap.end(); ++it)
		if (it->second.getNickname() == name)
			return (it->first);
	return (-1);
}
