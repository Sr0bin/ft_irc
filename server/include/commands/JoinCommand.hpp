#ifndef JOINCOMMAND_HPP
#define JOINCOMMAND_HPP
#include "ACommand.hpp"
#include "ACommandReply.hpp"

class JoinCommand : public ACommand {
  public:
	JoinCommand(Server &server);
	~JoinCommand();

	void execute(Client &client, Message &msg);
	size_t minParams(void) const;

  private:
	void sendTopic(Client &client, Channel *ch, const std::string &name,
				   const std::string &serverName);
	void sendNames(Client &client, Channel *ch, const std::string &name,
				   const std::string &serverName);
};

#endif
