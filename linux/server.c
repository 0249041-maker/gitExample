#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "caesar.h"
#define BUFSZ 1024

int main(int argc,char**argv){
    if(argc<3){fprintf(stderr,"Uso: %s <bind_ip> <port>\n",argv[0]);return 1;}
    const char* ip=argv[1]; int port=atoi(argv[2]);
    int s=socket(AF_INET,SOCK_STREAM,0); if(s<0){perror("socket");return 1;}
    int opt=1; setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    struct sockaddr_in a={0}; a.sin_family=AF_INET; a.sin_port=htons(port);
    inet_pton(AF_INET,ip,&a.sin_addr);
    if(bind(s,(struct sockaddr*)&a,sizeof(a))<0){perror("bind");return 1;}
    if(listen(s,1)<0){perror("listen");return 1;}
    printf("Server en %s:%d\n",ip,port);
    struct sockaddr_in cli; socklen_t cl=sizeof(cli);
    int c=accept(s,(struct sockaddr*)&cli,&cl); if(c<0){perror("accept");return 1;}
    uint8_t buf[BUFSZ]; ssize_t n=recv(c,buf,sizeof(buf),0);
    if(n>0){ char plain[BUFSZ]; caesar_decrypt_bytes(buf,(size_t)n,plain,sizeof(plain));
        printf("Bytes: %zd | Descifrado: %s\n",n,plain);
        send(c,buf,(size_t)n,0);
    }
    close(c); close(s); return 0;
}
