.PHONY:all
all:HTTP_Server_image

HTTP_Server_image:HTTP_Server_image.cc Log.cpp HTTP_Server.hpp
	g++ -o $@ $^ -std=c++11 -lpthread
	
.PHONY:clean
clean:
	rm -f HTTP_Server_image