build:
	gcc server/server.c -o server/Server
	gcc client/client.c -o client/Client

clean:
	rm -rf server/Server client/Client

