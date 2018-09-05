#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<error.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<string.h>
/*
 *基于TCP的一个简单网络聊天程序
 *1、创建套接字
 *2、为套接字绑定地址
 *3、开始监听套接字
 *4、接收连接成功的新socket（sip sport dip dport）
 *5、收发数据
 *6、关闭socket
 */
int main(int argc,char *argv[])
{
    int lst_sockfd=-1;
    if(argc!=3)
    {
        printf("USAGE:./tcp_server ip port\n");
        return -1;
    }
    //创建一个拉皮条的socket-----监听socket
    lst_sockfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(lst_sockfd<0){
        perror("socket error");
        return -1;
    }
    struct sockaddr_in lst_addr;
    lst_addr.sin_family=AF_INET;
    lst_addr.sin_port=htons(atoi(argv[2]));
    lst_addr.sin_addr.s_addr=inet_addr(argv[1]);
    socklen_t len=sizeof(struct sockaddr_in);
    int ret=bind(lst_sockfd,(struct sockaddr *)&lst_addr,len);
    if(ret<0)
    {
        perror("listen error");
        return -1;
    }
    if(listen(lst_sockfd,5)<0)
    {
        printf("listen error");
        return -1;
    }

        while(1)
        {
            int new_fd;
            struct sockaddr_in cli_addr;
            new_fd=accept(lst_sockfd,(struct sockaddr* )&cli_addr,&len);
            if(new_fd<0)
            {
                perror("accpet error");
                continue;
            }
            //接收连接客户端的数据
            //这个新的连接里已经定义好了数据通信的源地址端口和目的地址端口
            //因此在接收或发送数据的时候，就不需要重新确定了
            char buff[1024]={0};
            recv(new_fd,buff,1023,0);
            printf("client:%s:%d say:%s\n",inet_ntoa(cli_addr.sin_addr),
                    ntohs(cli_addr.sin_port),buff );
            printf("server say");
            fflush(stdout);
            char tmp[1024]={0};
            scanf("%s",tmp);
            send(new_fd,tmp,strlen(tmp),0);

        }
        close(lst_sockfd);
        return 0;
}

