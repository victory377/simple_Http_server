#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <event.h>

void webserver(int sock,fd_set *web){
    char data_to_send[1024];
    int fd,bytes_read,rev;
    char buf[1024],path[1024];
    char *ROOT;
    ROOT = getenv("PWD");
    char *req[3];
    char test[12]="/index.html";
    memset(buf, 0, sizeof(buf));
    memset(path, 0, sizeof(path));
    memset(data_to_send, 0, sizeof(data_to_send));

    rev=recv(sock, buf, sizeof(buf),0);
    if( rev == -1)
        printf("shut down\n");
    else
    {
        printf("%s ",buf);
        req[0] = strtok(buf, " \t\n");
        if( strncmp(req[0], "GET\0", 4) == 0){
            req[1] = strtok(NULL, " \t");
            req[2] = strtok(NULL, " \t\n");
            if( strncmp(req[2], "HTTP/1.0",8)!=0 && strncmp(req[2], "HTTP/1.1",8)!=0 )
            {
                write(sock, "HTTP/1.0 400 Bad Request\n",25);
            }
            else{
                strcpy(path,ROOT);
                strcpy(&path[strlen(ROOT)],test);
                printf("Path:%s\n",path);
                if ( (fd=open(path, O_RDONLY))!=-1 )    //FILE FOUND
                {
                    send(sock, "HTTP/1.0 200 OK\n\n", 17, 0);
                    while ((bytes_read=read(fd, data_to_send, 1024))>0 ) {
                        write (sock, data_to_send, bytes_read);
                    }
                    printf("Data:%s\n",data_to_send);
                }
                else    write(sock, "HTTP/1.0 404 Not Found\n", 23); //FILE NOT FOUND
            }
        }
    }
    FD_CLR(sock, web);
    close(sock);
}

int main() {
    struct sockaddr_storage users;
    socklen_t addr_size;
    struct addrinfo hints, *res, *p;
    int skfd, nw_sk, status;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; //Automatively fill Host IP

    if((status = getaddrinfo(NULL,"10000", &hints, &res)) !=0)
        printf("error\n");
    for(p = res; p!= NULL; p=p->ai_next){
        if((skfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0 )
        {    
            printf("Socket Error\n");
            close(skfd);
        }
        if(bind(skfd, res->ai_addr, res->ai_addrlen)<0){
            printf("Bind Error\n");
            close(skfd);
            continue;
        }
            break;
    }
    if(listen(skfd, 10) < 0)
        printf("Listen Error\n");
    fd_set web;
    addr_size = sizeof users;
    while((nw_sk = accept(skfd, (struct sockaddr *) &users, &addr_size))!=-1){
        FD_ZERO(&web);
        FD_SET(nw_sk, &web);
        select(nw_sk+1, &web, NULL, NULL, NULL);
        webserver(nw_sk,&web);
    }
    close(skfd);
    return 0;
}
