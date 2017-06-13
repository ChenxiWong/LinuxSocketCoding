// Last Update:2017-06-13 11:41:44
/**
 * @file aboutPrintf.c
 * @brief 
 * @author wangchenxi
 * @version 0.1.00
 * @date 2017-06-13
 */
#include <stdio.h>

int main(int argc, char* argv[])
{
    int i = 0;
    char* str="123456789";
    for(; i < 12; ++i)
        printf("%.*s\n", i, str);
    return 0;
}
