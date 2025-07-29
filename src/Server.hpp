/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Serverhpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: roespici <roespici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 16:02:06 by peli              #+#    #+#             */
/*   Updated: 2025/07/29 12:08:04 by roespici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <algorithm>
#include <poll.h>
#include <sys/socket.h>
#include <set>
#include <sstream>
#include "PollManager.hpp"
#include "Channel.hpp"
#include "Parsing.hpp"
#include "Client.hpp"

class Server
{
	private:
		std::string password;
		int port;
		int socket_fd;
		struct sockaddr_in addr;
		std::map<int, Client> clientsMap;
		std::map<std::string, Channel> channelsMap;

	public:
		Server() {};
		~Server() {};
		void parsing(char *port, char *password);
		void createSocket();
		void run();
		bool joinChannel(int fd, const std::string &channelName, const std::string &key);
		void kickClient(int fd, const std::string &channelName, const std::string &nickname, const std::string &comment);
		void inviteClient(int senderFd, int targetFd, const std::string &targetNickname, const std::string &channelName);
		bool setTopic(int fd, const std::string &topic, const std::string &channelName);
		void printTopic(int fd, const std::string &channelName, const std::string &topic);
		void broadcast(int senderFd, const std::string &message);
		void sendError(int fd, const std::string &code, const std::string &arg, const std::string &msg);
		void sendPrivMsg(int senderFd, int receiverFd, const std::string &target, const std::string &message, bool isChannel);
		bool isChannelExist(const std::string &channelName);
		bool isNicknameExist(const std::string &nickname);
		void changeInviteOnly(const std::string &channelName, const bool mode);
		void changeTopicRestriction(const std::string &channelName, const bool mode);
		void changeKey(const std::string &channelName, const std::string &key, const bool mode);
		void changeUserLimit(int fd, const std::string &channelName, std::string &limit, const bool mode);
		void changeOperator(const std::string &channelName, int fd, const bool mode);
		//GETTER
		std::map<int, Client> &getClientsList() {return (clientsMap);};
		std::map<std::string, Channel> &getChannelList() {return (channelsMap);};
		Channel &getChannel(const std::string &channelName) {return (channelsMap[channelName]);};
		Client &getClient(int fd) {return (clientsMap[fd]);};
		std::string getPassword() {return (this->password);};
		int getClientFd(const std::string &name);
};

