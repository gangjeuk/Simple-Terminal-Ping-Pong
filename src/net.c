#include <sys/types.h>
#include <string.h> // memset
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#define BUF_LEN 10000

#include <game.h>
#include <key.h>

int tcp_to_connect(int host, int port)
{
    int s;
    struct sockaddr_in serv_addr;
    s = socket(AF_INET, SOCK_STREAM, 0);
    if(s == -1)
    {
        printf("socket creation failed...\n");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(struct sockaddr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(host);
    serv_addr.sin_port = htons(port);

    if(connect(s, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0)
    {
        printf("connect failed\n");
        exit(EXIT_FAILURE);
    }
    printf("connected!\n");

    return s;
}

// set to listen status
int tcp_to_listen(int port)
{
    int s;
    struct sockaddr_in serv_addr;
    s = socket(AF_INET, SOCK_STREAM, 0);
    if(s == -1)
    {
        printf("socket creation failed...\n");
        exit(EXIT_FAILURE);
    }
    
    memset(&serv_addr, 0, sizeof(struct sockaddr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    if(bind(s, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in)) == -1)  
    {
        printf("bind failure\n");
        exit(EXIT_FAILURE);
    }

    listen(s, 1);

    return s;
}

int client(int host, int client)
{
    struct timeval tv = {0L, 0L};
    char buf[BUF_LEN];
    fd_set readfds, writefds;
    int serv_sock;
    int nfds, nread;


    serv_sock = tcp_to_connect(host, client);
    nfds = serv_sock + 1;

    while(1){
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(serv_sock, &writefds);
        if(select(nfds, &readfds, &writefds, NULL, &tv) < 0)
        {
            printf("select error\n");
            exit(EXIT_FAILURE);
        }

    
        // if something on server
        // ready to get message from server
        if(FD_ISSET(serv_sock, &writefds))
        {

            if((nread = recv(serv_sock, buf, BUF_LEN, 0)) > 0)
            {
                // receive canvas and print
                printf("\033[2J"); 

                int x, y;
                for (y = 0; y < MULTIPLAY_HEIGHT; y++)
                {
                    for (x = 0; x < MULTIPLAY_WIDTH; x++)
                        printf("%c", buf[ y*MULTIPLAY_WIDTH + x]);
                    printf("\r\n");
                }                    }
            memset(buf, 0, BUF_LEN);
        }
    
        
        // if something on your buffer(when you wrote something) 
        // send your message to server
        if(FD_ISSET(STDIN_FILENO, &readfds))
        {
            char c;
            if(read(STDIN_FILENO, &c, sizeof(c)) > 0)
            {
                buf[0] = c;
                buf[BUF_LEN - 1] = '\n';   
                if (send(serv_sock, buf, sizeof(buf), 0) < 0)
                    printf("sending error!\n ");
            }
        }

        

    }
}
