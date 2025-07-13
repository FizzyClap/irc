/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: peli <peli@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 16:03:33 by peli              #+#    #+#             */
/*   Updated: 2025/07/13 21:33:16 by peli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

server::server()
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
    creat_socket();
};

void server::creat_socket()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
        throw std::runtime_error ("Creat socket error");
    socket_fd = server_fd;
    fcntl(socket_fd, F_SETFL, O_NONBLOCK);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port); // pourquoi faut convertir?
    addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(addr.sin_zero), 0, 8);
    if (bind(server_fd, (const sockaddr*)&addr, (socklen_t)sizeof(addr)) != 0)
        throw std::runtime_error("bind() fail");
    if (listen(server_fd, SOMAXCONN) == -1)// nombre maximum de connexions entrantes mises en attente
        throw std::runtime_error("listen error");
};

void server::run()
{
    client client_;
    pollfd server_pollfd;
    server_pollfd.fd = socket_fd;
    server_pollfd.events = POLLIN;
    server_pollfd.revents = 0;

    client_.add_client(socket_fd);
    while (true)
    {
        std::vector<pollfd>& poll_fds = client_.get_pollfds();
        int activity = poll(poll_fds.data(), poll_fds.size(), 0);// .data() → donne un pollfd*, c’est ce que poll() attend.
        if (activity < 0)
        {
            std::cerr << "poll fail: " << strerror(errno) << std::endl;
            continue;
        }
        for (size_t i = 0; i < poll_fds.size(); i++)
        {                
            if (poll_fds[i].fd == socket_fd)
            {
                if (POLLIN & poll_fds[i].revents)
                {
                    //connect;
                    struct sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    int new_client = accept(socket_fd, (struct sockaddr*)&client_addr, &client_len);
                    if (new_client < 0)
                    {
                        std::cerr << "client connect fail: " << strerror(errno) << std::endl;
                        continue; 
                    }
                    client_.add_client(new_client);
                }
            }
            else
            {
                char buffer[1024];
                ssize_t j =recv(poll_fds[i].fd, buffer, sizeof(buffer), 0);
                if (j > 0)
                {
                    //you process the message (for example, check if it's a command like /nick, /join, etc);
                }
                else
                {
                    close(poll_fds[i].fd);
                    poll_fds.erase(poll_fds.begin() + i);
                    --i;
                }
            }
        }
    }
};