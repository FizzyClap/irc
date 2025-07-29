/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: roespici <roespici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 17:04:28 by roespici          #+#    #+#             */
/*   Updated: 2025/07/29 11:02:25 by roespici         ###   ########.fr       */
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
#include <sstream>

class Channel
{
	private:
		std::string	name;
		std::string	topic;
		std::set<int> members;
		std::set<int> operators;
		std::set<int> invited;
		bool inviteOnly;
		bool topicRestricted;
		bool keyEnabled;
		std::string key;
		bool userLimitEnabled;
		int userLimit;

	public:
		Channel() {};
		Channel(const std::string &channelName);
		~Channel() {};
		const std::string &getTopic() const {return (this->topic);};
		bool getModeInvite() const {return (this->inviteOnly);};
		bool getModeTopic() const {return (this->topicRestricted);};
		bool getModeKey() const {return (this->keyEnabled);};
		bool getModeLimit() const {return (this->userLimitEnabled);};
		const std::string getKeyPass() const {return (this->key);};
		size_t getLimitUser() const {return (this->userLimit);};
		size_t getNbUser() const {return (this->members.size());};
		const std::string getChannelName() const {return (this->name);};
		bool getIsInvited(int fd) const;
		void addOperator(int fd);
		void removeOperator(int fd);
		bool isOperator(int fd) const {return (this->operators.find(fd) != this->operators.end());};
		void addMembers(int fd);
		void removeMembers(int fd);
		bool isMember(int fd) const {return (this->members.find(fd) != this->members.end());};
		void addInvited(int fd);
		void setTopic(const std::string &topic);
		void setInviteMode(const bool mode);
		void setTopicMode(const bool mode);
		void setKeyMode(const bool mode, const std::string &key);
		bool setLimitMode(const bool mode, const std::string &limit);
};
