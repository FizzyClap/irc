/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: roespici <roespici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 18:03:54 by roespici          #+#    #+#             */
/*   Updated: 2025/07/17 18:20:09 by roespici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "client.hpp"

void	parseCommands(client client, int fd, const std::string &msg);
void	cmdNick(client client, int fd, const std::string &msg);
void	cmdJoin(client client, int fd, const std::string &msg);
