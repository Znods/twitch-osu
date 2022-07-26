#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#define DOMAIN "irc.chat.twitch.tv"

#define FATAL(x) perror(x), exit(EXIT_FAILURE)

int init_twitch(char *channel, char *botname, char *oauth){
    struct sockaddr_in serv = {0};
    struct hostent *host;
    serv.sin_family = AF_INET;
    serv.sin_port = htons(6667);

    if((host = gethostbyname(DOMAIN)) < 0){
        FATAL("gethostbyname()");
    }

    memmove(&serv.sin_addr.s_addr, host->h_addr_list[0], sizeof(host->h_length));
    
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0){
        FATAL("socket()");
    }

    if(connect(fd, (struct sockaddr *)&serv, (socklen_t)sizeof(serv)) < 0){
        FATAL("connect()");
    }

    return fd;
}
