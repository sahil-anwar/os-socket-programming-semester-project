CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -pthread

all: bin/broadcast-server bin/broadcast-client bin/distributed-server bin/distributed-client

bin/%-server: src/%/server.cpp
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $< -o $@

bin/%-client: src/%/client.cpp
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f bin/broadcast-server bin/broadcast-client bin/distributed-server bin/distributed-client

.PHONY: all clean
