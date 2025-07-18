/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: roespici <roespici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 18:06:15 by peli              #+#    #+#             */
/*   Updated: 2025/07/17 18:06:18 by roespici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include <map>
#include <poll.h>
#include <string>
#include <iostream>
#include <sys/socket.h>

struct ClientInfos
{
	int	fd;
	std::string	nickname;
};
//A voir pour remplacer client list

class client
{
	private:
		std::vector<int> Client_list;
		std::vector<pollfd> Pollfds;
		std::map<int, ClientInfos> clients;
	public:
		client(/* args */);
		~client();
		void add_client(int client_fd);
		std::vector<pollfd>& get_pollfds();
		void setNickname(int fd, const std::string &nickname);
		std::string getNickname(int fd);
		void broadcast(int senderFd, const std::string &messsage);
};

