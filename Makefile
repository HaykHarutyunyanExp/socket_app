CXX=gcc -pthread
CXXFLAGS=-Wall -Wextra -Werror -std=c99
INCLUDEFLAG=-I.

SERVER=server
CLIENT=client

release: $(SERVER)/Server $(CLIENT)/Client

$(SERVER)/Server: $(SERVER)/server.c
	$(CXX) $(CXXFLAGS) $^ -o $@

$(CLIENT)/Client: $(CLIENT)/client.c
	$(CXX) $(CXXFLAGS) $^ -o $@

$(SERVER)/%.c: $(SERVER)/%.ii
	$(CXX) $(CXXFLAGS) $(INCLUDEFLAG) -E $< -o $@

$(CLIENT)/%.c: $(CLIENT)/%.ii
	$(CXX) $(CXXFLAGS) $(INCLUDEFLAG) -E $< -o $@

clean:
	@rm -rfv $(CLIENT)/Client $(SERVER)/Server output.txt
	ls -ali

