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
#include <csignal>
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
#include "CommandHandler.hpp"
#include "Client.hpp"

class Channel;

class Server
{
	private:
		std::string _password;
		int _port;
		int _socketFd;
		struct sockaddr_in _addr;
		std::map<int, Client> _clientsMap;
		std::map<std::string, Channel> _channelsMap;
		bool _signal;

	public:
		Server() : _signal(false) {};
		~Server() {};
		void parsing(const std::string &port, const std::string &password);
		void createSocket();
		void run();
		void handleNewConnection(PollManager& pollManager, std::map<int, std::string> &clientsBuffer);
		bool handleClientMessage(int clientFd, std::map<int, std::string>& clientsBuffer);
		bool joinChannel(int fd, const std::string &channelName, const std::string &key);
		void quitChannels(int fd);
		void kickClient(int kickerFd, int fd, const std::string &channelName, const std::string &nickname, const std::string &comment);
		void inviteClient(int senderFd, int targetFd, const std::string &targetNickname, const std::string &channelName);
		bool setTopic(int fd, const std::string &topic, const std::string &channelName);
		void printTopic(int fd, const std::string &channelName, const std::string &topic, bool serverMsg);
		void broadcast(int senderFd, const std::string &message, const bool toOthers);
		void broadcastInChannel(int senderFd, const std::string &message, const std::string &channelName, const bool toOthers);
		void broadcastForJoin(int fd, const std::string &channel, const std::string &key);
		bool sendError(int fd, const std::string &code, const std::string &arg, const std::string &msg);
		void sendPrivMsg(int senderFd, int receiverFd, const std::string &target, const std::string &message, bool isChannel);
		void getModes(int fd, const std::string &channelName);
		bool isChannelExist(const std::string &channelName);
		bool isNicknameExist(const std::string &nickname);
		void changeInviteOnly(const std::string &channelName, const bool mode);
		void changeTopicRestriction(const std::string &channelName, const bool mode);
		void changeKey(const std::string &channelName, const std::string &key, const bool mode);
		void changeUserLimit(int fd, const std::string &channelName, std::string &limit, const bool mode);
		void changeOperator(int executorFd, const std::string &channelName, const std::string &targetName, const bool mode);
		void removeClient(int fd);
		void deleteChannel(const std::string &channelName);
		std::map<int, Client> &getClientsList() {return (this->_clientsMap);};
		std::map<std::string, Channel> &getChannelList() {return (this->_channelsMap);};
		Channel &getChannel(const std::string &channelName) {return (this->_channelsMap[channelName]);};
		Client &getClient(int fd) {return (this->_clientsMap[fd]);};
		std::string &getPassword() {return (this->_password);};
		int getClientFd(const std::string &name);
		void handleSignal();
		void disconnectAllClients();
};

