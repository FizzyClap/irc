#include "Server.hpp"
#include "Utils.hpp"
#include <arpa/inet.h>
#include <cstdlib>
#include <ctime>
#include <sstream>

class Bot
{
	private:
		int sockFd;
		Client botClient;
	public:
		Bot(int socketFd);
		~Bot() {};
		Client &getBot() {return (this->botClient);};
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
};
