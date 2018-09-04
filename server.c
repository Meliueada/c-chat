#include "utility.h"
int main()
{
    ///定义sockfd
    int server_sockfd = socket(AF_INET,SOCK_STREAM, 0);

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




    int epfd = epoll_create(EPOLL_SIZE);
	if(epfd < 0) { perror("epfd error"); exit(-1);}
    printf("epoll created, epollfd = %d\n", epfd);
    static struct epoll_event events[EPOLL_SIZE];
    addfd(epfd, server_sockfd,true);


    while(1)
    {
        int epoll_events_count = epoll_wait(epfd, events, EPOLL_SIZE, -1);
        if (epoll_events_count < 0){
            perror("epoll failure");
            break;
        }
        printf("epoll_events_count = %d\n", epoll_events_count);

        for(int i = 0; i < epoll_events_count; ++i)
        {
            int sockfd = events[i].data.fd;
            if(sockfd == server_sockfd)
            {
                struct sockaddr_in client_address;
                socklen_t client_addrLength = sizeof(struct sockaddr_in);
                int clientfd = accept( server_sockfd, ( struct sockaddr* )&client_address, &client_addrLength );
				printf("client connection from: %s : % d(IP : port), clientfd = %d \n",
 				inet_ntoa(client_address.sin_addr),
                ntohs(client_address.sin_port),
                clientfd);

                addfd(epfd, clientfd, true);

				// 服务端用list保存用户连接
  //              clients_list.push_back(clientfd);
                printf("Add new clientfd = %d to epoll\n", clientfd);
 //               printf("Now there are %d clients int the chat room\n", (int)clients_list.size());

				char message[BUF_SIZE];
                bzero(message, BUF_SIZE);
                sprintf(message, SERVER_WELCOME, clientfd);
                int ret = send(clientfd, message, BUF_SIZE, 0);
                if(ret < 0) { perror("send error"); exit(-1); }
            }
            else
            {
                int ret = sendBroadcastmessage(sockfd);
                if(ret < 0) { perror("error");exit(-1); }
                
            }

            }

    }



    close(server_sockfd);
    close(epfd);
    return 0;
}
