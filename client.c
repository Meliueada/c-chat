#include "utility.h"

//sendFunction
void sendToServer(int sockfd, char message[MAX_LINE]){
    send(sockfd, message, MAX_LINE, 0);
}

int main(int argc, char *argv[])
{
    ///定义sockaddr_in
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MYPORT);  ///服务器端口
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);  ///服务器ip

    ///定义tcp的sockfd
    int sock = socket(AF_INET,SOCK_STREAM, 0);
    if(sock <0){
        perror("sock error");
        exit(-1);
    }
    ///连接服务器，成功返回0，错误返回-1
    if (connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect");
        exit(1); }

    ///定义udp的sockfd
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
    //绑定 
    if(bind(sockListen, (struct sockaddr *)&recvAddr, sizeof(struct sockaddr)) == -1){
        perror("udp bind error");
        exit(-1);
    }
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
                printf("%s\n", recvline);
                }
                else{
                    int ret = read(events[i].data.fd, recvline, MAX_LINE);
                    if(ret == 0){
                        isClientWorking = 0;
                    }
                    else{
                       // send(sock, recvline, MAX_LINE, 0);
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

            

