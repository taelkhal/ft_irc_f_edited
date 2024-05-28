NAME = ircserv

FLAGS = c++ #-Wall -Wextra -Werror -std=c++98 -fsanitize=address -g3

SRC =   conf/configFile.cpp  main.cpp multuplixing/multuplixing.cpp \
		multuplixing/socket.cpp parse/parse.cpp Server.cpp Client.cpp irc.cpp

all : $(NAME) $(SRC) 

$(NAME) : $(SRC)	
	$(FLAGS) $(SRC) -o $(NAME)
clean : 
		rm -fr  $(NAME)

fclean : clean
		rm -fr $(NAME) 

re : clean all