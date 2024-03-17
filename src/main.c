#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#include <game.h>
#include <key.h>
#include <net.h>

int main(int argc, char **args) {
    // Get delay time from command line
    char buf[500];
    char *delay_str;
    long delay_time = DEFAULT_TERMINAL_REFRESH;
    if(argc > 1)
        delay_time = strtol(args[1], &delay_str, 10);

    while(1) {
        printf("\033[2J");
        printf("Tetris CLI by Dylan Turner cerca 2020.\n\n");
        printf("Controls:\n");
        printf(" - a/d -> move left/right respectively\n");
        printf(" - q/e -> rotate left/right\n");
        printf(" - s   -> drop piece quickly\n");
        printf(" - backspace -> quit game\n");
        printf(" - o -> single play\n");
        printf(" - m -> multi wait for connect\n");
        printf(" - c -> connect to others\n");
        printf("\nWhen starting the app, you can give a\n");
        printf("different delay time (in microseconds)\n");
        printf("to adjust for flicker\n");
        printf("\nDefault delay = %d\n", DEFAULT_TERMINAL_REFRESH);
        printf("Current delay = %ld\n", delay_time);
        printf("Recommended to keep new delay w/in a power of 10\n");
        printf("\nPress enter to begin...\n\n\n\n\n\n\n");

        set_conio_terminal_mode();

        int key = 0;
        int multiplay = false;
        while(key != 13) {
            while(!keyboard_hit(0));
            key = get_char(0);

            if(key == 127)
                goto quit;
            else if(key == KEY_MULTIPLAY){
                multiplay = true;
                printf("Waiting for connect ...\n");

                break;                
            }else if(key == KEY_CONNECT){
                printf("please input IP\n");
                scanf("%s", buf);
                client(atoi(buf), PORT_NUM);
                goto quit;
            }
        }

        play(delay_time, multiplay);
        reset_terminal_mode();
    }

    quit:
        reset_terminal_mode();

    return 0;
}

