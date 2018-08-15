/*
 * 基于TCP的一个网络版计算器--服务端
 *  1. 接受客户端传输过来的结构化数据，从结构化数据中解析出
 *      要进行计算的两个数字和运算符
 *  2. 进行计算，向客户端返回结果
 *
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
//客户端的请求数据结构，也就是说，服务端对接受到的数据，将使用
//这个结构来进行解析，解析出要运算的数据和运算符
//这个结构体就是对数据格式的一个组织与解析的约定
typedef struct _req_t{
    int num1;
    int num2;
    char sym;
}req_t;
//服务器端对客户端的运算结果返回数据结构
typedef struct _rsp_t{
    int res;
}rsp_t;

rsp_t cal_robot(char *buff)
{
    //数据解析，并进行计算
    rsp_t res;
    req_t *req = (req_t *)buff;

    if (req->sym == '+') {
        res.res = req->num1 + req->num2;
    }else if (req->sym == '-') {
        res.res = req->num1 - req->num2;
    }else if (req->sym == '*') {
        res.res = req->num1 * req->num2;
    }else if (req->sym == '/') {
        res.res = req->num1 / req->num2;
    }
    return res;
}
int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Usage: ./cal_server ip port\n");
        return -1;
    }
    //1. 创建监听套接字
    int lst_fd = -1;
    lst_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (lst_fd < 0) {
        perror("socket error");
        return -1;
    }
    //2. 绑定监听地址
    struct sockaddr_in lst_addr;
    lst_addr.sin_family = AF_INET;
    lst_addr.sin_port = htons(atoi(argv[2]));
    lst_addr.sin_addr.s_addr = inet_addr(argv[1]);
    socklen_t len = sizeof(struct sockaddr_in);
    int ret = bind(lst_fd, (struct sockaddr*)&lst_addr, len);
    if (ret < 0) {
        perror("bind error");
        return -1;
    }
    //3. 开始监听
    if (listen(lst_fd, 5) < 0) {
        perror("listen error");
        return -1;
    }
    while(1) {
        //4. 接收一个已经成功的连接，返回一个新的socket
        int newfd = -1;
        struct sockaddr_in cli_addr;
        newfd = accept(lst_fd, (struct sockaddr*)&cli_addr, &len);
        if (newfd < 0) {
            perror("accept error");
            return -1;
        }
        //5. 使用这个socket进行通信，接收客户端的业务请求
        //      接收数据，完成业务请求：
        //          数据计算
        char buff[1024] = {0};
        recv(newfd, buff, 1023, 0);
        rsp_t res;
        res = cal_robot(buff);
        //6. 向客户端返回结果
        send(newfd, (void*)&res, sizeof(res), 0);
        //7. 关闭socket
        close(newfd);
    }
    return 0;
}
