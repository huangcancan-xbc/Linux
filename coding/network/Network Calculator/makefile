.PHONY:all
all:tcp_server_calculator tcp_client_calculator

tcp_server_calculator:TCP_Server_Cal.cc Log.cpp Socket.hpp TCP_Server.hpp
	g++ -o $@ $^ -std=c++11 -lpthread
	
tcp_client_calculator:TCP_Client_Cal.cc Log.cpp Socket.hpp
	g++ -o $@ $^ -std=c++11 -lpthread

.PHONY:clean
clean:
	rm -f tcp_server_calculator tcp_client_calculator