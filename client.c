#include "utility.h"

//sendFunction
void sendToServer(int sockfd, char message[MAX_LINE]){
    if(send(sockfd, message, MAX_LINE, 0)== -1)
    {
        perror("send error\n");
    }

}

int main(int argc, char *argv[])
{
    struct sockaddr_in tcp_address;


    //创建TCP的socket
    int sock = create_connection("TCP", "CLIENT", tcp_address);

    struct sockaddr_in recvAddr;
    ///定义udp的sockfd
    int sockListen = build_udp_client(recvAddr);

    char sendline[MAX_LINE] , recvline[MAX_LINE];
    //判断客户端是否正常工作
    bool isClientWorking = true;
    //创建管道, fd[0]读，fd[1]写
    int pipe_fd[2];
    if(pipe(pipe_fd) < 0) { perror("pipe error"); exit(-1); }
    //创建epoll
    int epfd = epoll_create(EPOLL_SIZE);
    if(epfd < 0) { perror("epfd error"); exit(-1); }
    static struct epoll_event ev, events[EPOLL_SIZE];

    //添加sock到epoll
    epfd = epoll_create(CONNECT_SIZE);
    ev.data.fd = sock;
    ev.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockListen, &ev);
    //添加管道到epoll
    ev.data.fd = pipe_fd[0];
    ev.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD,pipe_fd[0],&ev);
    if (argc != 2){
        printf("please input your name\n");
        printf("EXAMPLE: './client li'\n");
        exit(-1);
    }
    else{
        char *registerPacket;
        registerPacket = makePacket("REGISTER--",argv[1]);
        sendToServer(sock, registerPacket);
    }

   //创建子进程 
   //
   int pid = fork();
   char *delim = "--"; //用户名称分隔符
   char *p;
   char type[32];

   if(pid < 0) { perror("fork error"); exit(-1); }
   else if(pid == 0)     // 子进程
   {
       close(pipe_fd[0]);
       printf("you could input EXIT to quit the chatroom\n");
       while(isClientWorking){
              bzero(&sendline, MAX_LINE);
              fgets(sendline , BUF_SIZE, stdin);
              if(strncasecmp(sendline, "EXIT", strlen("EXIT")) == 0)
              {
                  char *exitPacket;
                  exitPacket=makePacket("EXIT","--");
                  sendToServer(sock, exitPacket);
                  printf("have a good day and wish to see you again\n");
                  exit(-1);
              }
              if(write(pipe_fd[1], sendline, strlen(sendline)-1)<0){
               perror("write error"); 
               exit(-1);
           }//if
       }//while
   }//else if
   else{
       while(isClientWorking){
           int epoll_events_count = epoll_wait(epfd, events, EPOLL_SIZE, -1);
           int i;
           bzero(recvline, MAX_LINE);
           for (i = 0; i < epoll_events_count; ++i){
               int even_fd = events[i].data.fd;
               if (events[i].data.fd == sock){
               int recvbytes;
               int addrLen = sizeof(struct sockaddr_in);
               if((recvbytes = recvfrom(sockListen, recvline, 128, 0, (struct sockaddr *)&recvAddr, &addrLen)) != -1)
               recvline[recvbytes] = '\0';
               printf("recvie%s\n", recvline);
               }
               else{
                   int ret = read(events[i].data.fd, recvline, MAX_LINE);
                   if(ret == 0){
                       isClientWorking = 0;
                   }
                   else{
                      // send(sock, recvline, MAX_LINE, 0);
                      //printf("recvline%s\n",recvline);
                      sendToServer(sock, recvline);
                   }//else
               }//else
           }//for
       }//while
   }//else

   if(pid){
      //关闭父进程和sock
       close(pipe_fd[0]);
       close(sock);
   }else{
       //关闭子进程
       close(pipe_fd[1]);
   }
   return 0;

}

            

