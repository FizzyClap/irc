/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PollManager.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: roespici <roespici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 19:04:45 by peli              #+#    #+#             */
/*   Updated: 2025/07/29 12:20:09 by roespici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PollManager.hpp"

void PollManager::addClient(int clientFd)
{
	pollfd pollfds;
	pollfds.fd = clientFd;
	pollfds.events = POLLIN;
	pollfds.revents = 0;
	Pollfds.push_back(pollfds);
};
