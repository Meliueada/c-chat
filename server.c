#include "utility.h"
int main()
{
    //服务器发送的通知消息
    char message[BUF_SIZE];
    ssize_t n, ret;
    char buf[MAX_LINE];

    //创建TCP的socket
    struct sockaddr_in serv_address;
    int server_sockfd = build_tcp_connection("SERVER", serv_address);

   //创建EPOLL
    int epfd = epoll_create(EPOLL_SIZE);
    if(epfd < 0) { perror("epfd error"); exit(-1);}
    static struct epoll_event ev, events[EPOLL_SIZE];
    addfd(epfd, server_sockfd,true);

    // 服务端用数组保存用户连接
    char clients[20][10] = {0};
    //主循环
    while(1)
    {
        int epoll_events_count = epoll_wait(epfd, events, EPOLL_SIZE, -1);
        if (epoll_events_count < 0){
            perror("epoll failure");
            break;
        }

        int i;
        for(i = 0; i < epoll_events_count; ++i)
        {
            int sockfd = events[i].data.fd;
            //收到客户端连接
            if(sockfd == server_sockfd)
            {
                int clientfd= accept_client(server_sockfd);
                //添加clientfd进epoll
                addfd(epfd, clientfd, true);
            }//if

            /*如果是已链接用户，并且收到数据，进行读入*/
            else if(events[i].events & EPOLLIN){
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
                    char message_type[32];
                    char *p;
                    char *delim = "--";
                    char *username;
                    int usernum = events[i].data.fd;

                    sprintf(message_type, "%s\n",strtok(buf,delim));
                    while(p=strtok(NULL,delim)){
                        username = p;
                        printf("%s", p);

                }
                if(strncasecmp(message_type, "REGISTER", strlen("REGISTER")) == 0)
                {
                    //新用户欢迎广播
                    sprintf(message, SERVER_WELCOME, username);
                    sendBroadcastmessage(message);
                    sprintf(clients[usernum], "%s",username);
                    printf("用户名称登记为%s\n",clients[usernum]);
                    printf("用户名称登记号为%d\n",usernum);
                    //用户数量播报
                    int clients_count = getClientsNum(clients);
                    sprintf(message, USER_COUNT, clients_count);
                    sendBroadcastmessage(message);
                    continue;
                }
                else if(strncasecmp(message_type, "EXIT", strlen("EXIT")) == 0)
                    {
                        //用户退出广播
                    sprintf(message, SERVER_EXIT, clients[usernum]);
                    sendBroadcastmessage(message);
                    printf("用户名称登记号为%d",usernum);
                    deleteClient(clients, usernum);
                    int clients_count = getClientsNum(clients);
                    sprintf(message, USER_COUNT, clients_count);
                    sendBroadcastmessage(message);
                    continue;
                    }
                //设置用于注册写操作文件描述符和事件
                modfd(epfd, sockfd, false);

                }//else
            }//else
            else if(events[i].events & EPOLLOUT)
            {
                if((sockfd = events[i].data.fd) < 0)
                continue;
                sprintf(message, USER_SPEAK, clients[sockfd],buf);
                printf("message%s\n",message);

                sendBroadcastmessage(message);
                //设置用于读的文件描述符和事件
                modfd(epfd, sockfd, true);
            }//else
      }//for
    }
    close(server_sockfd);
    close(epfd);
    return 0;
}
