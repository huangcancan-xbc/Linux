ticket_purchase2: ticket_purchase2.cc
	g++ -o $@ $^ -std=c++11 -lpthread
.PHONY: clean
clean:
	rm -f ticket_purchase2