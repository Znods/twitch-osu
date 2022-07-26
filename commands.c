#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "oppai/oppai.c"

#define OWNER ":znods:"

void irc_commands(int fd, char *chat, char *channel){
    char arg2[10] = {'\0'}, arg4[10] = {'\0'};
    float arg3 = 0;
    int arg1 = 0;
    
    char username[50] = {'\0'};
    char command[10] = {'\0'};
    char privmsg[500] = {'\0'};
    char request[500] = {'\0'};
    char pp[50] = {'\0'};
    
    ezpp_t ez = ezpp_new();
    
    sscanf(chat, "%s %s %d %s %f %s", username, command, &arg1, arg2, &arg3, arg4);
    
    /* !pp <beatmap_id> */
    if(!strncmp("!pp", command, 3)){

        if(!strncmp("+hrhddt", arg2, 7) || !strncmp("+hrdthd", arg2, 7) || !strncmp("+hdhrdt", arg2, 7) || !strncmp("+hddthr", arg2, 7) || !strncmp("+dthdhr", arg2, 7) || !strncmp("+dthrhd", arg2, 7)){
            ezpp_set_mods(ez, MODS_HR | MODS_DT | MODS_HD);
        } else if(!strncmp("+hrhd", arg2, 5) || !strncmp("+hdhr", arg2, 5)){
            ezpp_set_mods(ez, MODS_HR | MODS_HD);
        } else if(!strncmp("+hrdt", arg2, 5) || !strncmp("+dthr", arg2, 5)){
            ezpp_set_mods(ez, MODS_HR | MODS_DT);
        } else if(!strncmp("+dthd", arg2, 5) || !strncmp("+hddt", arg2, 5)){
            ezpp_set_mods(ez, MODS_HD | MODS_DT);
        } else if(!strncmp("+dt", arg2, 3)){
            ezpp_set_mods(ez, MODS_DT);
        } else if(!strncmp("+hd", arg2, 3)){
            ezpp_set_mods(ez, MODS_HD);
        } else if(!strncmp("+hr", arg2, 3)){
            ezpp_set_mods(ez, MODS_HR);
        } 

        if(arg3 > 0.0){
            ezpp_set_accuracy_percent(ez, arg3);
        }

        sprintf(request, "curl -s -X GET https://osu.ppy.sh/osu/%d -o map.osu", arg1);
        system(request);
        ezpp(ez, "map.osu");
        //printf("%gpp\n", ezpp_pp(ez));
        sprintf(pp, "PRIVMSG #%s :%gpp\r\n", channel, ezpp_pp(ez));
        write(fd, pp, strlen(pp));
        remove("map.osu");
    }

    /* !kill - kills the bot completely */
    if(!strncmp("!kill", command, 5) && !strncmp(OWNER, username, 7)){
        puts("Killing...");
        sprintf(privmsg, "PRIVMSG #%s :Killing...\r\n", channel);
        write(fd, privmsg, strlen(privmsg));
        free(chat);
        close(fd);
        exit(EXIT_SUCCESS);
    }
}