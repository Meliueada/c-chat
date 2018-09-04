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

//epoll 支持的最大并发量
#define EPOLL_SIZE 5000

#define SERVER_WELCOME "Welcome you join  to the chat room! Your chat ID is: Client #%d"

#define BUF_SIZE 0x10000


#define CAUTION "There is only one int the char room!"

#define SERVER_MESSAGE "ClientID %d say >> %s"


//设置sockfd,pipefd非阻塞
int setnonblocking(int sockfd)
{
    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)| O_NONBLOCK);
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




//将服务器收到的clientfd的消息进行广播
int sendBroadcastmessage(int clientfd)
{

    char buf[BUF_SIZE], message[BUF_SIZE];
    //清零
    bzero(buf, BUF_SIZE);
    bzero(message, BUF_SIZE);

    printf("read from client(clientID = %d)\n", clientfd);
    int len = recv(clientfd, buf, BUF_SIZE, 0);

    if(len == 0)  // len = 0 client关闭了连接
    {
        close(clientfd);
      //  clients_list.remove(clientfd); //list删除fd
     //   printf("ClientID = %d closed.\n now there are %d client in the char room\n", clientfd, (int)clients_list.size());

    }
    else  //进行广播
    {
     //   if(clients_list.size() == 1) {
     //       send(clientfd, CAUTION, strlen(CAUTION), 0);
     //       return len;
     //   }

        sprintf(message, SERVER_MESSAGE, clientfd, buf);

      //  for(it = clients_list.begin(); it != clients_list.end(); ++it) {
      //     if(*it != clientfd){
      //          if( send(*it, message, BUF_SIZE, 0) < 0 ) { perror("error"); exit(-1);}
      //     }
      //  }
    }
    return len;
}

