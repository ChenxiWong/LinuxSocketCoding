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
    int listenfd;  //创建套接字 相当于安装一个话机
    if((listenfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
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
    /*servaddr.sin_addr.s_addr = htonl(INADDR_ANY);*/
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    /*inet_aton("127.0.0.1", &servaddr.sin_addr);*/
    if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
        ERR_EXIT("bind");
    if(listen(listenfd, SOMAXCONN) < 0)  // 监听成功后，套接字变为被动套接字，会被用来接受连接
        ERR_EXIT("listen");
    /* 内核维护着两个队列，
     * 一个维护着已经完成三次握手的队列，
     * 一个维护着等待完成三次握手的队列,
     * 两个队列之和不能超过SOMAXCONN */
    struct sockaddr_in peer_addr;
    socklen_t peer_len = sizeof(peer_addr);
    int conn;
    if((conn = accept(listenfd, (struct sockaddr*)& peer_addr, &peer_len)) < 0)
        ERR_EXIT("accept");
    /* accept 会从已经完成连接的队列头返回一个套接字，如果队列为空，则accept函数阻塞
     * accept 执行成功后，会将该套接字从队列中移除，以便更多客户端可以连接过来  */
    /*  conn 通常被称为已经连接套接字，主动套接字  */
    char recvbuf[1024];
    while(1)
    {
        memset(recvbuf, 0, sizeof(recvbuf));
        int ret = read( conn, recvbuf, sizeof(recvbuf));
        fputs(recvbuf, stdout);
        write(conn, recvbuf, ret);
    }
    close(listenfd);
    close(conn);
    return 0;
}
