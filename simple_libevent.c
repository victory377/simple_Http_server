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

void webserver(int sock, short event, void *arg) {
    struct event *ev=arg;
    char data_to_send[1024];
    int fd,bytes_read;
    char buf[1024],path[1024];
    char *ROOT;
    ROOT = getenv("PWD");
    char *req[3];
    char test[12]="/index.html";
    recv(sock, buf, sizeof(buf),0);
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
            //if( strncmp(req[1], "/\0",2) == 0)
             //  test= "/index.html";
            strcpy(path,ROOT);
            strcpy(&path[strlen(ROOT)],test);
            printf("Path:%s\n",path);
            if ( (fd=open(path, O_RDONLY))!=-1 )    //FILE FOUND
            {
                send(sock, "HTTP/1.0 200 OK\n\n", 17, 0);
                while ( (bytes_read=read(fd, data_to_send, 1024))>0 ) {
                    write (sock, data_to_send, bytes_read);
                    event_del(ev);
                }
                printf("Data:%s\n",data_to_send);
            }
            else    write(sock, "HTTP/1.0 404 Not Found\n", 23); //FILE NOT FOUND
        }
    }
    close(sock);
}

static void sock_connect(int fd, short event, void *argv) {
    struct event *ev;
    struct sockaddr_storage users;
    int nw_sk;
    socklen_t addr_size;

    addr_size = sizeof users;
    nw_sk = accept(fd, (struct sockaddr *) &users, &addr_size);
    if (nw_sk < 0)
    {
        printf("accept error\n");
    }
    ev=malloc(sizeof(struct event));
    event_set(ev, nw_sk, EV_READ, webserver, ev);
    event_add(ev, NULL);
}

int main() {
    struct addrinfo hints, *res, *p;
    struct event ev;
    int skfd, nw_sk, status;
    pthread_t user[10];

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

    event_init();
    event_set(&ev, skfd, EV_READ|EV_PERSIST, sock_connect, &ev);
    event_add(&ev, NULL);
    event_dispatch();
    close(skfd);
    return 0;
}
