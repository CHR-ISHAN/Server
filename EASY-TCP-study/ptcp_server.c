#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<errno.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<string.h>
#include<sys/wait.h>
/*基于TCP的一个多进程简单聊天程序
 * 1、在接收一个新客户端连接后，启动一个字进程处理客户端发来的请求（聊天信息）
 */
void handle(int sockfd,struct sockaddr_in addr)
{
    while(1)
    {
        char buff[1024]={0};
        int ret=recv(sockfd,buff,1023,0);
        if(ret<=0)
        {
            if(errno==EINTR||perror==EAGAIN)
            {
                usleep(1000);
                continue;
            }
            close(sockfd);
            exit(0);
        }
        printf("client:[%s:%d] say:%s\n",inet_ntoa(addr.sin_addr),ntohs(addr.sin_port),buff);
    send(sockfd,"i am robot~!!!!",14,0);
    }
    close(sockfd);
    return;
}
int create_worker(int sockfd,struct sockaddr_in addr)
{
    int pid=-1;
    pid=fork();
    if(pid<0)
    {
        close(sockfd);
        return -1;
    }
    else if(pid==0)
    {//启动字进程来处理客户端的聊天
        //如果字进程一直不退出，则父进程会一直等待它
        //我们就让子进程再创建一个孙子进程来处理聊天，而自己退出
        //这样的话，父进程的等待就会直接返回
        //而孙子进程处理完毕聊天后，会退出，资源被init进程回收
        if(fork()==0)

        {
            handle(sockfd,addr);
        }
        exit(0);
    }

    else{
        close(sockfd);
        wait(NULL);

    }
    return 0;
}
int main(int argc,char *argv[])
{
    int lst_fd=-1;
    if(argc!=3)
    {
        printf("USAGE :./ptcp_server ip port\n");
        return -1;
    }
    lst_fd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(lst_fd<0)
    {
        perror("socket error");
        return -1;

    }
    struct sockaddr_in lst_addr;
    lst_addr.sin_family=AF_INET;
    lst_addr.sin_port=htons(atoi(argv[2]));
    lst_addr.sin_addr.s_addr=inet_addr(argv[1]);
    socklen_t len=sizeof(struct sockaddr_in);
    int ret=bind(lst_fd,(struct sockaddr*)&lst_addr,len);
    if(ret<0)
    {
        perror("bind error");
        return -1;
    }
    if(listen(lst_fd,5)<0)
    {
        perror("listen error" );
        return -1;
    }
    while(1)
    {
        int newfd=-1;
        struct sockaddr_in cli_addr;
        newfd =accept(lst_fd,(struct sockaddr*)&cli_addr,&len);
        if(newfd<0)
        {
            perror("accept error");
            continue;
        }
        create_worker(newfd,cli_addr);
        

    }
}
