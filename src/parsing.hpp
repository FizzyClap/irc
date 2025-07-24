/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: roespici <roespici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 18:03:54 by roespici          #+#    #+#             */
/*   Updated: 2025/07/24 12:15:00 by roespici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "server.hpp"
#include "client.hpp"

void	parseCommands(server &srv, client &client, int fd, const std::string &msg);
void	cmdNick(server &srv, int fd, const std::string &msg);
void	cmdJoin(server &srv, int fd, const std::string &msg);
void	cmdKick(server &srv, int kickerFd, const std::string &msg);
void	cmdPass(server &srv, int fd, const std::string &msg);
bool	errorNick(server &srv, int fd, const std::string &nickname);
bool	errorKick(server &srv, int kickerFd, int targetFd, const std::string &channelName, const std::string &targetName);
