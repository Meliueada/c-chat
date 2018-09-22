#include "utility.h"



//创建TCP类型的SOCKET
int build_tcp_connection(char *conn_type, struct sockaddr_in sock_address){
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket error");
    }
    //设置地址，端口等
    memset(&sock_address, 0, sizeof(sock_address));
    sock_address.sin_family = AF_INET;
    sock_address.sin_port = htons(TCP_PORT);  ///服务器端口
    sock_address.sin_addr.s_addr = inet_addr(SERVER_IP);  ///服务器ip

    if(strncasecmp(conn_type, "SERVER", strlen("SERVER")) == 0)  //服务器端socket
    {
        if(bind(sockfd,(struct sockaddr *)&sock_address, sizeof(sock_address))==-1)
        {
            perror("bind");
            exit(1);
        }
        if(listen(sockfd,QUEUE) == -1)
        {
            perror("listen");
            exit(1);
        }
    }
    //创建CLIENT的SOCKET
    else if(strncasecmp(conn_type, "CLIENT", strlen("CLIENT")) == 0)  //客户端socket
    {
        if(connect(sockfd, (struct sockaddr *)&sock_address, sizeof(sock_address)) < 0)
        {
            perror("connect");
            exit(1); 
        }
    }
    return sockfd;
} 


//创建UDP连接 
int build_udp_connection(struct sockaddr_in sock_address){
   int sockListen;
   if((sockListen = socket(AF_INET, SOCK_DGRAM, 0))==-1)
   {
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
   return sockListen;
}


/*数组储存当前用户连接状态
 * 返回在线用户数量等
 */
//返回当前客户数量
int getClientsNum(char (*clients)[10]){
int i;
int j;
int count=0;
for (i = 0; i<10; i++){
    for(j=0;j<10; j++){
    if (clients[i][j]){
        printf("users:%s\n",clients[i]);
        count ++;
        break;
    }
 }
}

return count;
}


//删除客户
void deleteClient(char (*clients)[10], int num){
int j;
    for(j=0; j<10; j++){
    if (clients[num][j]){
        clients[num][j] = 0;
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
    theirAddr.sin_port = htons(UDP_PORT);

    int sendBytes;
    
    if((sendBytes = sendto(bcfd, msg, strlen(msg), 0,
            (struct sockaddr *)&theirAddr, sizeof(struct sockaddr))) == -1){
        printf("sendto fail, errno=%d\n", errno);
        return -1;
    }
    printf("msg=%s\n", msg );
    close(bcfd);
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
}


void modfd( int epollfd, int fd, char enable_in)
{
    struct epoll_event ev;
    ev.data.fd = fd;
    if(enable_in)
        ev.events = EPOLLIN|EPOLLET;     
    else 
        ev.events = EPOLLOUT| EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
    setnonblocking(fd);
 //   printf("fd added to epoll!\n\n");
}

//sendFunction
void sendToServer(int sockfd, char message[MAX_LINE]){
    if(send(sockfd, message, MAX_LINE, 0)== -1)
    {
        perror("send error\n");
    }
}


//组装报文
char* make_packet(char *msg_head, char *msg_body)
{
    char *head = malloc(strlen(msg_head)+10);//+1 for the zero-terminator
    int i ;
    strcpy(head, msg_head);
    for (i=0; i<HEAD_LEN; i++)
    {
        if (strlen(head) <HEAD_LEN)
        {
            strcat(head," ");
        }
    }
    char *len = malloc(strlen(msg_body)+3);
    sprintf(len,"%03d",strlen(msg_body));

    char *result = malloc(strlen(msg_head)+strlen(len)+strlen(msg_body)+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    if (result == NULL) exit (1);
    strcpy(result, head);
    strcat(result, len);
    strcat(result, msg_body);
    return result;
}

//发送报文
void send_packet(char *packet_type, char *message, int sockfd)
{

    char *packet = make_packet(packet_type, message);

    if(send(sockfd, packet, MAX_LINE, 0)== -1)
    {
        perror("send error\n");
    }

}


void receive_from_socket(int sockfd, struct sockaddr_in sock_address, char *recvline)
{
    int recvbytes;
    int addrLen = sizeof(struct sockaddr_in);
    if((recvbytes = recvfrom(sockfd, recvline, 128, 0, (struct sockaddr *)&sock_address, &addrLen)) != -1)
    {

        recvline[recvbytes] = '\0';
        printf("%s\n", recvline);
    }
   // return recvline;
}

//封装read函数
char *read_msg(int sockfd, char *src, int buffer)
{
    if(read(sockfd, src, buffer)<0)
    {
        perror("read error");
    }

    return src;
}

//封装accept函数
int accept_client(int server_sock)
{
    struct sockaddr_in client_address;
    int clientfd;
    socklen_t client_addrLength = sizeof(struct sockaddr_in);
    clientfd= accept( server_sock, ( struct sockaddr* )&client_address, &client_addrLength);
    return clientfd;
}


//组装报文
char make_package(char *package_type)
{
    if(strncasecmp(package_type, "REGISTER", strlen("REGISTER")) == 0)
    {
        make_register_packeage();
    }
    else if(strncasecmp(package_type, "EXIT", strlen("REGISTER")) == 0)
    {
        make_exit_package();
    }
    
}

