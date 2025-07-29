/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: roespici <roespici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 14:22:33 by peli              #+#    #+#             */
/*   Updated: 2025/07/29 12:32:57 by roespici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "CommandHandler.hpp"

int main(int argc, char **argv)
{
	try
	{
		if (argc != 3)
			throw std::runtime_error("Error");
		Server srv;
		srv.parsing(argv[1], argv[2]);
		srv.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}
