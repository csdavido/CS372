/***********************************
** AUTHOR: DAVID RIDER
** TITLE: client.c
** CLASS: OSU CS 372
** LAST MODIFIED: NOVEMBER 3, 2019
** DESCRIPTION: INITIATES A CLIENT CHAT APPLICATION AND 
** INTERACTS WITH A SERVER
** CITATION: Beej's Guide
***********************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

/*****FUNCTION DECLARATIONS*****/
struct addrinfo* GetAddResults(char *address, char *port);
int CreatSocket(struct addrinfo *res);
int Connect(int sockFeed, struct addrinfo *res);
void Chat(int sockFeed, char *user, char *server);

/*****PROGRAM FLOW******/
int main(int args, char *arg[])
{
    if(args != 3) {
        printf("Error! Incorrect arguments...\n");
        exit(1);
    };
    //BUFFERS FOR HOLDING NAMES
    char user[10];
    char server[10];

    int isCorrectLength = 0;
    while (isCorrectLength == 0) {
        //PROMPT FOR NAME
        printf("Enter username: ");
        //GET NAME
        scanf("%s",user);
        //CHECK LENGTH
        if(strlen(user) > 10) {
            //IF NAME IS TOO LONG, REPEAT LOOP
            printf("Error! User name must be 10 characters or less...\n");
        } else {
            //ELSE, EXIT LOOP
            isCorrectLength = 1;
        };
    };
    
    //GET ADDRESS
    struct addrinfo *res = GetAddResults(arg[1], arg[2]);
    //CREATE SOCKET
    int sockFeed = CreatSocket(res);
    //CONNECT SOCKET
    Connect(sockFeed, res);
    //INTITIATE TCP CONNECTION
    send(sockFeed, user,strlen(user), 0);
    recv(sockFeed, server, 10, 0);
    //START CHAT
    Chat(sockFeed, user, server);
    //CLEAR ADDRESS
    freeaddrinfo(res);
    //EXIT
    return 0;
}
/*****HELPER FUNCTIONS*******/
/****************************************************
** NAME:
**    GetAddResults
** DESCRIPTION:
**    Gets the address information  
****************************************************/
struct addrinfo* GetAddResults(char *address, char *port) {
    struct addrinfo *result;
    struct addrinfo hints;
    
    memset(&hints,0,sizeof(hints));
    //SET TCP STREAM
    hints.ai_socktype = SOCK_STREAM;
    //SET VERSION
    hints.ai_family = AF_INET;
    
    int status = 0;
    //GET ADDRESS INFO
    if ((status = getaddrinfo(address, port, &hints, &result)) != 0) {
        printf("Address Error...\n");
        exit(1);
    };
    
    return result;
};
/****************************************************
** NAME:
**    GetAddResults
** DESCRIPTION:
**    Creates the socket
****************************************************/
int CreatSocket(struct addrinfo *res) {
    
    int sockFeed = 0;
    //MAKE THE SOCKET
    if((sockFeed = socket(res->ai_family,res->ai_socktype, res->ai_protocol)) == -1) {
        fprintf(stderr,"Socket Creation Error...\n");
        exit(1);
    };
    
    return sockFeed;
};
/****************************************************
** NAME:
**    GetAddResults
** DESCRIPTION:
**    Connects the socket 
****************************************************/
int Connect(int sockFeed, struct addrinfo *res) {
    
    int status = 0;
    //CONNECT THE SOCKET
    if ((status = connect(sockFeed,res->ai_addr,res->ai_addrlen)) == -1) {
        fprintf((stderr), "Socket Connection Error...\n");
        exit(1);
    };

    return status;
};
/****************************************************
** NAME:
**    Chat
** DESCRIPTION:
**    Handles the chat functionality
****************************************************/
void Chat(int sockFeed, char *user, char *server) {
    //BUFFERS TO HOLD MESSAGES
    char in[501];
    char out[501];
    int size = 0;
    int status;
    //FLUSH STDIN
    fgets(in, sizeof(in), stdin);
    while(1) {
        //CLEAR BUFFERS
        memset(in,0,sizeof(in));
        memset(out,0,sizeof(out));
        //PROMPT USER
        printf("%s> ", user);
        //GRAB INPUT
        fgets(in, 501, stdin);
        //QUIT PROGRAM IF 'quit' IS ENTERED
        if (strcmp(in, "\\quit\n") == 0) {
            break;
        };
        //GET SIZE OF THE INPUT MESSAGE
        size = send(sockFeed, in, strlen(in) ,0);
        //GET STATUS OF OUTPUT MESSAGE
        status = recv(sockFeed, out, 500, 0);
        //CHECK FOR ERRORS
        if (size == -1) {
            fprintf(stderr, "Sending Error...\n");
            exit(1);
        };
        
        if (status == -1) {
            fprintf(stderr, "Receiving Error...\n");
            exit(1);
        } else if (status == 0) {
            printf("Server terminated connection...\n");
            break;
        } else {
            printf("%s> %s\n", server, out);
        };
    };
    //CLOSE FEED
    close(sockFeed);
    printf("Connection closing...\n");
};