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

void	cleanMessage(std::string &message);

void Server::parsing(char *argv1, char *argv2)
{
	if (!argv1)
		throw std::runtime_error("port doesn't exist");
	for (size_t i = 0; i < strlen(argv1); i++)
	{
		if (!isdigit(static_cast<unsigned char>(argv1[i])))
			throw std::runtime_error("Port is not numeric");
	}
	port = atoi(argv1);
	if (port > 65535 || port < 1024)
		throw std::runtime_error("Port no utilisable");
	if (argv2 != NULL)
		password = argv2;
	createSocket();
};

void Server::createSocket()
{
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
		throw std::runtime_error ("Creat socket error");

	int yes = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
		throw std::runtime_error("setsockopt(SO_REUSEADDR) failed");

	socket_fd = server_fd;
	fcntl(socket_fd, F_SETFL, O_NONBLOCK);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port); // pourquoi faut convertir?
	addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(addr.sin_zero), 0, 8);
	if (bind(server_fd, (const sockaddr*)&addr, (socklen_t)sizeof(addr)) != 0)
		throw std::runtime_error("bind() fail");
	if (listen(server_fd, SOMAXCONN) == -1)// nombre maximum de connexions entrantes mises en attente
		throw std::runtime_error("listen error");
};

void Server::run()
{
	PollManager client_;
	client_.addClient(socket_fd);
	while (true)
	{
		std::vector<pollfd>& poll_fds = client_.getPollFds();
		int activity = poll(poll_fds.data(), poll_fds.size(), 100);// .data() → donne un pollfd*, c’est ce que poll() attend.
		if (activity < 0)
		{
			std::cerr << "poll fail: " << strerror(errno) << std::endl;
			continue;
		}
		for (size_t i = 0; i < poll_fds.size(); ++i)
		{
			if (poll_fds[i].fd == socket_fd && (poll_fds[i].revents & POLLIN))
			{
					//connect;
					struct sockaddr_in client_addr;
					socklen_t client_len = sizeof(client_addr);
					int new_client = accept(socket_fd, (struct sockaddr*)&client_addr, &client_len);
					if (new_client < 0)
					{
						std::cerr << "Client connect fail: " << strerror(errno) << std::endl;
						continue;
					}
					fcntl(new_client, F_SETFL, O_NONBLOCK);
					std::cout << "Client connected: fd = " << new_client << std::endl;
					client_.addClient(new_client);
					clientsMap[new_client] = Client(new_client);
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
					cleanMessage(msg);
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

void send_to_client(int fd, const std::string& message)
{
	ssize_t sent = send(fd, message.c_str(), message.length(), 0);

	if (sent < 0) {
		std::cerr << "send error to client " << fd << ": " << strerror(errno) << std::endl;
	}
};

void cleanMessage(std::string &message)
{
	message.erase(message.find_last_not_of(" \r\n") + 1);
}

bool Server::joinChannel(int fd, const std::string &channelName, const std::string &key)
{
	if (channelsMap.find(channelName) == channelsMap.end())
	{
		channelsMap[channelName] = Channel(channelName);
		channelsMap[channelName].addOperator(fd);
	}
	Channel &chan = channelsMap[channelName];
	if (chan.getModeInvite() && !chan.getIsInvited(fd))
		this->sendError(fd, "473", this->getClient(fd).getNickname() + " " + channelName, "Cannot join channel (+i)");
	else if (chan.getModeKey() && chan.getKeyPass() != key)
		this->sendError(fd, "475", this->getClient(fd).getNickname() + " " + channelName, "Cannot join channel (+k)");
	else if (chan.getModeLimit() && chan.getLimitUser() == chan.getNbUser())
		this->sendError(fd, "471", this->getClient(fd).getNickname() + " " + channelName, "Cannot join channel (+l)");
	else
	{
		chan.addMembers(fd);
		return (true);
	}
	return (false);
}

void Server::broadcast(int senderFd, const std::string &message)
{
	for (std::map<int, Client>::iterator it = clientsMap.begin(); it != clientsMap.end(); ++it)
	{
		int fd = it->first;
		if (fd != senderFd && it->second.isRegistered())
			send(fd, message.c_str(), message.length(), 0);
	}
}

void Server::kickClient(int fd, const std::string &channelName, const std::string &nickname, const std::string &comment)
{
	std::map<std::string, Channel>::iterator it = channelsMap.find(channelName);
	if (it != channelsMap.end())
	{
		it->second.removeMembers(fd);
		std::string kickMsg;
		if (comment.empty())
			kickMsg = "Kick " + nickname + " from " + channelName + "\r\n";
		else
			kickMsg = "Kick " + nickname + " from " + channelName + " using \"" + comment + "\" as the reason\r\n";
		this->broadcast(fd, kickMsg);
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
	for (std::map<int, Client>::iterator it = clientsMap.begin(); it != clientsMap.end(); ++it)
	{
		int fd = it->second.getFd();
		if ((fd == receiverFd && !isChannel) || (fd != senderFd && this->getChannel(channelName).isMember(fd) && isChannel))
			send(fd, message.c_str(), message.length(), 0);
	}
}

bool Server::isChannelExist(const std::string &channelName)
{
	std::map<std::string, Channel> channelsList = this->getChannelList();
	for (std::map<std::string, Channel>::iterator it = channelsList.begin(); it != channelsList.end(); ++it)
		if (it->second.getChannelName() == channelName)
			return (true);
	return (false);
}

bool Server::isNicknameExist(const std::string &nickname)
{
	std::map<int, Client> clientsList = this->getClientsList();
	for (std::map<int, Client>::iterator it = clientsList.begin(); it != clientsList.end(); ++it)
		if (it->second.getNickname() == nickname)
			return (true);
	return (false);
}

void Server::inviteClient(int senderFd, int targetFd, const std::string &targetNickname, const std::string &channelName)
{
	std::string senderNickname = this->getClient(senderFd).getNickname();
	std::string toSend = ":" + senderNickname + " INVITE " + targetNickname + " :" + channelName + "\r\n";
	send(targetFd, toSend.c_str(), toSend.length(), 0);
	this->channelsMap[channelName].addInvited(targetFd);
}

bool Server::setTopic(int fd, const std::string &topic, const std::string &channelName)
{
	if (channelsMap[channelName].getModeTopic() && !this->channelsMap[channelName].isOperator(fd))
	{
		this->sendError(fd, "482", channelName, "You're not channel operator");
		return (false);
	}
	channelsMap[channelName].setTopic(topic);
	return (true);
}

void Server::printTopic(int fd, const std::string &channelName, const std::string &newTopic)
{
	if (!newTopic.empty())
		if (!this->setTopic(fd, newTopic, channelName))
			return ;
	std::string topic = channelsMap[channelName].getTopic();
	for (std::map<int, Client>::iterator it = clientsMap.begin(); it != clientsMap.end(); ++it)
	{
		std::string message = "";
		if (!topic.empty())
			message = ":" + it->second.getNickname() + " TOPIC " + channelName + " :" + topic + "\r\n";
		else
			message = ":" + it->second.getNickname() + " TOPIC " + channelName + " :No topic is set\r\n";
		if (channelsMap[channelName].isMember(fd))
			send(it->second.getFd(), message.c_str(), message.length(), 0);
	}
}

void Server::changeInviteOnly(const std::string &channelName, const bool mode)
{
	this->channelsMap[channelName].setInviteMode(mode);
}

void Server::changeTopicRestriction(const std::string &channelName, const bool mode)
{
	this->channelsMap[channelName].setTopicMode(mode);
}

void Server::changeKey(const std::string &channelName, const std::string &key, const bool mode)
{
	this->channelsMap[channelName].setKeyMode(mode, key);
}

void Server::changeUserLimit(int fd, const std::string &channelName, std::string &limit, const bool mode)
{
	if (!this->channelsMap[channelName].setLimitMode(mode, limit))
		this->sendError(fd, "696", this->getClient(fd).getNickname() + " " + channelName + " l", "Invalid limit");
}

void Server::changeOperator(const std::string &channelName, int fd, const bool mode)
{
	if (mode == true)
		this->channelsMap[channelName].addOperator(fd);
	else
		this->channelsMap[channelName].removeOperator(fd);
}

int Server::getClientFd(const std::string &name)
{
	for (std::map<int, Client>::iterator it = clientsMap.begin(); it != clientsMap.end(); ++it)
		if (it->second.getNickname() == name)
			return (it->first);
	return (-1);
}
