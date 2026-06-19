# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rorollin <rorollin@student.42lyon.fr>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/01/19 15:44:49 by rorollin          #+#    #+#              #
#    Updated: 2026/06/20 16:20:55 by rorollin         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

#SOURCES######################

SOURCES_DIR = src

SOURCES_NAME = main.cpp \
			   Server.cpp \
			   Client.cpp \
			   Channel.cpp \
			   Message.cpp \
			   Parser.cpp \
			   ACommand.cpp \
			   CommandDispatcher.cpp \
			   AMultiplexer.cpp \
			   PollMultiplexer.cpp \
			   IrcException.cpp \
			   FatalException.cpp \
			   ACommandError.cpp \
			   randomCmd.cpp     #FOR TESTING PURPOSE ONLY

#---------------------------

SOURCES = $(addprefix $(SOURCES_DIR)/,\
		  $(SOURCES_NAME)\
		  )
 
#OBJECTS#######################

OBJ_DIR = .obj

OBJECTS = $(SOURCES:%.cpp=$(OBJ_DIR)/%.o)

#DEPS##########################

DEPS = $(SOURCES:%.cpp=$(OBJ_DIR)/%.d)

#INCLUDES#######################

HEADERS_DIR = include/

INCLUDES = $(addprefix -I , $(HEADERS_DIR)) 

# INCLUDES = 

#COMPILER#####################

COMPILER = c++

CFLAGS_PROD = -Wall -Wextra -Werror -MMD -MP -std=c++98 -ggdb -fno-eliminate-unused-debug-types

CFLAGS = $(CFLAGS_PROD)

export CFLAGS

all: $(NAME)
	
$(NAME): $(OBJECTS)
	$(COMPILER) $(CFLAGS) $(INCLUDES) $^ -o $@
	@echo "$(NAME) built succesfully."

$(OBJ_DIR)/%.o: %.cpp 
	mkdir -p $(dir $@)
	$(COMPILER) $(CFLAGS) $(INCLUDES) -c $< -o $@

-include $(DEPS)

clean:
	@rm -rf $(OBJ_DIR)
	@rm -rf .cache/
	@rm -f .bonus
	@echo "Cleaned !"

fclean:
	@$(MAKE) clean
	@rm -f $(NAME)
	@echo "Fcleaned !"

re:	
	$(MAKE) fclean 
	$(MAKE) all

.PHONY: all clean fclean re
