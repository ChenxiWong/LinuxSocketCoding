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

void echo_serv(int conn)
{
    char recvbuf[1024];
    while(1)
    {
        memset(&recvbuf, 0, sizeof(recvbuf));
        int ret = readline( conn, &recvbuf, 1024);
        if(ret == -1)
            ERR_EXIT("readline");
        if( ret == 0)
        {
            printf("client stoped now.");
            break;
        }
        fputs(recvbuf, stdout);
        writen(conn, &recvbuf, strlen(recvbuf));
    }
}
#include <sys/wait.h>
void handle_sigchld(int signal_id)
{
//    wait(NULL);
    while(waitpid(-1, NULL, WNOHANG) > 0);
    /*
     *首先，第一次想到while循环还可以这么写。
     *第二， 推荐使用这种方式来处理多进程处理连接推出。
     *因为使用该种方式，此处代码收缩性更好，可以在此处处理其他更多的事情。
     */
//    waitpid(-1, NULL, WNOHANG);
}


int main(int argc, char* argv[])
{
    /* signal(SIGCHLD, SIG_IGN); //忽略僵尸信号进程    */
    signal(SIGCHLD, handle_sigchld);
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
    int on = 1;
    if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) //设置地址重复利用。 TIME_WAIT状态下依然可以连接
        ERR_EXIT("setsockopt");
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
    while(1)
    {
        if((conn = accept(listenfd, (struct sockaddr*)& peer_addr, &peer_len)) < 0)
            ERR_EXIT("accept");
        /* accept 会从已经完成连接的队列头返回一个套接字，如果队列为空，则accept函数阻塞
         * accept 执行成功后，会将该套接字从队列中移除，以便更多客户端可以连接过来  */
        /*  conn 通常被称为已经连接套接字，主动套接字  */
        printf("ip = %s , port= %d .\n", inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port));
        pid_t pid = fork();
        if(pid == -1)
            ERR_EXIT("fork");
        if(pid == 0)
        {
            close(listenfd); // 子进程不需要处理监听
            echo_serv(conn);
            exit(EXIT_SUCCESS);
        }
        else
            close(conn);  // 父进程不需要处理连接
    }
    close(listenfd);
    close(conn);
    return 0;
}
