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
		int _fd;
		std::string _nickname;
		std::string _username;
		std::string _hostname;
		std::string _servername;
		std::string _realname;
		bool _authenticated;
		bool _registered;
		bool _userSet;
		bool _nickSet;

	public:
		Client() {};
		Client(int fd);
		~Client() {};
		int getFd() const {return (_fd);};
		const std::string &getNickname() const {return (_nickname);};
		const std::string &getUsername() const {return (_username);};
		const std::string &getHostname() const {return (_hostname);};
		const std::string &getServername() const {return (_servername);};
		const std::string &getRealname() const {return (_realname);};
		bool isAuthenticated() const {return (_authenticated);};
		bool isRegistered() const {return (_registered);};
		bool isUserSet() const {return (_userSet);};
		bool isNickSet() const {return (_nickSet);};
		void setNickname(const std::string &nickname);
		void setUsername(const std::string &username);
		void setHostname(const std::string &hostname);
		void setServername(const std::string &servername);
		void setRealname(const std::string &realname);
		void setAuthenticated(bool auth);
		void setRegistered(bool regis);
};
