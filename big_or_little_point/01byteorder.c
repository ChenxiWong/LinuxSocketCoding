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
    unsigned int x = 0x12345678;
    unsigned char* p = (unsigned char*) &x;
    printf("%0x %0x %0x %0x\n", p[0], p[1], p[2], p[3]);
    if(p[0] == 0x78)
    {
        printf("little ending.\n");
    }else if( p[0] == 0x12)
    {
        printf("big ending.\n");
    }else
    {
        printf("error occured.\n");
    }
    /*    */
    unsigned int y = htonl(x);
    p = (unsigned char*) &y;
    printf("%0x %0x %0x %0x\n", p[0], p[1], p[2], p[3]);
    return 0;
}
