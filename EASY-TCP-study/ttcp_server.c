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
#include<pthread.h>
/*
 * 基于TCP的一个多线程简单网络聊天程序
 * 
 */
void *handle(void *arg)
{
    int sockfd=(int)arg;
    pthread_t tid=pthread_self();
    while(1)
    {
        char buff[1024]={0};
        int ret=recv(sockfd,buff,1023,0);
        if(ret<0)
        {
            break;
        }
        printf("client :%lu say:%s\n",tid,buff);
        send(sockfd,"i don't know!!",14,0);
    }
    close(sockfd);
    return NULL;
}
int create_robot(int sockfd)
{
    pthread_t tid;
    pthread_create(&tid,NULL,handle,(void*)sockfd);
    pthread_detach(tid);
    return 0;
}

int main(int argc,char *argv[])
{
    int lstfd=-1;
    lstfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(lstfd<0)
    {
        perror("socket error");
        return -1;
    }
    struct sockaddr_in lst_addr;
    lst_addr.sin_family=AF_INET;
    lst_addr.sin_port=htons(atoi(argv[2]));
    lst_addr.sin_addr.s_addr=inet_addr(argv[1]);
    socklen_t len=sizeof(struct sockaddr_in);
    int ret=bind(lstfd,(struct sockaddr*)&lst_addr,len);
    if(ret<0)
    {
        perror("bind error");
        return -1;
    }
    if(listen(lstfd,5)<0)
    {
        perror("listen error");
        return -1;
    }
    while(1)
    {
        int newfd=-1;
        struct sockaddr_in cli_addr;
        newfd=accept(lstfd,(struct sockaddr*)&cli_addr,&len);
        if(newfd<0)
        {
            perror("accept error");
            continue;
        }
        create_robot(newfd);
    }

    return 0;
}
