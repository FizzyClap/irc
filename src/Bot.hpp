#include "Server.hpp"
#include "Utils.hpp"
#include <arpa/inet.h>
#include <cstdlib>
#include <ctime>
#include <sstream>

class Bot
{
	private:
		int _sockFd;
		std::string _password;
	public:
		Bot(int socketFd);
		~Bot() {};
		void sendRawMessage(const std::string &msg);
		void listen();
		bool joinChannel(bool &list, const std::string &msg);
		void handleCommand(const std::string &msg);
		std::vector<std::string> splitForBot(const std::string &msg);
		std::string getTarget(std::vector<std::string> &tokens);
		const std::string help() const;
		const std::string hello(const std::string &target) const;
		const std::string time() const;
		const std::string roll() const;
		const std::string &getPassword() const {return (_password);};
};
