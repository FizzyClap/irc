#include "Client.hpp"

Client::Client(int fd)
{
	this->fd = fd;
	this->hostname = "";
	this->realname = "";
	this->username = "";
	this->nickname = "";
	this->authenticated = false;
	this->registered = false;
	this->userSet = false;
	this->nickSet = false;
}

void Client::setNickname(const std::string &nickname)
{
	this->nickname = nickname;
	this->nickSet = true;
}

void Client::setUsername(const std::string &username)
{
	this->username = username;
	this->userSet = true;
}

void Client::setHostname(const std::string &hostname)
{
	this->hostname = hostname;
}

void Client::setServername(const std::string &servername)
{
	this->servername = servername;
}

void Client::setRealname(const std::string &realname)
{
	this->realname = realname;
}

void Client::setAuthenticated(bool auth)
{
	this->authenticated = auth;
}

void Client::setRegistered(bool regis)
{
	this->registered = regis;
}
