/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: roespici <roespici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 18:03:54 by roespici          #+#    #+#             */
/*   Updated: 2025/07/29 12:28:48 by roespici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Server.hpp"
#include "Utils.hpp"

class Server;

typedef void (*CommandHandler)(Server&, int, const std::vector<std::string>&);
void	parseCommands(Server &srv, int fd, const std::string &msg);
void	cmdPass(Server &srv, int fd, const std::vector<std::string> &tokens);
void	cmdNick(Server &srv, int fd, const std::vector<std::string> &tokens);
void	cmdUser(Server &srv, int fd, const std::vector<std::string> &tokens);
void	cmdJoin(Server &srv, int fd, const std::vector<std::string> &tokens);
void	cmdKick(Server &srv, int kickerFd, const std::vector<std::string> &tokens);
void	cmdPrivMsg(Server &srv, int fd, const std::vector<std::string> &tokens);
void	cmdInvite(Server &srv, int senderFd, const std::vector<std::string> &tokens);
void	cmdTopic(Server &srv, int fd, const std::vector<std::string> &tokens);
void	cmdMode(Server &srv, int fd, const std::vector<std::string> &tokens);
void	cmdList(Server &srv, int fd, const std::vector<std::string> &tokens);
bool	errorPass(Server &srv, int fd, const std::vector<std::string> tokens);
bool	errorUser(Server &srv, int fd, const std::vector<std::string> tokens);
bool	errorNick(Server &srv, int fd, const std::vector<std::string> tokens);
bool	errorJoin(Server &srv, int fd, const std::vector<std::string> tokens);
bool	errorKick(Server &srv, int kickerFd, const std::vector<std::string> tokens);
bool	errorParams(Server &srv, int fd, std::vector<std::string> tokens, size_t min_params, size_t max_param);
bool	errorPrivMsg(Server &srv, int fd, const std::vector<std::string> tokens);
bool	errorInvite(Server &srv, int senderFd, const std::vector<std::string> tokens);
bool	errorTopic(Server &srv, int fd, const std::vector<std::string> tokens);
bool	errorMode(Server &srv, int fd, const std::vector<std::string> tokens);
bool	isAuthenticated(Server &srv, int fd, std::string cmd);
bool	isRegistered(Server &srv, int fd, std::string cmd);
bool	fillArg(Server &srv, int fd, const std::vector<std::string> tokens, std::string &arg, int &params);
