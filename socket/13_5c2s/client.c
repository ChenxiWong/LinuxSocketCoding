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
#include <signal.h>
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


ssize_t readn(int fd, void* buf, size_t count)
{
    size_t nleft = count;
    ssize_t nread = 0;
    char* bufp = (char*) buf;
    while( nleft > 0)
    {
        if((nread = read(fd, bufp, nleft)) < 0)
        {
            if(errno == EINTR)continue;
            return -1;
        }
        else if (nread == 0) // EOF
            return count - nleft;

        bufp += nread;
        nleft -= nread;
    }
    return count;
}


ssize_t writen(int fd, void* buf, size_t count)
{
    size_t nleft = count;
    ssize_t nwritten = 0;
    char* bufp = (char*) buf;
    while( nleft > 0)
    {
        if((nwritten = write(fd, bufp, nleft)) < 0)
        {
            if(errno == EINTR) // 出现信号中断
                continue;
            return -1;
        }
        else if (nwritten == 0)
            continue;

        bufp += nwritten;
        nleft -= nwritten;
    }
    return count;
}

ssize_t recv_peek(int sockfd, void *buf, size_t len)
{
    while(1)
    {
        int ret = recv(sockfd, buf, len, MSG_PEEK);
        if(ret == -1 && errno == EINTR)
            continue;
        return ret;
    }
}
ssize_t readline(int sockfd, void* buf, size_t maxline)
{
    int ret;
    int nread;
    char* bufp = buf;
    int nleft = maxline;
    while(1)
    {
        ret = recv_peek(sockfd, bufp, nleft);
        if ( ret == 0)
            return ret;
        nread = ret;
        if( nread > nleft)
            exit(EXIT_FAILURE);
        int i;
        for( i = 0; i < nread; ++i)
        {
            if ( '\n' == bufp[i])
            {
                ret = readn( sockfd, bufp, i + 1);
                if ( ret != nread)
                    exit(EXIT_FAILURE);
                return ret;
            }
        }
        nleft -= nread;
        ret = readn( sockfd, bufp, nread);
        if( ret != nread)
            exit(EXIT_FAILURE);
        bufp += nread;
    }
    return -1;
}

void echo_cli(int sock)
{
    char sendbuf[1024];
    char recvbuf[1024];
    memset(&sendbuf, 0, sizeof(sendbuf));
    memset(&recvbuf, 0, sizeof(recvbuf));
    while(fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)
    {
        writen(sock, sendbuf, strlen(sendbuf));

        int ret = readline( sock, recvbuf, sizeof(recvbuf));
        if(ret == -1)
        {
            ERR_EXIT("readline");
        }
        else if(ret == 0)
        {
            printf("client stoped now.");
            break;
        }
        fputs(recvbuf, stdout);
        memset(sendbuf, 0, sizeof(sendbuf));
        memset(recvbuf, 0, sizeof(recvbuf));
    }
}



int main(int argc, char* argv[])
{
    int sock[5];  //创建套接字 相当于安装一个话机
    int i = 0;
    for(; i < 5; ++i)
    {
        if((sock[i] = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
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
        if(connect(sock[i], (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
            ERR_EXIT("connect");
        struct sockaddr_in localaddr;
        socklen_t addrlen = sizeof(localaddr);
        /* 根据socket 获取IP、端口  */
        if(getsockname( sock[i], (struct sockaddr*)&localaddr, &addrlen) < 0)
            ERR_EXIT("getsockname");
        printf("ip = %s, port = %d\n", inet_ntoa(localaddr.sin_addr), ntohs(localaddr.sin_port));
    }
    echo_cli(sock[0]);
    //close(sock);
    return 0;
}
