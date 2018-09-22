MYNAME = makefile
CC = gcc

all: server client

utility: utility.c
	${CC} -c utility.c -o utility.o


server.o:server.c
	${CC} -c server.c -o server.o

client.o:client.c
	${CC} -c client.c -o client.o


server:server.o utility.o
	${CC} server.o utility.o -o server

client: client.o utility.o
	${CC} client.o utility.o -o client



broadcast_server:
	${CC} broadcast_server.c -o broadcast_server

broadcast_client:
	${CC} broadcast_client.c -o broadcast_client

udp_client:
	${CC} udp_client.c -o udpclient

clean:
	rm server client
