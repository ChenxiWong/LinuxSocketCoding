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


struct packet
{
    int len;
    char buffer[1024];
};


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
    struct packet sendbuf;
    struct packet recvbuf;
    memset(&sendbuf, 0, sizeof(sendbuf));
    memset(&recvbuf, 0, sizeof(recvbuf));
    while(fgets(sendbuf.buffer, sizeof(sendbuf.buffer), stdin) != NULL)
    {
        int n = strlen(sendbuf.buffer);
        sendbuf.len = htonl(n);
        write(sock, &sendbuf, 4+n);

        int ret = readn( sock, &recvbuf.len, 4);
        if(ret == -1)
        {
            ERR_EXIT("read");
        }
        else if(ret < 4)
        {
            printf("client stoped now.");
            break;
        }
        n = ntohl(recvbuf.len);
        ret = readn( sock, recvbuf.buffer, n);
        if(ret == -1)
            ERR_EXIT("read");
        else if(ret < n)
        {
            printf("client stoped now.");
            break;
        }
        fputs(recvbuf.buffer, stdout);
        memset(&sendbuf, 0, sizeof(sendbuf));
        memset(&recvbuf, 0, sizeof(recvbuf));
    }
    close(sock);
    return 0;
}
