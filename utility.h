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
#define SERVER_WELCOME "Welcome client 【%s】 to join the chatroom!"
#define SERVER_EXIT  "client 【%s】exit the chatroom!"
#define BUF_SIZE 0x10000

#define CAUTION "There is only one in the chatroom!"
#define USER_COUNT "Now there are %d users in the chatroom"
#define USER_SPEAK "Client 【%s】 say >>>> %s"

//函数声明
//void set_address(struct sockaddr_in sock_address, int port, char *ip_address);
int do_socket(int sockfd, char *conn_type, struct sockaddr_in servaddr);

//创建socket及bind，listen等
int create_connection(char *sock_type, char *conn_type, struct sockaddr_in sock_address){
    int sockfd;
    //创建TCP类型的SOCKET
    if(strncasecmp(sock_type, "TCP", strlen("TCP")) == 0) 
    {
        printf("TCP\n");
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
            perror("socket error");
        }
        //设置地址，端口等
        memset(&sock_address, 0, sizeof(sock_address));
        sock_address.sin_family = AF_INET;
        sock_address.sin_port = htons(TCP_PORT);  ///服务器端口
        sock_address.sin_addr.s_addr = inet_addr(SERVER_IP);  ///服务器ip

    //    set_address(sock_address, TCP_PORT, SERVER_IP);
    }
    //创建UDP类型的SOCKET
    else if(strncasecmp(sock_type, "UDP", strlen("UDP")) == 0)
    {
        printf("UDP\n");
        if((sockfd = socket(AF_INET, SOCK_DGRAM, 0))==-1)
        {
            perror("socket error");
        }
        int set = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &set, sizeof(int));
        //设置地址，端口等
        memset(&sock_address, 0, sizeof(struct sockaddr_in));
        sock_address.sin_family = AF_INET;
        sock_address.sin_port = htons(UDP_PORT);  ///服务器端口
        sock_address.sin_addr.s_addr = inet_addr(SERVER_IP);  ///服务器ip
        printf("utility s_addr%d\n",sock_address.sin_addr.s_addr);

        if(bind(sockfd,(struct sockaddr *)&sock_address, sizeof(struct sockaddr))==-1)
        {
            perror("bind");
            exit(1);
        }
    }

    sockfd = do_socket(sockfd, conn_type, sock_address);
    return sockfd;
} 

////设置地址端口等
//void set_address(struct sockaddr_in sock_address, int port, char *ip_address)
//    {
//        memset(&sock_address, 0, sizeof(sock_address));
//        sock_address.sin_family = AF_INET;
//        sock_address.sin_port = htons(port);  ///服务器端口
//        sock_address.sin_addr.s_addr = inet_addr(ip_address);  ///服务器ip
//    }


int do_socket(int sockfd, char *conn_type, struct sockaddr_in servaddr)
{
    if(strncasecmp(conn_type, "SERVER", strlen("SERVER")) == 0)  //服务器端socket
    {
        printf("%s\n","SERVER");
        if(bind(sockfd,(struct sockaddr *)&servaddr, sizeof(servaddr))==-1)
        {
            perror("bind");
            exit(1);
        }
        if(listen(sockfd,QUEUE) == -1)
        {
            perror("listen");
            exit(1);
        }
    }//if

    else if(strncasecmp(conn_type, "CLIENT", strlen("CLIENT")) == 0)  //客户端socket
    {
        printf("%s\n","CLIENT");
        if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        {
            perror("connect");
            exit(1); 
        }
    }//else if

    return sockfd;
}//void


int build_udp_client(struct sockaddr_in sock_address){
      int sockListen;
      if((sockListen = socket(AF_INET, SOCK_DGRAM, 0))==-1){
         perror("udp sock error");
         exit(-1);
      }

   int set = 1;
   setsockopt(sockListen, SOL_SOCKET, SO_REUSEADDR, &set, sizeof(int));
   struct sockaddr_in recvAddr;
   memset(&recvAddr, 0, sizeof(struct sockaddr_in));
   recvAddr.sin_family = AF_INET;
   recvAddr.sin_port = htons(4001);
   recvAddr.sin_addr.s_addr = INADDR_ANY;
   printf("%ds_addr\n",recvAddr.sin_addr.s_addr);
   //绑定 
   if(bind(sockListen, (struct sockaddr *)&recvAddr, sizeof(struct sockaddr)) == -1){
       perror("udp bind error");
       exit(-1);
   }
   return sockListen;
 
}


/*数组储存当前用户连接状态
 * 返回在线用户数量等
 */
//返回当前客户数量
int getClientsNum(char (*clients)[10]){
int i;
int j;
int count=0;
for (i = 0; i<10; i++){
    for(j=0;j<10; j++){
    if (clients[i][j]){
        printf("users:%s\n",clients[i]);
        count ++;
        printf("j%d:\n",j);
        break;
    }
 }
}

return count;
}

//删除客户
void deleteClient(char (*clients)[10], int num){
int j;
    for(j=0; j<10; j++){
    if (clients[num][j]){
        clients[num][j] = 0;
    }

}
}


//设置sockfd,pipefd非阻塞
int setnonblocking(int sockfd)
{
    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)| O_NONBLOCK);
    return 0;
}


//将服务器收到的clientfd的消息进行广播
int sendBroadcastmessage(char msg[30])
{
    int bcfd;
    if((bcfd = socket(PF_INET, SOCK_DGRAM, 0)) == -1){
        printf("socket fail\n");
        return -1;
    }
    int optval = 1;
    setsockopt(bcfd, SOL_SOCKET, SO_BROADCAST | SO_REUSEADDR, &optval, sizeof(int));
    struct sockaddr_in theirAddr;
    memset(&theirAddr, 0, sizeof(struct sockaddr_in));
    theirAddr.sin_family = AF_INET;
    theirAddr.sin_addr.s_addr = inet_addr("255.255.255.255");
    theirAddr.sin_port = htons(UDP_PORT);

    int sendBytes;
    
    if((sendBytes = sendto(bcfd, msg, strlen(msg), 0,
            (struct sockaddr *)&theirAddr, sizeof(struct sockaddr))) == -1){
        printf("sendto fail, errno=%d\n", errno);
        return -1;
    }
    printf("msg=%s\n", msg );
    close(bcfd);
    return 0;

}

void addfd( int epollfd, int fd, bool enable_et )
{
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN;     //输入触发epoll-event
    if( enable_et )
        ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
    setnonblocking(fd);
 //   printf("fd added to epoll!\n\n");
}

//生成报文
char* makePacket(char *s1, char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    if (result == NULL) exit (1);
    strcpy(result, s1);
    strcat(result, s2);

    return result;

}



