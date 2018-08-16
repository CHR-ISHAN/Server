/*  使用select实现单线程多客户端聊天机器人服务端程序
 *  
 *  
 *
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

int main(int argc, char *argv[])
{
    int lst_fd, cli_fd;
    int i, ret, max_fd, j;
    socklen_t   len;
    struct sockaddr_in lst_addr;
    struct sockaddr_in cli_addr;
    fd_set  readfds;
    int fd_list[1024];
    struct timeval tv;

    if (argc != 3) {
        printf("Usage: ./tcp_select ip port\n");
        return -1;
    }
    lst_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (lst_fd < 0) {
        perror("socket error");
        return -1;
    }
    lst_addr.sin_family = AF_INET;
    lst_addr.sin_port = htons(atoi(argv[2]));
    lst_addr.sin_addr.s_addr = inet_addr(argv[1]);
    len = sizeof(struct sockaddr_in);
    ret = bind(lst_fd, (struct sockaddr*)&lst_addr, len);
    if (ret < 0) {
        perror("bind error");
        return -1;
    }
    if (listen(lst_fd, 5) < 0) {
        perror("listen error");
        return -1;
    }
    for (i = 0; i < 1024; i++) {
        fd_list[i] = -1;
    }
    fd_list[0] = lst_fd;
    while(1) {
        tv.tv_sec = 3;
        tv.tv_usec = 0;
        max_fd = lst_fd;
        FD_ZERO(&readfds);
        for (i = 0; i < 1024; i++) {
            //循环将所有已经存在的描述符添加到监控集合中
            if (fd_list[i] != -1) {
                FD_SET(fd_list[i], &readfds);
            }
            //从数组中找出最大的描述符
            if (max_fd < fd_list[i]) {
                max_fd = fd_list[i];
            }
        }
        //开始select监控
        ret = select(max_fd+1, &readfds, NULL, NULL, &tv);
        if (ret < 0) {
            perror("select error");
            continue;
        }else if (ret == 0) {
            printf("timeout\n");
            continue;
        }
        for(i = 0; i < 1024; i++) {
            if (fd_list[i] == -1) {
                continue;
            }
            //select啊返回前将readfds中未就绪的描述符全都移除了
            //也就意味着readfds中现在还有的描述符都是就绪的
            //因为fd_list数组中保存了所有的描述符
            //因此拿fd_list中的每一个描述符判断是否还在readfds中
            //来确定到底时哪一个描述符就绪
            if (FD_ISSET(fd_list[i], &readfds)) {
                //当就绪的描述符是监听描述符，则代表有新连接到来
                if (fd_list[i] == lst_fd) {
                    cli_fd = accept(lst_fd, 
                            (struct sockaddr*)&cli_addr, &len);
                    //这个新的连接，也需要select来监控，因此我们把
                    //这个描述符先添加到数组中，等下次循环的时候就
                    //自动添加到集合中了
                    for(j = 0; j < 1024; j++) {
                        if (fd_list[j] == -1) {
                            fd_list[j] = cli_fd;
                            break;
                        }
                    }
                }else {
                    char buff[1024] = {0};
                    ret = recv(fd_list[i], buff, 1023, 0);
                    if (ret <= 0) {
                        if (errno == EINTR) {
                            continue;
                        }
                        close(fd_list[i]);
                        fd_list[i] = -1;
                    }
                    printf("client say:%s\n", buff);
                    send(fd_list[i], "what\?\?\?\?!!", 10, 0);
                }
            }
        }
    }
    return 0;
}
