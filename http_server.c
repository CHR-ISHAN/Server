/*
 * 简单的http服务器程序
 * 不管什么请求 ，都回复一个最简单的html界面
 * <html><body><p>hello world</p></body></html>
 * 传输层：TCP
 * TCP服务端程序：
 * 1、创建套接字
 * 2、绑定
 * 3、监听
 * 4、接收
 * 5、接收HTTP请求
 * 6、回复数据
 * http回复格式：首行
 *              请求头
 *              空行
 *              正文
 */
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
char *res_header1(int statu,char* context)
{
    static char buff[1024]={0};
    sprintf(buff,"HTTP/1.1 %d %s\r\n",statu,context);
    return buff;
}
char* res_header2(char *body)
{
    static char buff[1024]={0};
    sprintf(buff,"Content-Length:ld\r\n",strlen(body));
    sprintf(buff,"%s%s\r\n",buff,"Content-Type:text/html");
    //请求头与正文之间的空行间隔
    sprintf(buff,"%s%s",buff,"\r\n");
    return buff;


}
int main(int argc,char * argv[])
{
    int lst_fd,cli_fd,ret;
    struct sockaddr_in lst_addr;
    struct sockaddr_in cli_addr;
    if(argc!=3)
    {
        printf("USAGE:./http ip port\n");
        return -1;
    }
    lst_fd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(lst_fd<0)
    {
        perror("socket error");
        return -1;
    }
    lst_addr.sin_family=AF_INET;
    lst_addr.sin_port=htons(atoi(argv[2]));
    lst_addr.sin_addr.s_addr=inet_addr(argv[1]);
   socklen_t len =sizeof(struct sockaddr_in);
   ret=bind(lst_fd,(struct sockaddr*)&lst_addr,len);
   if(ret<0)
   {
       perror("listen error");
       return -1;
   }
    if(listen(lst_fd,5)<0)
    {
        perror("listen error");
        return -1;
    }
    while(1)
    {
        cli_fd=accept(lst_fd,(struct sockaddr*)&cli_addr,&len);
        if(cli_fd<0)
        {
            perror("accept error");
            continue;
        }
        char buff[1024]={0};
        ret=recv(cli_fd,buff,1023,0);
        if(ret<=0)
        {
            close(cli_fd);
            continue;
        }
        char body[1024]="<html><body><p>hello world</p></body></html>";
        memset(buff,0x00,1024);
        strcpy(buff,"HTTP/1.1 200\r\n");
        send(cli_fd,buff,strlen(buff),0);
        memset(buff,0x00,1024);
        sprintf(buff,"%s","LOCATION:http://www.baidu.com\r\n");
        send(cli_fd,buff,strlen(buff),0);
        memset(buff,0x00,1024);
       sprintf(buff,"Content-Length:%ld\r\n",0);
       send(cli_fd,buff,strlen(buff),0);
       send(cli_fd,"\r\n",strlen("\r\n"),0);
       send(cli_fd,body,strlen(body),0);
       close(cli_fd);


    }
    close(lst_fd);
    return 0;

}
