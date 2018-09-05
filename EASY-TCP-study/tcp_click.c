#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<error.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<string.h>
int main(int argc,char *argv[])
{
    int sockfd=-1;
    if(argc!=3)
    {
        printf("USAGE :./tcp_server");
        return -1;
    }
    sockfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(sockfd<0)

    {
        perror("socket error");
        return -1;

    }
    struct sockaddr_in serv_addr;
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(atoi(argv[2]));
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    socklen_t len=sizeof(struct sockaddr_in);
    int ret=connect(sockfd,(struct sockaddr*)&serv_addr,len);
    if(ret<0)
    {
        perror("connect error");
        return -1;
    }
    while(1)
    {
        printf("client say");
        fflush(stdout);
        char buff[1024]={0};
        scanf("%s",buff);
        send(sockfd,buff,strlen(buff),0);
        //接收服务器端的回复
        char tmp[1024]={0};
        recv(sockfd,tmp,1023,0);
        printf("server say:%s\n",tmp);

    }
    close(sockfd);
    return 0;
}
