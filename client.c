#include "utility.h"

//创建连接socket
int create_connection(char *conn_type)
{
    int sockfd;
    struct sockaddr_in address;
    if(strncasecmp(conn_type, "TCP", strlen("TCP")) == 0)
    {
        sockfd = build_tcp_connection("CLIENT", address);
    }
    else if(strncasecmp(conn_type, "UDP", strlen("UDP"))==0)
    {
        sockfd = build_udp_connection(address);
    }
    return sockfd;
}

void register_to_server(int server_fd, struct register_info info)
{
    

}





int main(int argc, char *argv[])
{
    
    //创建TCP的socket
    int tcp_sock = create_connection("TCP");
    ///定义udp的sockfd
    int udp_sock = create_connection("UDP");


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
    ev.data.fd = tcp_sock;
    ev.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, udp_sock, &ev);

    //添加管道到epoll
    addfd(epfd, pipe_fd[0],false);

    //判断用户是否输入用户名
    if (argc != 2){
        printf("USAGE: ./client <nickname>\n");
        exit(-1);
    }
    else{
        send_packet("REGISTER", argv[1], tcp_sock);
    }

   //创建子进程 
   int pid = fork();

   if(pid < 0) { perror("fork error"); exit(-1); }
   else if(pid == 0)     // 子进程
   {
       close(pipe_fd[0]);
       printf("You could input EXIT to quit the chatroom\n");
       while(1){
              bzero(&sendline, MAX_LINE);
              fgets(sendline , BUF_SIZE, stdin);
              if(strncasecmp(sendline, "EXIT", strlen("EXIT")) == 0)
              {
                  send_packet("EXIT", "--", tcp_sock);
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
               //接受server消息
               if (events[i].data.fd == tcp_sock){
                  //  receive_from_socket(udp_sock, recvAddr, recvline);
                  printf("USAGE: ./client <nickname>\n");
               }
               //接受命令行输入信息并发送至客户端
               else{
                    read_msg(events[i].data.fd, recvline, MAX_LINE);
                    send_packet("MESSAGE", recvline, tcp_sock);
               }//else
           }//for
       }//while
   }//else

   if(pid){
      //关闭父进程和sock
       close(pipe_fd[0]);
       close(tcp_sock);
   }else{
       //关闭子进程
       close(pipe_fd[1]);
   }
   return 0;

}

            

