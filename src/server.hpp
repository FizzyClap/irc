/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: roespici <roespici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 16:02:06 by peli              #+#    #+#             */
/*   Updated: 2025/07/17 18:09:54 by roespici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include "client.hpp"
#include "parsing.hpp"
#include <sys/socket.h>

class server
{
	private:
		std::string password;
		int port;
		int socket_fd;
		struct sockaddr_in addr;

	public:
		server();
		~server();
		server(const server &other);
		server& operator = (const server& other);

		void parsing(char *port, char *password);
		void creat_socket();
		void run();
};

