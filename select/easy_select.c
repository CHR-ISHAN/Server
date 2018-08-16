/* 实现一个简单的标准输入监控程序，从标准输入拿到数据显示到终端
 *  目的：熟悉一下select用法和功能
 *  select功能是对描述符集合中的描述符进行状态改变监控
 *  当集合中有描述符就绪时，将返回
 *  或者当select等待超时时，将返回
 *  或者当select等待出错时，将返回
 *  int select(int nfds, fd_set readfds, writefds, exceptfds, tv);
 *  nfds:监控的最大描述符+1
 *  readfds：监控可读的描述符集合
 *  writefds：监控可写的描述符集合
 *  exceptfds：监控异常的描述符集合
 *  tv：select是一个阻塞调用，但是可以设置阻塞的时间
 *      NULL：  一直阻塞
 *      0：     非阻塞
 *      >0:     在指定在这段时间内如果没有描述符就绪，则返回0，超时
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/select.h>

int main()
{
    fd_set readfds;
    struct timeval tv;
    int max_fd = 0;

    while(1) {
        //设置select的等待超时时间
        tv.tv_sec = 3;
        tv.tv_usec = 0;
        //清空可读事件集合
        FD_ZERO(&readfds);
        //将标准输入的描述符加入集合中
        FD_SET(0, &readfds);
        int ret = select(max_fd+1, &readfds, NULL, NULL, &tv);
        if (ret < 0) {
            perror("select error");
            continue;
        }else if (ret == 0) {
            printf("timeout!!\n");
            continue;
        }
        //当集合中有描述符就绪，select会返回告诉我们，但是
        //select并不会告诉我们到底时哪一个描述符就绪了
        char  buff[1024] = {0};
        ret = read(0, buff, 1023);
        buff[ret - 1] = '\0';
        printf("buff:[%s]\n", buff);
    }
    return 0;
}

