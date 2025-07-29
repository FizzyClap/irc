#pragma once
#include <vector>
#include <string>
#include <map>
#include <poll.h>
#include <string>
#include <iostream>
#include <sys/socket.h>

class Client
{
	private:
		int fd;
		std::string nickname;
		std::string username;
		std::string hostname;
		std::string servername;
		std::string realname;
		bool authenticated;
		bool registered;
		bool userSet;
		bool nickSet;

	public:
		Client() {};
		Client(int fd);
		~Client() {};
		int getFd() const {return (this->fd);};
		const std::string &getNickname() const {return (this->nickname);};
		const std::string &getUsername() const {return (this->username);};
		const std::string &getHostname() const {return (this->hostname);};
		const std::string &getServername() const {return (this->servername);};
		const std::string &getRealname() const {return (this->realname);};
		bool isAuthenticated() const {return (this->authenticated);};
		bool isRegistered() const {return (this->registered);};
		bool isUserSet() const {return (this->userSet);};
		bool isNickSet() const {return (this->nickSet);};
		void setNickname(const std::string &nickname);
		void setUsername(const std::string &username);
		void setHostname(const std::string &hostname);
		void setServername(const std::string &servername);
		void setRealname(const std::string &realname);
		void setAuthenticated(bool auth);
		void setRegistered(bool regis);
};
