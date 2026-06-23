/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ModeCommand.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rorollin <rorollin@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 15:35:03 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/23 16:17:23 by rorollin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef MODECOMMAND_HPP
# define MODECOMMAND_HPP
# include "ACommand.hpp"

class Channel;

typedef struct ModeChange
{
	bool sign;         // true = '+', false = '-'
	char mode;         // 'i' 't' 'k' 'o' 'l'
	std::string arg;   // empty when the mode takes no argument

} ModeChange;
class ModeCommand : public ACommand {
  public:
	ModeCommand(Server &server);
	~ModeCommand();

	void execute(Client &client, Message &msg);
	size_t minParams() const;
private:
	std::vector<ModeChange> parseModeChange(Message &msg);
	// Applies one change to the channel; returns true if state actually changed.
	// Throws UserNotInChannel (441) for a '+o'/'-o' on an unknown/non-member.
	bool applyOne(Channel &ch, const ModeChange &change);
	// "+itk <key> <limit>" snapshot of the channel's current modes (for 324).
	std::string currentModeString(Channel &ch);
};

#endif
