#include <stdio.h>

//主函数(main)

1. register;
2. connect to server
3. receive other client's message
4. heartbeat
5. unregist

create_client_connection(conn_type, ip, port)
{
    connect;
    if  conn_type == 'udp':
        create_udp_connection(ip, port)
    else if conn_type =='tcp':
        create_tcp_connection(ip, port)
}

create_server_connection(conn_type, ip, port)
{
    listen;
}

create_udp_connection(ip, port)
create_tcp_connection(ip, port)

main()
{
    char client_ip;
    int clientfd = create_client_connection("TCP", server_ip, server_port);
    while true
    {
        
    }
}
