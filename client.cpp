#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>


#include <iostream>
#include <string>

using namespace std;

int main(int argc, char *argv[])
{
    int sockfd = -1;

    sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    if ( -1 == sockfd ) 
    {
        perror( "sock created" );
        exit( -1 );
   	}
    
	struct sockaddr_in server_addr;
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    if(inet_aton("127.0.0.1",&server_addr.sin_addr) == 0) //服务器的IP地址来自程序的参数
    {
        printf("Server IP Address Error!\n");
        exit(1);
    }
    	
    server_addr.sin_port = htons(9877);
    socklen_t server_addr_length = sizeof(server_addr);
    //向服务器发起连接,连接成功后client_socket代表了客户机和服务器的一个socket连接
    if(connect(sockfd,(struct sockaddr*)&server_addr, server_addr_length) < 0)
    {
        printf("Can Not Connect!\n");
        exit(1);
    }
	
	printf("connect successfull\n");

    char recvBuf[1024] = { 0 };

	string request;
    
    while( getline(cin,request) ) 
	{
        write( sockfd, request.c_str(), request.size() );
        read( sockfd, recvBuf, sizeof( recvBuf ) );
        fputs( recvBuf, stdout );
		printf("\n");
        memset( recvBuf, 0, sizeof( recvBuf ) );
   	}

    close( sockfd );

    return 0;
}
