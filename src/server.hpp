/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: roespici <roespici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 16:02:06 by peli              #+#    #+#             */
/*   Updated: 2025/07/28 16:28:24 by roespici         ###   ########.fr       */
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
#include "client.hpp"

struct Channel
{
	std::string	name;
	std::string	topic;
	std::set<int> members;
	std::set<int> operators;
	std::set<int> invited;
	bool inviteOnly;
	bool topicRestricted;
	bool keyEnabled;
	std::string key;
	bool userLimitEnabled;
	int userLimit;
};

struct ClientInfos
{
	int fd;
	std::string nickname;
	std::string username;
	std::string hostname;
	std::string servername;
	std::string realname;
	bool authenticated;
	bool registered;
	bool userSet;
	bool nickSet;
};

class server
{
	private:
		std::string password;
		int port;
		int socket_fd;
		struct sockaddr_in addr;
		std::map<int, ClientInfos> clientsMap;
		std::map<std::string, Channel> channelsMap;

	public:
		server();
		~server();
		server(const server &other);
		server& operator = (const server& other);
		void parsing(char *port, char *password);
		void creat_socket();
		void run();
		bool joinChannel(int fd, const std::string &channelName, const std::string &key);
		void kickClient(int fd, const std::string &channelName, const std::string &nickname, const std::string &comment);
		void inviteClient(int senderFd, int targetFd, const std::string &targetNickname, const std::string &channelName);
		void printTopic(int fd, const std::string &channelName, const std::string &topic);
		void broadcast(int senderFd, const std::string &message);
		void sendError(int fd, const std::string &code, const std::string &arg, const std::string &msg);
		void sendPrivMsg(int senderFd, int receiverFd, const std::string &target, const std::string &message, bool isChannel);
		bool isMemberOfChannel(int fd, const std::string &channelName);
		bool isChannelExist(const std::string &channelName);
		bool isNicknameExist(const std::string &nickname);
		bool isOperator(int fd, const std::string &channelName);
		void addOperator(int fd, const std::string &channelName);
		void retireOperator(int fd, const std::string &channelName);
		void enableInviteOnly(const std::string &channelName);
		void disableInviteOnly(const std::string &channelName);
		void enableTopicRestriction(const std::string &channelName);
		void disableTopicRestriction(const std::string &channelName);
		void enableKey(const std::string &channelName, const std::string &key);
		void disableKey(const std::string &channelName);
		void enableUserLimit(int fd, const std::string &channelName, std::string &limit);
		void disableUserLimit(const std::string &channelName);
		int atoi(const std::string &str);
		//SETTER
		void setClientNickname(int fd, const std::string &nickname);
		void setClientUsername(int fd, const std::string &username);
		void setClientHostname(int fd, const std::string &hostname);
		void setClientServername(int fd, const std::string &servername);
		void setClientRealname(int fd, const std::string &realname);
		bool setTopic(int fd, const std::string &topic, const std::string &channelName);
		//GETTER
		std::string getClientNickname(int fd) {return (clientsMap[fd].nickname);};
		std::string getClientUsername(int fd) {return (clientsMap[fd].username);};
		std::string getClientHostname(int fd) {return (clientsMap[fd].hostname);};
		std::string getClientServername(int fd) {return (clientsMap[fd].servername);};
		std::string getClientRealname(int fd) {return (clientsMap[fd].realname);};
		int getClientFd(const std::string &nickname);
		std::map<int, ClientInfos> &getClientsList() {return (clientsMap);};
		std::map<std::string, Channel> &getChannelList() {return (channelsMap);};
		ClientInfos& getClientInfos(int fd) {return (clientsMap[fd]);};
		std::string getPassword() { return (this->password);};
		std::string getTopic(const std::string &channelName) {return (channelsMap[channelName].topic);};
		bool getModeInvite(const std::string &channelName) {return (channelsMap[channelName].inviteOnly);};
		bool getModeTopic(const std::string &channelName) {return (channelsMap[channelName].topicRestricted);};
		bool getModeKey(const std::string &channelName) {return (channelsMap[channelName].keyEnabled);};
		bool getModeLimit(const std::string &channelName) {return (channelsMap[channelName].userLimitEnabled);};
		std::string getKeyPass(const std::string &channelName) {return (channelsMap[channelName].key);};
		size_t getLimitUser(const std::string &channelName) {return (channelsMap[channelName].userLimit);};
		size_t getNbUser(const std::string &channelName) {return (channelsMap[channelName].members.size());};
		bool getIsInvited(int fd, const std::string &channelName);
};

