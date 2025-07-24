/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: roespici <roespici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 18:03:54 by roespici          #+#    #+#             */
/*   Updated: 2025/07/24 15:58:11 by roespici         ###   ########.fr       */
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
bool	errorNick(server &srv, int fd, const std::string &nickname);
bool	errorKick(server &srv, int kickerFd, int targetFd, const std::string &channelName, const std::string &targetName);
bool	errorParams(server &srv, int fd, std::vector<std::string> tokens, int min_params, int max_param);
bool	isAuthenticated(server &srv, int fd, std::string cmd);
bool	isRegistered(server &srv, int fd, std::string cmd);
void	welcomeMessage(server &srv, int fd);
