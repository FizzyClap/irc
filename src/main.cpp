/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: roespici <roespici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 14:22:33 by peli              #+#    #+#             */
/*   Updated: 2025/07/21 14:54:10 by roespici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#include "client.hpp"

int main(int argc, char **argv)
{
    try
    {
        if (argc != 3)
            throw std::runtime_error("Error");

        server  server;
        server.parsing(argv[1], argv[2]);
		server.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
}
