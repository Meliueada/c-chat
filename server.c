#include "utility.h"
int main()
{
    ///定义sockfd
    int server_sockfd = socket(AF_INET,SOCK_STREAM, 0);

    ssize_t n, ret;
    char buf[MAX_LINE];
    ///定义sockaddr_in
    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(MYPORT);
    server_sockaddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    ///bind，成功返回0，出错返回-1
    if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1)
    {
        perror("bind");
        exit(1);
    }

    ///listen，成功返回0，出错返回-1
    if(listen(server_sockfd,QUEUE) == -1)
    {
        perror("listen");
        exit(1);
    }


    //创建事件表
    int epfd = epoll_create(EPOLL_SIZE);
	if(epfd < 0) { perror("epfd error"); exit(-1);}
    printf("epoll created, epollfd = %d\n", epfd);
    static struct epoll_event ev, events[EPOLL_SIZE];
    /*生成用于处理accept的epoll专用文件描述符*/
    epfd = epoll_create(CONNECT_SIZE);
    /*设置监听描述符*/
    ev.data.fd = server_sockfd;
    /*设置处理事件类型*/
    ev.events = EPOLLIN | EPOLLET;
    /*注册事件*/
    epoll_ctl(epfd, EPOLL_CTL_ADD, server_sockfd, &ev);


    // 服务端用数组保存用户连接
    int clients[10] = {0};
    while(1)
    {
        int epoll_events_count = epoll_wait(epfd, events, EPOLL_SIZE, -1);
        if (epoll_events_count < 0){
            perror("epoll failure");
            break;
        }
        printf("epoll_events_count = %d\n", epoll_events_count);

        int i;
        for(i = 0; i < epoll_events_count; ++i)
        {
            int sockfd = events[i].data.fd;
            if(sockfd == server_sockfd)
            {
                struct sockaddr_in client_address;
                socklen_t client_addrLength = sizeof(struct sockaddr_in);
                int clientfd= accept( server_sockfd, ( struct sockaddr* )&client_address, &client_addrLength );
				printf("client connection from: %s : % d(IP : port), clientfd= %d \n",
 				inet_ntoa(client_address.sin_addr),
                ntohs(client_address.sin_port),
                clientfd);

                /*设置为非阻塞*/
                setnonblocking(clientfd);
                ev.data.fd = clientfd;
                ev.events = EPOLLIN | EPOLLET;
                epoll_ctl(epfd , EPOLL_CTL_ADD , clientfd , &ev);

                int i;
                int clients_count;   //在线用户数量
                for (i=0;i<10;i++){
                    if (clients[i]==0) {
                        clients[i]=clientfd;
                        break;
                    }
                    printf("%d",clients[i]);
                }
                printf("Add new clientfd = %d to epoll\n", clientfd);
                clients_count = getClientsNum(clients);
                printf("Now there are %d clients in the chatroom\n",clients_count );

         //       //服务器发送欢迎消息给client
         //       char message[BUF_SIZE];
         //       bzero(message, BUF_SIZE);
         //       sprintf(message, SERVER_WELCOME, clientfd);
         //       int ret = send(clientfd, message, BUF_SIZE, 0);
         //       if(ret < 0) { perror("send error"); exit(-1); }

            }//if

            /*如果是已链接用户，并且收到数据，进行读入*/
            else if(events[i].events & EPOLLIN){
                printf("EPOLLIN999999999999999\n");
                if((sockfd = events[i].data.fd) < 0)
                    continue;
                bzero(buf , MAX_LINE);
                if((n = read(sockfd , buf , MAX_LINE)) <= 0)
                {
                    close(sockfd);
                    events[i].data.fd = -1;
                }//if
                else{
                    buf[n] = '\0';
                    printf("clint[%d] send message: %s\n", i , buf);

                    /*设置用于注册写操作文件描述符和事件*/
                    ev.data.fd = sockfd;
                    ev.events = EPOLLOUT| EPOLLET;
                    epoll_ctl(epfd , EPOLL_CTL_MOD , sockfd , &ev);

                }//else
            }//else
                else if(events[i].events & EPOLLOUT)
            {
                printf("EPOLLOUT99999999999999\n");
                if((sockfd = events[i].data.fd) < 0)
                continue;
                sendBroadcastmessage(buf);

              //  if((ret = write(sockfd , buf , n)) != n)
              //  {
              //      printf("error writing to the sockfd!\n");
              //      break;
              //  }//if
                /*设置用于读的文件描述符和事件*/
                ev.data.fd = sockfd;
                ev.events = EPOLLIN | EPOLLET;
                /*修改*/
                epoll_ctl(epfd , EPOLL_CTL_MOD , sockfd , &ev);
            }//else

      }//for

    }

    close(server_sockfd);
    close(epfd);
    return 0;
}
