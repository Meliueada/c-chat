#include <sys/epoll.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TCP_PORT 8887
#define UDP_PORT 4001
#define QUEUE   20
#define BUFFER_SIZE 1024
//#define SERVER_IP "192.168.10.204" 
#define SERVER_IP "127.0.0.1"
#define MAX_LINE 2048
#define CONNECT_SIZE 256

//epoll 支持的最大并发量
#define EPOLL_SIZE 5000
#define SERVER_WELCOME "-----------------Welcome client 【%s】 to join the chatroom!-------------------"
#define SERVER_EXIT  "-------------------client 【%s】exit the chatroom!----------------------"
#define BUF_SIZE 0x10000

#define CAUTION "There is only one in the chatroom!"
#define USER_COUNT "Now there are %d users in the chatroom"
#define USER_SPEAK "【%s】 say >>>> %s"
#define HEAD_LEN 10

struct register_info
{
    
}




int build_tcp_connection(char *conn_type, struct sockaddr_in sock_address);
int build_udp_connection(struct sockaddr_in sock_address);

int getClientsNum(char (*clients)[10]);

void deleteClient(char (*clients)[10], int num);

int setnonblocking(int sockfd);

int sendBroadcastmessage(char msg[30]);

void addfd( int epollfd, int fd, bool enable_et );

//sendFunction
void sendToServer(int sockfd, char message[MAX_LINE]);


char* make_packet(char *msg_head, char *msg_body);

void send_packet(char *packet_type, char *message, int sockfd);

void receive_from_socket(int sockfd, struct sockaddr_in sock_address, char *recvline);

char *read_msg(int sockfd, char *src, int buffer);

int accept_client(int server_sock);





