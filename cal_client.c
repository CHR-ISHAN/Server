/*
 * 基于TCP的网络版计算器客户端
 *      将要运算的数据和运算符以指定的格式进行组织，
 *      将组织好的数据发送给服务器端
 *      等待服务器端返回的结果
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

typedef struct _req_t {
    int num1;
    int num2;
    char sym;
}req_t;
typedef struct _rsp_t{
    int res;
}rsp_t;
int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Usage: ./cal_client ip port\n");
        return -1;
    }
    //1. 创建套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        perror("socket error");
        return -1;
    }
    //2. 向服务端发起连接
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    socklen_t len = sizeof(struct sockaddr_in);
    int ret = connect(sockfd, (struct sockaddr*)&serv_addr, len);
    if (ret < 0) {
        perror("connect error");
        return -1;
    }
    while(1) {
        //3. 将要运算的数据和运算符以一定的格式进行组织
        req_t req;
        req.num1 = 55;
        req.num2 = 101;
        req.sym = '+';
        //4. 将组织好的数据发送到服务器端
        send(sockfd, (void*)&req, sizeof(req_t), 0);
        //5. 等待服务器端返回的结果进行解析展示
        rsp_t rsp;
        recv(sockfd, (void*)&rsp, sizeof(rsp_t), 0);
        printf("res:%d\n", rsp.res);
        break;
    }
    //6. 关闭socket、
    close(sockfd);
    return 0;
}
