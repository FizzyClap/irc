#include "Bot.hpp"

int main()
{
	try
	{
		int sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock < 0)
			throw std::runtime_error ("Create socket error");
		sockaddr_in serverAddr;
		memset(&serverAddr, 0, sizeof(serverAddr));
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(6667);
		serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
		if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
			throw std::runtime_error ("Connexion socket error");
		Bot bot(sock);
		bot.sendRawMessage("PASS test");
		bot.sendRawMessage("NICK bot");
		bot.sendRawMessage("USER bot bot bot");
		bot.sendRawMessage("LIST");
		bot.listen();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}

}

Bot::Bot(int socketFd) : sockFd(socketFd), botClient(socketFd) {};

void Bot::sendRawMessage(const std::string &msg)
{
	std::string toSend = msg + "\r\n";
	send(this->sockFd, toSend.c_str(), toSend.length(), 0);
}

void Bot::listen()
{
	char buffer[512];
	bool list = false;
	while (true)
	{
		memset(buffer, 0, sizeof(buffer));
		int bytes = recv(this->sockFd, buffer, sizeof(buffer) - 1, 0);
		if (bytes <= 0)
			break ;
		std::string msg = cleanMessage(buffer);
		if (!msg.empty())
			std::cout << msg << std::endl;
		if (joinChannel(list, msg))
			continue ;
		this->handleCommand(msg);
	}
}

bool Bot::joinChannel(bool &list, const std::string &msg)
{
	if (!list && (msg[0] == '#' || msg[0] == '&'))
	{
		list = true;
		std::vector<std::string> channelToJoin = splitForBot(msg);
		std::string strToJoin = "JOIN ";
		for (std::vector<std::string>::iterator it = channelToJoin.begin(); it != channelToJoin.end(); ++it)
		{
			if (*it != channelToJoin.back())
				strToJoin += *it + ",";
			else
				strToJoin += *it;
		}
		this->sendRawMessage(strToJoin);
		return (true);
	}
	return (false);
}

void Bot::handleCommand(const std::string &msg)
{
	std::vector<std::string> tokens = splitForBot(msg);
	if (tokens.size() != 4 || tokens[1] != "PRIVMSG")
		return ;
	std::string target = this->getTarget(tokens);
	std::string cmd = tokens[3].erase(0, 1);
	std::string reply;
	if (cmd == "!help")
		reply = help();
	else if (cmd == "!hello")
		reply = hello(target);
	else if (cmd == "!time")
		reply = time();
	else if (cmd == "!roll")
		reply = roll();
	else
		return ;
	std::string fullMsg = "PRIVMSG " + target + " :" + reply;
	sendRawMessage(fullMsg);
}

std::vector<std::string> Bot::splitForBot(const std::string &msg)
{
	std::vector<std::string> result;
	std::string token;
	bool foundColon = false;
	for (size_t i = 0; i < msg.size();)
	{
		if (i != 0 && !foundColon && msg[i] == ':')
		{
			foundColon = true;
			token = msg.substr(i);
			result.push_back(token);
			return (result) ;
		}
		if (msg[i] == ' ')
		{
			if (!token.empty())
			{
				result.push_back(token);
				token.clear();
			}
			++i;
		}
		else
			token += msg[i++];
	}
	if (!token.empty())
		result.push_back(token);
	return (result);
}

std::string Bot::getTarget(std::vector<std::string> &tokens)
{
	std::string target = tokens[2];
	if (tokens[2] == "bot")
		target = tokens[0].erase(0, 1);
	return (target);
}

const std::string Bot::help() const
{
	return ("You can call me by using : !help, !hello, !time or !roll");
}

const std::string Bot::hello(const std::string &target) const
{
	return ("Hello, " + target);
}

const std::string Bot::time() const
{
	time_t now = std::time(NULL);
	struct tm *gmt = std::gmtime(&now);
	std::ostringstream oss;
	oss << "It's ";
	oss << gmt->tm_hour + 2 << ":" << gmt->tm_min << ":" << gmt->tm_sec << " (UTC+2) 🕒";
	return (oss.str());
}

const std::string Bot::roll() const
{
	std::srand(std::time(NULL));
	int result = std::rand() % 6 + 1;
	std::ostringstream oss;
	oss << result;
	return ("rolled a " + oss.str() + " 🎲");
}
