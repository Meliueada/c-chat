MYNAME = makefile
CC = gcc

all: server client

server:server.c
	${CC} server.c -o server

client: client.c 
	${CC} client.c -o client

broadcast_server:
	${CC} broadcast_server.c -o broadcast_server

broadcast_client:
	${CC} broadcast_client.c -o broadcast_client

udp_client:
	${CC} udp_client.c -o udpclient

clean:
	rm server client
