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
#include <netdb.h>

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
    char host[100] ={0};
    memset(host, 00, 100);
    if (gethostname(host, sizeof(host)) < 0)
        ERR_EXIT("gethostname");
    printf(" host is %s .\n", host);
    struct hostent *hp;
    if ((hp = gethostbyname(host)) == NULL)
        ERR_EXIT("gethostbyname");
    int i = 0;
    while( hp->h_addr_list[i] != NULL)
    {
        printf(" addr is %s .\n", inet_ntoa(*(struct in_addr*) hp->h_addr_list[i]));
        ++i;
    }
}
