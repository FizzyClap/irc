/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: roespici <roespici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 18:03:54 by roespici          #+#    #+#             */
/*   Updated: 2025/07/23 14:35:00 by roespici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "server.hpp"
#include "client.hpp"

void	parseCommands(server &srv, client &client, int fd, const std::string &msg);
void	cmdNick(server &srv, int fd, const std::string &msg);
void	cmdJoin(server &srv, int fd, const std::string &msg);
void	cmdKick(server &srv, const std::string &msg);
