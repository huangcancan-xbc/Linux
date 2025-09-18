.PHONY:all
all:HTTP_Server

HTTP_Server:HTTP_Server.cc Log.cpp HTTP_Server.hpp
	g++ -o $@ $^ -std=c++11 -lpthread
	
.PHONY:clean
clean:
	rm -f HTTP_Server