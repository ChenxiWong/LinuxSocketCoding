// Last Update:2017-05-16 21:38:00
/**
 * @file 07socket.c
 * @brief 
 * @author wangchenxi
 * @version 0.1.00
 * @date 2017-05-16
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define ERR_EXIT(m) \
    do \
    { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while(0)


int main(int argc, char* argv[])
{
    int sock;  //创建套接字 相当于安装一个话机
    if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        ERR_EXIT("socket");
    /* listenfd = socket(PF_INET, SOCK_STREAM, 0); */
    /*                   第一个参数指定IPV4协议
     *                   第二个参数指定流式套接字
     *                   第三个可以显示指定TCP协议，也可以设置为0
     *                   因为通过前两个参数基本可以确定第三个参数   */
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5188);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    /*inet_aton("127.0.0.1", &servaddr.sin_addr);*/
    if(connect(sock, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
        ERR_EXIT("connect");
    char sendbuf[1024];
    char recvbuf[1024];
    memset(sendbuf, 0, sizeof(sendbuf));
    while(fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)
    {
        write(sock, sendbuf, strlen(sendbuf));
        memset(recvbuf, 0, sizeof(recvbuf));
        read( sock, recvbuf, strlen(sendbuf));
        fputs(recvbuf, stdout);
        memset(sendbuf, 0, sizeof(sendbuf));
    }
    close(sock);
    return 0;
}
