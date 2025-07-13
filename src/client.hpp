/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: peli <peli@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 18:06:15 by peli              #+#    #+#             */
/*   Updated: 2025/07/13 21:38:33 by peli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include <poll.h>

class client
{
private:
    std::vector<int> Client_list;
    std::vector<pollfd> Pollfds;
public:
    client(/* args */);
    ~client();
    void add_client(int client_fd);
    std::vector<pollfd>& get_pollfds();
};

