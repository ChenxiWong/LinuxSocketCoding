// Last Update:2017-05-16 20:50:09
/**
 * @file blpoint.c
 * @brief 
 * @author wangchenxi
 * @version 0.1.00
 * @date 2017-05-16
 */
#include<stdio.h>
#include <arpa/inet.h>
int main(int argc, char*argv[])
{
    unsigned long addr = inet_addr("192.168.0.100"); // 注意得到的字节序号是一个网络字节序
    printf("addr = %u\n", ntohl(addr)); //将网络字节序号数字转换成主机字节序号

    struct in_addr ipaddr;
    ipaddr.s_addr = addr;
    printf("%s\n", inet_ntoa(ipaddr));
    return 0;
}

/*
 * 对应的函数：
 *              htonl()                  inet_ntoa()
 *           -------------->           -------------->
 * 主机字节序                网络字节序               点分字符串
 *           <--------------           <--------------
 *              ntohl()                   inet_addr()
 *
 */
