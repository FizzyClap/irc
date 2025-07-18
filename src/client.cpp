/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: roespici <roespici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 19:04:45 by peli              #+#    #+#             */
/*   Updated: 2025/07/18 12:00:46 by roespici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"

client::client(/* args */)
{
}

client::~client()
{
}

void client::add_client(int client_fd)
{
    Client_list.push_back(client_fd);
    pollfd pollfds;
    pollfds.fd = client_fd;
    pollfds.events = POLLIN;
    pollfds.revents = 0;
    Pollfds.push_back(pollfds);
};

std::vector<pollfd>& client::get_pollfds()
{
    return (Pollfds);
}

void	client::setNickname(int fd, const std::string &nickname)
{
	std::map<int, ClientInfos>::iterator it = clients.find(fd);
	if (it != clients.end())
		it->second.nickname = nickname;
	else
	{
		ClientInfos	infos;
		infos.fd = fd;
		infos.nickname = nickname;
		clients[fd] = infos;
	}
}

std::string	client::getNickname(int fd)
{
	std::map<int, ClientInfos>::iterator it = clients.find(fd);
	if (it != clients.end())
		return (it->second.nickname);
	return ("");
}

void	client::broadcast(int senderFd, const std::string &message)
{
	std::map<int, ClientInfos>::iterator it;
	for (it = clients.begin(); it != clients.end(); ++it)
	{
		int fd = it->first;
		if (fd != senderFd)
			send(fd, message.c_str(), message.length(), 0);
	}
}

void	client::joinChannel(int fd, const std::string &channelName)
{
	std::map<std::string, Channel>::iterator it = channels.find(channelName);
	if (it != channels.end())
		it->second.name = channelName;
	else
	{
		Channel	newChannel;
		newChannel.name = channelName;
		newChannel.members.push_back(fd);
		channels[channelName] = newChannel;
	}
}
