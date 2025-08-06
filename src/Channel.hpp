/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: roespici <roespici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 17:04:28 by roespici          #+#    #+#             */
/*   Updated: 2025/08/06 16:22:27 by roespici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <algorithm>
#include <poll.h>
#include <sys/socket.h>
#include <set>
#include <list>
#include <sstream>
#include "Utils.hpp"

class Channel
{
	private:
		std::string	_name;
		std::string	_topic;
		std::list<int> _membersInOrder;
		std::set<int> _members;
		std::set<int> _operators;
		std::set<int> _invited;
		bool _inviteOnly;
		bool _topicRestricted;
		bool _keyEnabled;
		std::string _key;
		bool _userLimitEnabled;
		int _userLimit;

	public:
		Channel() {};
		Channel(const std::string &channelName);
		~Channel() {};
		const std::string &getTopic() const {return (_topic);};
		bool getModeInvite() const {return (_inviteOnly);};
		bool getModeTopic() const {return (_topicRestricted);};
		bool getModeKey() const {return (_keyEnabled);};
		bool getModeLimit() const {return (_userLimitEnabled);};
		const std::string getKeyPass() const {return (_key);};
		size_t getLimitUser() const {return (_userLimit);};
		size_t getNbUser() const {return (_members.size());};
		const std::string getChannelName() const {return (_name);};
		std::set<int> getOperators() const {return (_operators);};
		std::list<int> getMembersOrder() const {return (_membersInOrder);};
		bool getIsInvited(int fd) const;
		void addOperator(int fd);
		void addOperatorBySeniority();
		void removeOperator(int fd);
		bool isOperator(int fd) const {return (_operators.find(fd) != _operators.end());};
		void addMembers(int fd);
		void removeMembers(int fd);
		bool isMember(int fd) const {return (_members.find(fd) != _members.end());};
		void addInvited(int fd);
		void removeInvited(int fd);
		void setTopic(const std::string &topic);
		void setInviteMode(const bool mode);
		void setTopicMode(const bool mode);
		void setKeyMode(const bool mode, const std::string &key);
		bool setLimitMode(const bool mode, const std::string &limit);
};
