/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: roespici <roespici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 16:02:06 by peli              #+#    #+#             */
/*   Updated: 2025/07/24 16:03:26 by roespici         ###   ########.fr       */
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
	std::set<int> members;
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
		void joinChannel(int fd, const std::string &channelName);
		void kickClient(int fd, const std::string &channelName, const std::string &nickname);
		void broadcast(int senderFd, const std::string &message);
		void sendError(int fd, const std::string &code, const std::string &arg, const std::string &msg);
		bool isMemberOfChannel(int fd, const std::string &channelName);
		//SETTER
		void setClientNickname(int fd, const std::string &nickname);
		void setClientUsername(int fd, const std::string &username);
		void setClientHostname(int fd, const std::string &hostname);
		void setClientServername(int fd, const std::string &servername);
		void setClientRealname(int fd, const std::string &realname);
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
};

