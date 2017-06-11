# Last Update:2017-06-12 00:21:42
#########################################################################
# File Name: compliter.sh
# Author: wangchenxi
# mail: chinawangchenxi@gmail.com
# Created Time: 2017年06月12日 星期一 00时21分42秒
#########################################################################
#!/bin/bash 
rm -rf *.lis *.c *.log $1
proc iname=$1.pc
gcc $1.c -I $ORACLE_INCLUDE_PATH  -L/opt/oracle_client/lib/ -lclntsh -o $1
