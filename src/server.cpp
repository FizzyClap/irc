/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: roespici <roespici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 16:03:33 by peli              #+#    #+#             */
/*   Updated: 2025/07/24 12:17:32 by roespici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#include "parsing.hpp"

void	cleanMessage(std::string &message);

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
    creat_socket();
};

void server::creat_socket()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
        throw std::runtime_error ("Creat socket error");

    int yes = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
        throw std::runtime_error("setsockopt(SO_REUSEADDR) failed");

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
    client_.add_client(socket_fd);
    while (true)
    {
        std::vector<pollfd>& poll_fds = client_.get_pollfds();
        int activity = poll(poll_fds.data(), poll_fds.size(), 100);// .data() → donne un pollfd*, c’est ce que poll() attend.
        if (activity < 0)
        {
            std::cerr << "poll fail: " << strerror(errno) << std::endl;
            continue;
        }
        for (size_t i = 0; i < poll_fds.size(); ++i)
        {
            if (poll_fds[i].fd == socket_fd && (poll_fds[i].revents & POLLIN))
            {
                    //connect;
                    struct sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    int new_client = accept(socket_fd, (struct sockaddr*)&client_addr, &client_len);
                    if (new_client < 0)
                    {
                        std::cerr << "Client connect fail: " << strerror(errno) << std::endl;
                        continue;
                    }
                    fcntl(new_client, F_SETFL, O_NONBLOCK);
                    std::cout << "Client connected: fd = " << new_client << std::endl;
                    client_.add_client(new_client);
					ClientInfos infos;
					infos.fd = new_client;
					infos.nickname = "";
					infos.authenticated = false;
					clientsMap[new_client] = infos;
                    continue;
            }
            if (poll_fds[i].revents & POLLIN)
            {
                char buffer[1024];
                ssize_t j = recv(poll_fds[i].fd, buffer, sizeof(buffer), 0);
                if (j > 0)
                {
					buffer[j] = '\0';
					std::string msg(buffer);
					cleanMessage(msg);
					parseCommands(*this, client_, poll_fds[i].fd, msg);
                }
                else
                {
                    close(poll_fds[i].fd);
                    std::cerr << "Client " << poll_fds[i].fd << " disconnected." << std::endl;
                    poll_fds.erase(poll_fds.begin() + i);
                    --i;
                }
            }
        }
    }
};

void    send_to_client(int fd, const std::string& message)
{
    ssize_t sent = send(fd, message.c_str(), message.length(), 0);

    if (sent < 0) {
        std::cerr << "send error to client " << fd << ": " << strerror(errno) << std::endl;
    }
};

void	cleanMessage(std::string &message)
{
	message.erase(message.find_last_not_of(" \r\n") + 1);
}

void server::joinChannel(int fd, const std::string &channelName)
{
	channelsMap[channelName].name = channelName;
	channelsMap[channelName].members.insert(fd);
}

void server::setClientNickname(int fd, const std::string &nickname)
{
	clientsMap[fd].nickname = nickname;
}

std::string server::getClientNickname(int fd)
{
	return (clientsMap[fd].nickname);
}

int server::getClientFd(const std::string &nickname)
{
	for (std::map<int, ClientInfos>::iterator it = clientsMap.begin(); it != clientsMap.end(); ++it)
	{
		if (it->second.nickname == nickname)
			return (it->first);
	}
	return (-1);
}

void server::broadcast(int senderFd, const std::string &message)
{
	for (std::map<int, ClientInfos>::iterator it = clientsMap.begin(); it != clientsMap.end(); ++it)
	{
		int fd = it->first;
		if (fd != senderFd)
			send(fd, message.c_str(), message.length(), 0);
	}
}

void server::kickClient(server &srv, int fd, const std::string &channelName, const std::string &nickname)
{
	std::map<std::string, Channel>::iterator it = channelsMap.find(channelName);
	if (it != channelsMap.end())
	{
		it->second.members.erase(fd);
		std::string kickMsg = nickname + " has been kicked from " + channelName + ".\n";
		srv.broadcast(fd, kickMsg);
	}
}

void server::sendError(int fd, const std::string &code, const std::string &arg, const std::string &msg)
{
	std::ostringstream oss;
	oss << ":ircserv " << code << " * " << arg << " :" << msg << "\r\n";
	send(fd, oss.str().c_str(), oss.str().size(), 0);
}

bool server::isMemberOfChannel(int fd, const std::string &channelName)
{
	std::map<std::string, Channel>::iterator it = channelsMap.find(channelName);
	if (it == channelsMap.end())
		return (false);
	return (it->second.members.find(fd) != it->second.members.end());
}
