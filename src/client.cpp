/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: peli <peli@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 19:04:45 by peli              #+#    #+#             */
/*   Updated: 2025/07/13 20:19:11 by peli             ###   ########.fr       */
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