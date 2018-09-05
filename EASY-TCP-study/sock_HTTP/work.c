#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#define MAX 1024

static void usage(const char *proc)
{
	printf("Usage: %s port\n", proc);
}

static int getLine(int sock, char line[], int num)
{
	char c = 'x';
	int i = 0;
	while(c != '\n' && i < num - 1){
		ssize_t s = recv(sock, &c, 1, 0);
		if(s > 0){
			if(c == '\r'){
				recv(sock, &c, 1, MSG_PEEK);
				if(c == '\n'){
					recv(sock, &c, 1, 0);
				}else{
					c = '\n';
				}
			}

			// \r,\r\n, \n -> \n
			line[i++] = c;
		}else{
			break;
		}
	}
	line[i] = 0;
	return i;
}

void clearHeader(int sock)
{
	char line[MAX];
	do{
		getLine(sock, line, MAX);
	}while(strcmp(line, "\n" != 0));
}

void echoError(int sock, int status_code)
{
	switch(status_code){
		case 404:
		//	show_404(sock);
			break;
		defalt:
			break;
	}
}

void* handlerRequest(void *arg)
{
	int sock = (int)arg;
	char line[MAX];
	char method[MAX/16];
	char url[MAX];
	int i,j;
	int status_code = 200;
	int cgi = 0;

	getLine(sock, line, MAX);

	printf("line: %s\n", line);

	i=0;
	j=0;
	while(i < sizeof(method) - 1 &&\
			j < sizeof(line) &&\
			!isspace(line[j])){
		method[i] = line[j];
		i++, j++;
	}
	method[i] = 0;
	
	//Get, Post, gEt, pOst
	if( strcasecmp(method, "GET") == 0){
	}
	else if(strcasecmp(method, "POST") == 0){
		cgi = 1;
	}
	else{
		status_code = 404;
		clearHeader(sock);
		goto end;
	}


	while(j < sizeof(line) && isspace(line[j])){
		j++;
	}

	i = 0;
	while( i < sizeof(url) - 1 &&\
			j < sizeof(line) &&\
			!isspace(line[j])){
		url[i] = line[j];
		i++, j++;
	}
	url[i] = 0;



end:
	if(status_code != 200){
		echoError(sock, status_code);
	}

	close(sock);
}

int startup(int port)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0){
		perror("socket");
		exit(2);
	}

	int opt = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	local.sin_port = htons(port);

	if(bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0){
		perror("bind");
		exit(3);
	}

	if(listen(sock, 10) < 0){
		perror("listen");
		exit(4);
	}

	return sock;
}

// ./httpd 9999
int main(int argc, char *argv[])
{
	if(argc != 2){
		usage(argv[0]);
		return 1;
	}

	int listen_sock = startup(atoi(argv[1]));

	for( ; ; ){
		struct sockaddr_in client;
		socklen_t len = sizeof(client);
		int sock = accept(listen_sock, (struct sockaddr*)&client, &len);
		if(sock < 0){
			perror("accept");
			continue;
		}

		printf("get a new connect...!\n");
		pthread_t tid;
		pthread_create(&tid, NULL, handlerRequest, (void *)sock);
		pthread_detach(tid);
	}

	return 0;
}
