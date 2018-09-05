#include "utility.h"


/*readline函数实现*/
ssize_t readline(int fd, char *vptr, size_t maxlen)
{
    ssize_t n, rc;
    char    c, *ptr;

    ptr = vptr;
    for (n = 1; n < maxlen; n++) {
        if ( (rc = read(fd, &c,1)) == 1) {
            *ptr++ = c;
            if (c == '\n')
                break;  /* newline is stored, like fgets() */
        } else if (rc == 0) {
            *ptr = 0;
            return(n - 1);  /* EOF, n - 1 bytes were read */
        } else
            return(-1);     /* error, errno set by read() */
    }

    *ptr = 0;   /* null terminate like fgets() */
    return(n);
}


/*普通客户端消息处理函数*/
void str_cli(int sockfd)
    {

     /*发送和接收缓冲区*/
     char sendline[MAX_LINE] , recvline[MAX_LINE];
     while(fgets(sendline , MAX_LINE , stdin) != NULL)
        {
        write(sockfd , sendline , strlen(sendline));
        
        bzero(recvline , MAX_LINE);
        if(readline(sockfd , recvline , MAX_LINE) == 0)
        {
        perror("server terminated prematurely");
        exit(1);
        }//if
        
        if(fputs(recvline , stdout) == EOF)
        {
        perror("fputs error");
        exit(1);
        }//if
        
        bzero(sendline , MAX_LINE);
  }//while
  }






int main()
{

    ///定义sockaddr_in
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MYPORT);  ///服务器端口
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);  ///服务器ip


    ///定义sockfd
    int sock = socket(AF_INET,SOCK_STREAM, 0);
    if(sock <0){
        perror("sock error");
        exit(-1);
    }

    ///连接服务器，成功返回0，错误返回-1
    if (connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect");
        exit(1);
    }



    //判断客户端是否正常工作
    bool isClientwork = true;

    char message[BUF_SIZE];

    str_cli(sock);

    close(sock);
    return 0;
}



