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


//新用户注册
void register_to_server(int argc, char **argv, int server_fd)
{
    register_info info;
    //判断用户是否输入用户名
    if (argc != 2){
        printf("USAGE: ./client <nickname>\n");
        exit(-1);
    }
    else{
        strcpy(info.name , argv[1]);
        send_packet("REGISTER", argv[1], server_fd);
    }
}

//心跳测试，反馈服务器是否关闭
void heart_beat(int server_fd)
{
    printf("dddddd%d\n", server_fd);

    //if(send_packet("HEART", "--", server_fd) == -1)
    if(send_packet("MESSAGE", "--", server_fd) == -1)
    {
        perror("server has closed");
    }
    sleep(10);

}


////接收命令行参数
void read_from_cmd(int tcp_sock){
    //声明输入，输出
    char sendline[MAX_LINE] , recvline[MAX_LINE];
    while(1)
    {
    bzero(&sendline, MAX_LINE);
    fgets(sendline , BUF_SIZE, stdin);
    printf("用户输入了%s\n",sendline);
    send_packet("MESSAGE", recvline, tcp_sock);
    }
}

int main(int argc, char *argv[])
{
    
    pthread_t thread[2];
    //创建TCP的socket
    int tcp_sock = create_connection("TCP");
    ///定义udp的sockfd
    int udp_sock = create_connection("UDP");

    //用户向服务器注册
    register_to_server(argc, argv, tcp_sock);

    printf("severfd------%d",tcp_sock);
    //心跳测试线程
    memset(&thread, 0, sizeof(thread));
    pthread_create(&thread[0], NULL, heart_beat, tcp_sock);


    pthread_create(&thread[1], NULL, read_from_cmd,  tcp_sock);
    if(thread[0] !=0) {                   //comment4
                pthread_join(thread[0],NULL);
                printf("线程1已经结束\n");
        }
        if(thread[1] !=0) {                //comment5
                pthread_join(thread[1],NULL);
                printf("线程2已经结束\n");
        }

//    //声明输入，输出
//    char sendline[MAX_LINE] , recvline[MAX_LINE];
//
//    //创建管道, fd[0]读，fd[1]写
//    int pipe_fd[2];
//    if(pipe(pipe_fd) < 0) { perror("pipe error"); exit(-1); }
//
//    //创建epoll
//    int epfd = epoll_create(EPOLL_SIZE);
//    if(epfd < 0) { perror("epfd error"); exit(-1); }
//    static struct epoll_event ev, events[EPOLL_SIZE];
//
//    //添加sock到epoll
//    addfd(epfd, udp_sock, false);
//    //添加管道到epoll
//    addfd(epfd, pipe_fd[0],false);
//
//   //创建子进程 
//   int pid = fork();
//
//   if(pid < 0) { perror("fork error"); exit(-1); }
//   else if(pid == 0)     // 子进程
//   {
//       close(pipe_fd[0]);
//       printf("You could input EXIT to quit the chatroom\n");
//       while(1){
//              bzero(&sendline, MAX_LINE);
//              fgets(sendline , BUF_SIZE, stdin);
//              if(strncasecmp(sendline, "EXIT", strlen("EXIT")) == 0)
//              {
//                  send_packet("EXIT", "--", tcp_sock);
//                  exit(-1);
//              }
//              if(write(pipe_fd[1], sendline, strlen(sendline)-1)<0){
//               perror("write error"); 
//               exit(-1);
//           }//if
//       }//while
//   }//else if
//   else{
//       while(1){
//           int epoll_events_count = epoll_wait(epfd, events, EPOLL_SIZE, -1);
//           int i;
//           bzero(recvline, MAX_LINE);
//           for (i = 0; i < epoll_events_count; ++i){
//               //接受server消息
//               if (events[i].data.fd == udp_sock){
//                  receive_from_socket(udp_sock, recvline);
//               }
//               //接受命令行输入信息并发送至客户端
//               else{
//                    read_msg(events[i].data.fd, recvline, MAX_LINE);
////                    send_packet("MESSAGE", recvline, tcp_sock);
//               }//else
//           }//for
//       }//while
//   }//else
//
//   if(pid){
//      //关闭父进程和sock
//       close(pipe_fd[0]);
//       close(tcp_sock);
//   }else{
//       //关闭子进程
//       close(pipe_fd[1]);
//   }
//   return 0;

}

            

