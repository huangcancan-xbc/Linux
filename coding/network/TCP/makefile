.PHONY:all
all:tcpserver tcpclient

tcpserver:Main.cc
	g++ -o $@ $^ -std=c++11 -lpthread
tcpclient:TCP_Client.cc
	g++ -o $@ $^ -std=c++11 -lpthread

.PHONY:clean
clean:
	rm -f tcpserver tcpclient