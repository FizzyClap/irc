/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: roespici <roespici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 17:06:02 by roespici          #+#    #+#             */
/*   Updated: 2025/07/31 19:03:53 by roespici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(const std::string &channelName)
{
	_name = channelName;
	_topic = "";
	_inviteOnly = false;
	_topicRestricted = false;
	_keyEnabled = false;
	_userLimitEnabled = false;
	_key = "";
	_userLimit = 0;
}

void Channel::addOperator(int fd)
{
	_operators.insert(fd);
}

void Channel::removeOperator(int fd)
{
	_operators.erase(fd);
}

void Channel::addMembers(int  fd)
{
	_members.insert(fd);
}

void Channel::removeMembers(int fd)
{
	_members.erase(fd);
}

bool Channel::getIsInvited(int fd) const
{
	if (_invited.find(fd) != _invited.end())
		return (true);
	return (false);
}

void Channel::addInvited(int fd)
{
	_invited.insert(fd);
}

void Channel::removeInvited(int fd)
{
	_invited.erase(fd);
}


void Channel::setTopic(const std::string &topic)
{
	_topic = topic;
}

void Channel::setInviteMode(const bool mode)
{
	_inviteOnly = mode;
}

void Channel::setTopicMode(const bool mode)
{
	_topicRestricted = mode;
}

void Channel::setKeyMode(const bool mode, const std::string &key)
{
	_keyEnabled = mode;
	_key = key;
}

bool Channel::setLimitMode(const bool mode, const std::string &limit)
{
	int newLimit = atoi(limit);
	if (newLimit == 0)
		return (false);
	_userLimitEnabled = mode;
	_userLimit = newLimit;
	return (true);
}

