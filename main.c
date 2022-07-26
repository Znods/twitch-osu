#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include <sys/socket.h>
#include <sys/types.h>

#define CHANNEL "<channel>"
#define BOTNAME "<botname>"
#define TOKEN "<oauth token>"

#define BUFSIZE 15000

#define FATAL(x) perror(x), exit(EXIT_FAILURE)

int init_twitch(char *channel, char *botname, char *oauth);
void login_irc(int fd, char *channel, char *botname, char *oauth);
void ping_check(int fd, char *twitch_chat);
void parse_chat(char *twitch_chat, char *buffer);
void irc_commands(int fd, char *chat, char *channel);

int main(){
    char *twitch_chat = (char *)malloc(BUFSIZE * sizeof(char));
    char *buffer = (char *)malloc(BUFSIZE * sizeof(char));
    bool running = true;
    int fd = 0;
    if(twitch_chat == NULL){
        FATAL("malloc()");
    }
    /* Create socket & connect to irc.chat.twitch.tv:6667 #channel */
    fd = init_twitch(CHANNEL, BOTNAME, TOKEN);
    /* Verify bot with irc.twitch.tv */
    login_irc(fd, CHANNEL, BOTNAME, TOKEN);

    /* Main Logic Loop */
    while(running){
        memset(twitch_chat, '\0', BUFSIZE);
        recv(fd, twitch_chat, BUFSIZE, 0);
        /* Parse Twitch Chat */
        parse_chat(twitch_chat, buffer);
        /* Check for Commands */
        irc_commands(fd, buffer, CHANNEL);
        /* Write Twitch Chat to Console */
        write(1, buffer, strlen(buffer));
        /* Check for PING */
        ping_check(fd, twitch_chat);
    }

    free(twitch_chat);
    close(fd);
    return 0;
}

void parse_chat(char *twitch_chat, char *buffer){
    char *line = strtok(twitch_chat, "!");
    char *prev = '\0';
    memset(buffer, '\0', BUFSIZE);
    memmove(buffer, line, strlen(line) + 1);   
    while(line != NULL){
        prev = line;
        line = strtok(NULL, ":");
    }
    int sz = strlen(buffer);
    /* Concat String to Name */
    buffer[sz] = ':', buffer[sz + 1] = ' ';
    strcat(buffer, prev);
}

void login_irc(int fd, char *channel, char *botname, char *oauth){
    /* Wait */
    char pass[strlen(oauth)];//, *pass = '\0';
    char buffer[strlen(botname)];//, *buffer = '\0';
    char chan[strlen(channel)];//, *chan = '\0';
    char response[255];

    usleep(7500);
    sprintf(pass, "PASS oauth:%s\r\n", oauth);
    if(write(fd, pass, strlen(pass)) < 0){
        FATAL("write()");
    }

    usleep(7500);
    sprintf(buffer, "NICK %s\r\n", botname);
    if(write(fd, buffer, strlen(buffer)) < 0){
        FATAL("write()");
    }

    usleep(7500);
    sprintf(chan, "JOIN #%s\r\n", channel);
    if(write(fd, chan, strlen(chan)) < 0){
        FATAL("write()");
    }

    /* check for invalid information */
    memset(response, '\0', 255);
    if(recv(fd, response, 255, SOCK_NONBLOCK) < 0){
        FATAL("read()");
    }

    if(!strncmp(response, ":tmi.twitch.tv NOTICE * :Login authentication failed", 52)){
        printf("Login authentication failed.\n\n");
        exit(EXIT_FAILURE);
    }

    puts(response);

}

void ping_check(int fd, char *twitch_chat){
    if(!strncmp(twitch_chat, "PING", 4)){
        if(send(fd, "PONG :tmi.twitch.tv\r\n", strlen("PONG :tmi.twitch.tv\r\n"), 0) < 0){
            FATAL("send()");
        }
    }
}
