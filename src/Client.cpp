#include "Client.hpp"

Client::Client(int fd)
{
	_fd = fd;
	_hostname = "";
	_realname = "";
	_username = "";
	_nickname = "";
	_authenticated = false;
	_registered = false;
	_userSet = false;
	_nickSet = false;
}

void Client::setNickname(const std::string &nickname)
{
	_nickname = nickname;
	_nickSet = true;
}

void Client::setUsername(const std::string &username)
{
	_username = username;
	_userSet = true;
}

void Client::setHostname(const std::string &hostname)
{
	_hostname = hostname;
}

void Client::setServername(const std::string &servername)
{
	_servername = servername;
}

void Client::setRealname(const std::string &realname)
{
	_realname = realname;
}

void Client::setAuthenticated(bool auth)
{
	_authenticated = auth;
}

void Client::setRegistered(bool regis)
{
	_registered = regis;
}
