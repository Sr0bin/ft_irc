# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rorollin <rorollin@student.42lyon.fr>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/06/16 14:21:38 by rorollin          #+#    #+#              #
#    Updated: 2026/06/16 14:26:00 by rorollin         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

#SERVER########################

SERVER = server

SERVER_PATH = $(SERVER)/$(NAME)

#COMPILER#####################


all: make_server $(NAME)

$(NAME): make_server
	cp $(SERVER_PATH) .
	@echo "$(NAME) built succesfully."

make_server:
	$(MAKE) -C $(SERVER)
	
clean:
	@$(MAKE) -s -C $(SERVER) clean
	@echo "Cleaned !"

fclean:
	@$(MAKE) -C $(SERVER) fclean
	@rm -f $(NAME)
	@echo "Fcleaned !"

re: fclean all

.PHONY: all clean fclean re make_server
