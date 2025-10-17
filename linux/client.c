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
    if(argc<4){fprintf(stderr,"Uso: %s <server_ip> <port> <mensaje>\n",argv[0]);return 1;}
    const char* ip=argv[1]; int port=atoi(argv[2]); const char* msg=argv[3];
    int s=socket(AF_INET,SOCK_STREAM,0); if(s<0){perror("socket");return 1;}
    struct sockaddr_in a={0}; a.sin_family=AF_INET; a.sin_port=htons(port);
    inet_pton(AF_INET,ip,&a.sin_addr);
    if(connect(s,(struct sockaddr*)&a,sizeof(a))<0){perror("connect");return 1;}
    uint8_t pkt[BUFSZ]; size_t n=caesar_encrypt_bytes(msg,5,pkt,sizeof(pkt));
    send(s,pkt,n,0);
    uint8_t rep[BUFSZ]; ssize_t r=recv(s,rep,sizeof(rep),0);
    if(r>0){ char plain[BUFSZ]; caesar_decrypt_bytes(rep,(size_t)r,plain,sizeof(plain));
        printf("Respuesta %zd | Descifrado: %s\n",r,plain);
    }
    close(s); return 0;
}
