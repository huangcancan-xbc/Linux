.PHONY:all
all:udpserver udpclient

udpserver:Main.cc
	g++ -o $@ $^ -std=c++11 -lpthread
udpclient:UDP_Client.cc
	g++ -o $@ $^ -std=c++11 -lpthread

.PHONY:clean
clean:
	rm -f udpserver udpclient