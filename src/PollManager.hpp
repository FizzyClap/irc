/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PollManager.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: roespici <roespici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 18:06:15 by peli              #+#    #+#             */
/*   Updated: 2025/07/31 09:35:43 by roespici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include <map>
#include <poll.h>
#include <string>
#include <iostream>
#include <sys/socket.h>

class PollManager
{
	private:
		std::vector<pollfd> _Pollfds;
	public:
		PollManager() {};
		~PollManager() {};
		void addClient(int clientFd);
		std::vector<pollfd> &getPollFds() {return (_Pollfds);};
};
