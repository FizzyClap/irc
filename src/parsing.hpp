/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: roespici <roespici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 18:03:54 by roespici          #+#    #+#             */
/*   Updated: 2025/07/28 16:12:20 by roespici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "server.hpp"
#include "client.hpp"

void	parseCommands(server &srv, client &client, int fd, const std::string &msg);
void	cmdPass(server &srv, int fd, const std::vector<std::string> &tokens);
void	cmdNick(server &srv, int fd, const std::vector<std::string> &tokens);
void	cmdUser(server &srv, int fd, const std::vector<std::string> &tokens);
void	cmdJoin(server &srv, int fd, const std::vector<std::string> &tokens);
void	cmdKick(server &srv, int kickerFd, const std::vector<std::string> &tokens);
void	cmdPrivMsg(server &srv, int fd, const std::vector<std::string> &tokens);
void	cmdInvite(server &srv, int senderFd, const std::vector<std::string> &tokens);
void	cmdTopic(server &srv, int fd, const std::vector<std::string> &tokens);
void	cmdMode(server &srv, int fd, const std::vector<std::string> &tokens);
bool	errorUser(server &srv, int fd, const std::vector<std::string> tokens);
bool	errorNick(server &srv, int fd, const std::vector<std::string> tokens);
bool	errorJoin(server &srv, int fd, const std::vector<std::string> tokens);
bool	errorKick(server &srv, int kickerFd, const std::vector<std::string> tokens);
bool	errorParams(server &srv, int fd, std::vector<std::string> tokens, size_t min_params, size_t max_param);
bool	errorPrivMsg(server &srv, int fd, const std::vector<std::string> tokens);
bool	errorInvite(server &srv, int senderFd, const std::vector<std::string> tokens);
bool	errorTopic(server &srv, int fd, const std::vector<std::string> tokens);
bool	errorMode(server &srv, int fd, const std::vector<std::string> tokens);
bool	isAuthenticated(server &srv, int fd, std::string cmd);
bool	isRegistered(server &srv, int fd, std::string cmd);
void	welcomeMessage(server &srv, int fd);
std::string eraseColon(std::vector<std::string> tokens, size_t size);
int atoi(const std::string &str);
bool fillArg(server &srv, int fd, const std::vector<std::string> tokens, std::string &arg, int &params);
