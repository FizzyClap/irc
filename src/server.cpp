/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: peli <peli@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 16:03:33 by peli              #+#    #+#             */
/*   Updated: 2025/07/12 18:24:54 by peli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

server::server(/* args */)
{
}

server::~server()
{
}

server::server(const server &other)
{
    (void)other;
};

server& server::operator = (const server& other)
{
    if (this != &other)
    {
    }
    return (*this);
};

void server::parsing(char *argv1, char *argv2)
{
    if (!argv1)
        throw std::runtime_error("port doesn't exist");
    for (size_t i = 0; i < strlen(argv1); i++)
    {
        if (!isdigit(static_cast<unsigned char>(argv1[i])))
            throw std::runtime_error("Port is not numeric");
    }
    port = atoi(argv1);
    if (port > 65535 || port < 1024)
        throw std::runtime_error("Port no utilisable");
    if (argv2 != NULL)
        password = argv2;
    std::cout << "port is :" << port << std::endl;
    std::cout << "password is :" << password << std::endl;
};

void server::creat_socket()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
        throw std::runtime_error ("Creat socket error");
}