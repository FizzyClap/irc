/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: roespici <roespici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 16:02:06 by peli              #+#    #+#             */
/*   Updated: 2025/07/23 14:49:04 by roespici         ###   ########.fr       */
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
#include "client.hpp"
#include <set>

struct Channel
{
	std::string	name;
	std::set<int> members;
};

struct ClientInfos
{
	int fd;
	std::string nickname;
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
		void setClientNickname(int fd, const std::string &nickname);
		std::string getClientNickname(int fd);
		int getClientFd(const std::string &nickname);
		void joinChannel(int fd, const std::string &channelName);
		void kickClient(server &srv, int fd, const std::string &channelName, const std::string &nickname);
		void broadcast(int senderFd, const std::string &message);
};

