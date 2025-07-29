/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: roespici <roespici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 17:06:02 by roespici          #+#    #+#             */
/*   Updated: 2025/07/29 12:07:57 by roespici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(const std::string &channelName)
{
	this->name = channelName;
	this->topic = "";
	this->inviteOnly = false;
	this->topicRestricted = false;
	this->keyEnabled = false;
	this->userLimitEnabled = false;
	this->key = "";
	this->userLimit = 0;
}

void Channel::addOperator(int fd)
{
	this->operators.insert(fd);
}

void Channel::removeOperator(int fd)
{
	this->operators.erase(fd);
}

void Channel::addMembers(int fd)
{
	this->members.insert(fd);
}

void Channel::removeMembers(int fd)
{
	this->members.erase(fd);
}

bool Channel::getIsInvited(int fd) const
{
	if (this->invited.find(fd) != this->invited.end())
		return (true);
	return (false);
}

void Channel::addInvited(int fd)
{
	this->invited.insert(fd);
}

void Channel::setTopic(const std::string &topic)
{
	this->topic = topic;
}

void Channel::setInviteMode(const bool mode)
{
	this->inviteOnly = mode;
}

void Channel::setTopicMode(const bool mode)
{
	this->topicRestricted = mode;
}

void Channel::setKeyMode(const bool mode, const std::string &key)
{
	this->keyEnabled = mode;
	this->key = key;
}

int atoi(const std::string &str)
{
	std::istringstream iss(str);
	int result;
	iss >> result;
	return (result);
}

bool Channel::setLimitMode(const bool mode, const std::string &limit)
{
	int newLimit = atoi(limit);
	if (newLimit == 0)
		return (false);
	this->userLimitEnabled = mode;
	this->userLimit = newLimit;
	return (true);
}

