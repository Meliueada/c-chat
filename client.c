#include "utility.h"

int main(int argc, char *argv[])
{
    //创建TCP的socket
    struct sockaddr_in tcp_address;
    int sock = build_tcp_connection("CLIENT", tcp_address);

    ///定义udp的sockfd
    struct sockaddr_in recvAddr;
    int sockListen = build_udp_connection(recvAddr);

    //声明输入，输出
    char sendline[MAX_LINE] , recvline[MAX_LINE];

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
    addfd(epfd, pipe_fd[0],false);

    //判断用户是否输入用户名
    if (argc != 2){
        printf("please input your name\n");
        printf("EXAMPLE: './client li'\n");
        exit(-1);
    }
    else{
        send_packet("REGISTER", argv[1], sock);
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
       while(1){
              bzero(&sendline, MAX_LINE);
              fgets(sendline , BUF_SIZE, stdin);
              if(strncasecmp(sendline, "EXIT", strlen("EXIT")) == 0)
              {
                  send_packet("EXIT", "--", sock);
                  exit(-1);
              }
              if(write(pipe_fd[1], sendline, strlen(sendline)-1)<0){
               perror("write error"); 
               exit(-1);
           }//if
       }//while
   }//else if
   else{
       while(1){
           int epoll_events_count = epoll_wait(epfd, events, EPOLL_SIZE, -1);
           int i;
           bzero(recvline, MAX_LINE);
           for (i = 0; i < epoll_events_count; ++i){
               //接受客户端发送消息
               if (events[i].data.fd == sock){
                    receive_from_socket(sockListen, recvAddr, recvline);
               }
               //接受命令行输入信息并发送至客户端
               else{
                    read_msg(events[i].data.fd, recvline, MAX_LINE);
                    send_packet("MESSAGE", recvline, sock);
               }//else
          // }
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

            

