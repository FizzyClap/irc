/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: roespici <roespici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 16:03:33 by peli              #+#    #+#             */
/*   Updated: 2025/07/28 16:29:53 by roespici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#include "parsing.hpp"

void	cleanMessage(std::string &message);

server::server()
{
}

server::~server()
{
}

server::server(const server &other)
{
    (void)other;
};

server& server::operator = (const server& other)
{
    if (this != &other)
    {
    }
    return (*this);
};

void server::parsing(char *argv1, char *argv2)
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
    creat_socket();
};

void server::creat_socket()
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

void server::run()
{
    client client_;
    client_.add_client(socket_fd);
    while (true)
    {
        std::vector<pollfd>& poll_fds = client_.get_pollfds();
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
                    client_.add_client(new_client);
					ClientInfos infos;
					infos.fd = new_client;
					infos.nickname = "";
					infos.username = "";
					infos.hostname = "";
					infos.servername = "";
					infos.realname = "";
					infos.authenticated = false;
					infos.registered = false;
					infos.userSet = false;
					infos.nickSet = false;
					clientsMap[new_client] = infos;
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
					parseCommands(*this, client_, poll_fds[i].fd, msg);
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

void    send_to_client(int fd, const std::string& message)
{
    ssize_t sent = send(fd, message.c_str(), message.length(), 0);

    if (sent < 0) {
        std::cerr << "send error to client " << fd << ": " << strerror(errno) << std::endl;
    }
};

void	cleanMessage(std::string &message)
{
	message.erase(message.find_last_not_of(" \r\n") + 1);
}

bool server::joinChannel(int fd, const std::string &channelName, const std::string &key)
{
	if (channelsMap.find(channelName) == channelsMap.end())
	{
		channelsMap[channelName].name = channelName;
		channelsMap[channelName].topic = "";
		channelsMap[channelName].inviteOnly = false;
		channelsMap[channelName].topicRestricted = false;
		channelsMap[channelName].keyEnabled = false;
		channelsMap[channelName].userLimitEnabled = false;
		channelsMap[channelName].key = "";
		channelsMap[channelName].userLimit = 0;
		if (channelsMap[channelName].operators.empty())
			channelsMap[channelName].operators.insert(fd);
	}
	if (this->getModeInvite(channelName) && !this->getIsInvited(fd, channelName))
		this->sendError(fd, "473", this->getClientNickname(fd) + " " + channelName, "Cannot join channel (+i)");
	else if (this->getModeKey(channelName) && this->getKeyPass(channelName) != key)
		this->sendError(fd, "475", this->getClientNickname(fd) + " " + channelName, "Cannot join channel (+k)");
	else if (this->getModeLimit(channelName) && this->getLimitUser(channelName) == this->getNbUser(channelName))
		this->sendError(fd, "471", this->getClientNickname(fd) + " " + channelName, "Cannot join channel (+l)");
	else
	{
		channelsMap[channelName].members.insert(fd);
		return (true);
	}
	return (false);
}

void server::setClientNickname(int fd, const std::string &nickname)
{
	clientsMap[fd].nickname = nickname;
}

void server::setClientUsername(int fd, const std::string &username)
{
	clientsMap[fd].username = username;
}

void server::setClientHostname(int fd, const std::string &hostname)
{
	clientsMap[fd].hostname = hostname;
}

void server::setClientServername(int fd, const std::string &servername)
{
	clientsMap[fd].servername = servername;
}

void server::setClientRealname(int fd, const std::string &realname)
{
	clientsMap[fd].realname = realname;
}

int server::getClientFd(const std::string &nickname)
{
	for (std::map<int, ClientInfos>::iterator it = clientsMap.begin(); it != clientsMap.end(); ++it)
	{
		if (it->second.nickname == nickname)
			return (it->first);
	}
	return (-1);
}

void server::broadcast(int senderFd, const std::string &message)
{
	for (std::map<int, ClientInfos>::iterator it = clientsMap.begin(); it != clientsMap.end(); ++it)
	{
		int fd = it->first;
		if (fd != senderFd && it->second.registered)
			send(fd, message.c_str(), message.length(), 0);
	}
}

void server::kickClient(int fd, const std::string &channelName, const std::string &nickname, const std::string &comment)
{
	std::map<std::string, Channel>::iterator it = channelsMap.find(channelName);
	if (it != channelsMap.end())
	{
		it->second.members.erase(fd);
		std::string kickMsg;
		if (comment.empty())
			kickMsg = "Kick " + nickname + " from " + channelName + "\r\n";
		else
			kickMsg = "Kick " + nickname + " from " + channelName + " using \"" + comment + "\" as the reason\r\n";
		this->broadcast(fd, kickMsg);
	}
}

void server::sendError(int fd, const std::string &code, const std::string &arg, const std::string &msg)
{
	std::ostringstream oss;
	oss << ":ircserv " << code << " * " << arg << " :" << msg << "\r\n";
	send(fd, oss.str().c_str(), oss.str().size(), 0);
}

bool server::isMemberOfChannel(int fd, const std::string &channelName)
{
	std::map<std::string, Channel>::iterator it = channelsMap.find(channelName);
	if (it == channelsMap.end())
		return (false);
	return (it->second.members.find(fd) != it->second.members.end());
}

void server::sendPrivMsg(int senderFd, int receiverFd, const std::string &channelName, const std::string &message, bool isChannel)
{
	for (std::map<int, ClientInfos>::iterator it = clientsMap.begin(); it != clientsMap.end(); ++it)
	{
		int fd = it->second.fd;
		if ((fd == receiverFd && !isChannel) || (fd != senderFd && isMemberOfChannel(fd, channelName) && isChannel))
			send(fd, message.c_str(), message.length(), 0);
	}
}

bool server::isChannelExist(const std::string &channelName)
{
	std::map<std::string, Channel> channelsList = this->getChannelList();
	for (std::map<std::string, Channel>::iterator it = channelsList.begin(); it != channelsList.end(); ++it)
		if (it->second.name == channelName)
			return (true);
	return (false);
}

bool server::isNicknameExist(const std::string &nickname)
{
	std::map<int, ClientInfos> clientsList = this->getClientsList();
	for (std::map<int, ClientInfos>::iterator it = clientsList.begin(); it != clientsList.end(); ++it)
		if (it->second.nickname == nickname)
			return (true);
	return (false);
}

void server::inviteClient(int senderFd, int targetFd, const std::string &targetNickname, const std::string &channelName)
{
	std::string senderNickname = this->getClientNickname(senderFd);
	std::string toSend = ":" + senderNickname + " INVITE " + targetNickname + " :" + channelName + "\r\n";
	send(targetFd, toSend.c_str(), toSend.length(), 0);
	this->channelsMap[channelName].invited.insert(targetFd);
}

bool server::setTopic(int fd, const std::string &topic, const std::string &channelName)
{
	if (this->getModeTopic(channelName) && !this->isOperator(fd, channelName))
	{
		this->sendError(fd, "482", channelName, "You're not channel operator");
		return (false);
	}
	channelsMap[channelName].topic = topic;
	return (true);
}

void server::printTopic(int fd, const std::string &channelName, const std::string &newTopic)
{
	if (!newTopic.empty())
		if (!this->setTopic(fd, newTopic, channelName))
			return ;
	std::string topic = channelsMap[channelName].topic;
	for (std::map<int, ClientInfos>::iterator it = clientsMap.begin(); it != clientsMap.end(); ++it)
	{
		std::string message = "";
		if (!topic.empty())
			message = ":" + it->second.nickname + " TOPIC " + channelName + " :" + topic + "\r\n";
		else
			message = ":" + it->second.nickname + " TOPIC " + channelName + " :No topic is set\r\n";
		if (isMemberOfChannel(it->second.fd, channelName))
			send(it->second.fd, message.c_str(), message.length(), 0);
	}
}

bool server::isOperator(int fd, const std::string &channelName)
{
	std::map<std::string, Channel>::iterator it = channelsMap.find(channelName);
	if (it == channelsMap.end())
		return (false);
	return (it->second.operators.find(fd) != it->second.operators.end());
}

void server::addOperator(int fd, const std::string &channelName)
{
	channelsMap[channelName].operators.insert(fd);
}

void server::retireOperator(int fd, const std::string &channelName)
{
	channelsMap[channelName].operators.erase(fd);
}

void server::enableInviteOnly(const std::string &channelName)
{
	if (!channelsMap[channelName].inviteOnly)
		channelsMap[channelName].inviteOnly = true;
}

void server::disableInviteOnly(const std::string &channelName)
{
	if (channelsMap[channelName].inviteOnly)
		channelsMap[channelName].inviteOnly = false;
}

void server::enableTopicRestriction(const std::string &channelName)
{
	if (!channelsMap[channelName].topicRestricted)
		channelsMap[channelName].topicRestricted = true;
}

void server::disableTopicRestriction(const std::string &channelName)
{
	if (channelsMap[channelName].topicRestricted)
		channelsMap[channelName].topicRestricted = false;
}

void server::enableKey(const std::string &channelName, const std::string &key)
{
	if (!channelsMap[channelName].keyEnabled)
		channelsMap[channelName].keyEnabled = true;
	channelsMap[channelName].key = key;
}

void server::disableKey(const std::string &channelName)
{
	if (channelsMap[channelName].keyEnabled)
		channelsMap[channelName].keyEnabled = false;
	channelsMap[channelName].key = "";
}

void server::enableUserLimit(int fd, const std::string &channelName, std::string &limit)
{
	int newLimit = atoi(limit);
	if (newLimit == 0)
	{
		this->sendError(fd, "696", this->getClientNickname(fd) + " " + channelName + " l", "Invalid limit");
		return ;
	}
	if (!channelsMap[channelName].userLimitEnabled)
		channelsMap[channelName].userLimitEnabled = true;
	channelsMap[channelName].userLimit = newLimit;
}

void server::disableUserLimit(const std::string &channelName)
{
	if (channelsMap[channelName].userLimitEnabled)
		channelsMap[channelName].userLimitEnabled = false;
	channelsMap[channelName].userLimit = 0;
}

bool server::getIsInvited(int fd, const std::string &channelName)
{
	if (channelsMap[channelName].invited.find(fd) != channelsMap[channelName].invited.end())
		return (true);
	return (false);
}

int server::atoi(const std::string &str)
{
	std::istringstream iss(str);
	int result;
	iss >> result;
	return (result);
}
