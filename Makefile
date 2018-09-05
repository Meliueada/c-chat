MYNAME = makefile
CC = gcc

server:server.c
	cc server.c -o server

client: client.c 
	cc client.c -o client

broadcast_server:
	cc broadcast_server.c -o broadcast_server

broadcast_client:
	cc broadcast_client.c -o broadcast_client

udp_client:
	cc udp_client.c -o udpclient

clean:
	rm server client
