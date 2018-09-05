#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>



#define MYPORT  8887
#define QUEUE   20
#define BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1" 
#define MAXLINE 4096
#define MAX_LINE 2048
#define CONNECT_SIZE 256


//epoll 支持的最大并发量
#define EPOLL_SIZE 5000

#define SERVER_WELCOME "Welcome you join  to the chat room! Your chat ID is: Client #%d"

#define BUF_SIZE 0x10000


#define CAUTION "There is only one in the chatroom!"

#define SERVER_MESSAGE "ClientID %d say >> %s"


/*数组储存当前用户连接状态
 * 返回在线用户数量等
 */
//返回当前客户数量
int getClientsNum(int *clients){
int i;
int count=0;
for (i = 0; i<10; i++){
    if (clients[i]!= 0){
        count ++;
    }
}

return count;
}

//删除客户
void deleteClient(int *clients, int num){
int i;
int clients_index;
for (i=0; i<10; i++){
    if (clients[i] == num){
        clients_index = i;
        clients[i] = 0;
        break;
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
    theirAddr.sin_port = htons(4001);

    int sendBytes;
    
    if((sendBytes = sendto(bcfd, msg, strlen(msg), 0,
            (struct sockaddr *)&theirAddr, sizeof(struct sockaddr))) == -1){
        printf("sendto fail, errno=%d\n", errno);
        return -1;
    }
    printf("msg=%s, msgLen=%d, sendBytes=%d\n", msg, strlen(msg), sendBytes);
   // close(bcfd);
    return 0;

}

//群聊消息通过广播接受
int getBroadcastMessage(){
    int sockListen;
    if((sockListen = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
        printf("socket fail\n");
        exit(-1);
    }
    int set = 1;
    setsockopt(sockListen, SOL_SOCKET, SO_REUSEADDR, &set, sizeof(int));
    struct sockaddr_in recvAddr;
    memset(&recvAddr, 0, sizeof(struct sockaddr_in));
    recvAddr.sin_family = AF_INET;
    recvAddr.sin_port = htons(4001);
    recvAddr.sin_addr.s_addr = INADDR_ANY;
    // 必须绑定，否则无法监听
    if(bind(sockListen, (struct sockaddr *)&recvAddr, sizeof(struct sockaddr)) == -1){
        printf("bind fail\n");
        exit(-1);
    }
    int recvbytes;
    char recvbuf[128];
    recvbuf[recvbytes] = '\0';
    int addrLen = sizeof(struct sockaddr_in);
    while(1){
    if((recvbytes = recvfrom(sockListen, recvbuf, 128, 0,
        (struct sockaddr *)&recvAddr, &addrLen)) != -1){
        recvbuf[recvbytes] = '\0';
        printf("receive a broadCast messgse:%s\n", recvbuf);
    }else{
        printf("recvfrom fail\n");
    }
    }
    close(sockListen);
    return 0;

}


