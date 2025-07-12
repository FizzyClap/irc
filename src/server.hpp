/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: peli <peli@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 16:02:06 by peli              #+#    #+#             */
/*   Updated: 2025/07/12 18:23:25 by peli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <cstring>
#include <cstdlib>

class server
{
private:
    std::string password;
    int port;
public:
    server(/* args */);
    ~server();
    server(const server &other);
    server& operator = (const server& other);

    void parsing(char *port, char *password);
    void creat_socket();
};

