/******************************************************************************************************************
** AUTHOR: DAVID RIDER
** TITLE: ftserver.c
** CLASS: OSU CS 372
** LAST MODIFIED: December 2, 2019
** DESCRIPTION: INITIATES A FILE TRANSFER SERVER
** CITATION: 
**     Textbook (Computer Networking: A Top-Down Approach) 
**      Beej's Guide
**      Lecture Notes
#*******************************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>

//************FUNCTION DEFINITIONS************//
struct addrinfo* GetAddResults(char *address, char *port);
int CreateSocket(struct addrinfo *res);
int Connect(int sockFeed, struct addrinfo *res);
void Bind(int sockFeed, struct addrinfo *res);
void Listen(int sockFeed);
int OpenDir(char **dirContents);
void TransFile(char *addr, char *portNum, char *fName);
void TransDir(char * ipAddress, char * port, char ** files, int totalNum);
void Allow(int feed);

//MAIN PROGRAM FLOW
int main(int argc, char *argv[]){
    //CHECK FOR VALID ARGUMENT NUMBER
    if(argc != 2){                  
        fprintf(stderr, "Error! Incorrect number of arguments...\n");
        exit(1);
    }
    //PRINT MESSAGE
    printf("Server open on port %s\n", argv[1]);   
    char *nullAddress = NULL;
    struct addrinfo *res = GetAddResults(nullAddress, argv[1]);
    int sockFeed = CreateSocket(res);
    Bind(sockFeed, res);
    Listen(sockFeed);
    
    struct sockaddr_storage client; 
    socklen_t clientAddr;

    int feed;
    
    while(1){       
        clientAddr = sizeof(client);     
        feed = accept(sockFeed, (struct addrinfo *)&client, &clientAddr);
        
        if(feed == -1){           
            continue;
        }
        //ALLOW CONNECTION
        Allow(feed);
        //CLOSE FEED
        close(feed);
    }
    
    freeaddrinfo(res);
}

/****************************************************
** NAME:
**    GetAddResults
** DESCRIPTION:
**    Gets the address information 
** CITATION:
**    Myself(Project 1) 
****************************************************/
struct addrinfo* GetAddResults(char *address, char *port) {
    int status;

    struct addrinfo *res;
    struct addrinfo hints;
    
    //IF NO ADDRESS
    if (address == NULL) {
        //ALLOCATE MEMORY
        memset(&hints,0,sizeof(hints));
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;
        hints.ai_family = AF_INET;
        
        
        //CHECK STATUS
        status = getaddrinfo(NULL, port, &hints, &res);
        if(status != 0) {
            //PRINT ERROR
            printf("Socket Address Error...\n");
            //EXIT
            exit(1);
        }
    } else {
        //ALLOCATE MEMORY
        memset(&hints, 0, sizeof hints);
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_family = AF_INET;
        
        //CHECK STATUS
        status = getaddrinfo(address, port, &hints, &res);
        if(status != 0)
        {
            printf("Invalid Port Error...\n");
            exit(1);
        }
    }

    return res;

};
/****************************************************
** NAME:
**    CreateSocket
** DESCRIPTION:
**    Creates the socket
** CITATION:
**    Myself(Project 1)
****************************************************/
int CreateSocket(struct addrinfo *res) {
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
**    Connect
** DESCRIPTION:
**    Connects the socket
** CITATION:
**    Myself(Project 1) 
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
**    Bind
** DESCRIPTION:
**    Binds socket to port
****************************************************/
void Bind(int sockFeed, struct addrinfo *res) {
    int bindSocket = 0;
    if ((bindSocket=(bind(sockFeed, res->ai_addr, res->ai_addrlen) == -1))) {
        //CLOSE FEED
        close(sockFeed);
        //PRINT ERROR
        fprintf(stderr, "Socket Binding Error...\n");
        exit(1);
    };
}

/****************************************************
** NAME:
**    Listen
** DESCRIPTION:
**    Listens on port
****************************************************/
void Listen(int sockFeed) {  
    int listenPort = 0;
    if((listenPort =(listen(sockFeed, 1) == -1))) {
        //CLOSE FEED
        close(sockFeed);
        //PRINT ERROR
        fprintf(stderr, "Socket Listening Error...\n");
        exit(1);
    }
}
/****************************************************
** NAME:
**    OpenDir
** DESCRIPTION:
**    Opens directory, copies the file name to the
**    dirContents array, returns the number of files
****************************************************/
int OpenDir(char **dirContents) {
    //GET DIRECTORY
    DIR *dir;
    struct dirent *dir_ent;
    //OPEN DIRECTORY
    dir = opendir(".");
    //COUNTER TO DESIGNATE NUMBER OF FILES IN DIRECTORY
    int c = 0;
    //IF THE DIRECTORY EXISTS
    if (dir) {
        //WHILE THE DIRECTORY IS POPULATED
        while ((dir_ent = readdir(dir)) != NULL) {
            //IF IT IS A REGULAR DIRECTORY
            if (dir_ent->d_type == DT_REG) {
                //COPY THE FILE NAMES
                strcpy(dirContents[c], dir_ent->d_name);
                //INCREMENT FILE COUNTER
                c++;
            }
        }
        //CLOSE DIRECTORY
        closedir(dir);
    }
    //RETURN NUMBER OF FILES IN THE DIRECTORY
    return c;
}
/****************************************************
** NAME:
**    TransFile
** DESCRIPTION:
**    Transfers the file from server to client
** CITATION:
**    Beej's Guide
****************************************************/
void TransFile(char *addr, char *portNum, char *fName){                
    //AVOID RACE CONDITIONS
    sleep(2);
    //GET ADDRESS
    struct addrinfo *res = GetAddResults(addr, portNum);
    //MAKE SOCKET      
    int sock = CreateSocket(res); 
    //CONNECT SOCKET                                              
    Connect(sock, res);
    //BUFFER FOR FILE
    char fileBuff[2000];
    //ALLOCATE                                                                         
    memset(fileBuff, 0, sizeof(fileBuff));
    //OPEN FILE
    int feed = open(fName, O_RDONLY);
    //WHILE READING FILE
    while (1) {
        //READ FILE
        int lineFeed = read(feed, fileBuff, sizeof(fileBuff) - 1);
        //BAD FILE
        if (lineFeed < 0) {
            fprintf(stderr, "File Read Error...\n");
            return;
        }
        //EMPTY FILE
        if (lineFeed == 0) {
            break;
        }

        void *temp = fileBuff;
        while (lineFeed > 0) {
            //DATA BEING SENT
            int sent = send(sock, temp, sizeof(fileBuff),0);
            //ERROR HANDLING
            if (sent < 0) {
                fprintf(stderr, "File Writing Error...\n");
                return;
            }
            //DECREMENT & INCREMENT ACCORDING TO Beej's
            lineFeed = lineFeed - sent;
            temp = temp + sent;
        }
        //CLEAR BUFFER
        memset(fileBuff, 0, sizeof(fileBuff));              
    }
    //ADD END OF FILE MESSAGE
    char *eofMSG = "__done__";
    send(sock, eofMSG, sizeof(eofMSG),0);
    //CLOSE SOCKET 
    close(sock);
    //FREE ADDRESS          
    freeaddrinfo(res);
}
/***************************************************
** NAME:
**    TransDir
** DESCRIPTION:
**    Transfers the directory info from server to
**    client
** CITATION:
**    Beej's Guide
****************************************************/
void TransDir(char * addr, char *portNum, char **fileArray, int numFiles) { 
    //AVOID RACE CONDITIONS
    sleep(2);
    //SET UP CONNECTION
    struct addrinfo *res = GetAddResults(addr, portNum);
    int sock = CreateSocket(res);
    Connect(sock, res);
    //LOOP THROUGH FILE ARRAY
    int x;
    for (x = 0; x < numFiles; x++){ 
        //SEND FILES                    
        send(sock, fileArray[x], 100 ,0);                 
    }
    //SEND COMPLETE MESSAGE
    char* completed = "COMPLETE";
    send(sock, completed, strlen(completed), 0);
    //CLOSE SOCKET
    close(sock);
    //FREE ADDRESS
    freeaddrinfo(res);
}
/***************************************************
** NAME:
**    Allow
** DESCRIPTION:
**    Allows connections from client to occur
** CITATION:
**    Beej's Guide
****************************************************/
void Allow(int feed){              
    //BUFFERS
    char portBuff[100];
    char ipBuff[100];
    char cmdBuff[500];
    //MESSAGES
    char *valid = "VALID";
    char *invalid = "INVALID";
    //SET BUFFER FOR PORT NUMBER
    memset(portBuff, 0, sizeof(portBuff));
    recv(feed, portBuff, sizeof(portBuff)-1, 0);
    send(feed, valid, strlen(valid), 0);
    //SET BUFFER FOR COMMAND
    memset(cmdBuff, 0, sizeof(cmdBuff));            
    recv(feed, cmdBuff, sizeof(cmdBuff)-1, 0);
    send(feed, valid, strlen(valid),0);
    //SET BUFFER FOR IP ADDRESS
    memset(ipBuff, 0, sizeof(ipBuff));
    recv(feed, ipBuff, sizeof(ipBuff)-1,0);
    send(feed, valid, strlen(valid),0);
    //PRINT CONNECTION MESSAGE
    printf("New connection from %s\n", ipBuff);
    //FOR LIST FILE COMMAND
    if (strcmp(cmdBuff, "l") == 0) {                  
        //PRINT MESSAGES
        printf("List directory requested on port %s \n", portBuff);
        printf("Sending file list to %s \n", ipBuff);
        //SEND VALIDATOR
        send(feed, valid, strlen(valid),0);
        //CREATE ARRAY TO HOLD FILES
        char **fileArray = malloc(500 *sizeof(char *));
        int x;
        for(x = 0; x < 500; x++){
            fileArray[x] = malloc(100*sizeof(char));
            memset(fileArray[x],0,sizeof(fileArray[x]));
        }
        //GET NUMBER OF FILES
        int files = OpenDir(fileArray);
        //SEND DIRECTORY DATA
        TransDir(ipBuff, portBuff, fileArray, files);
        //DELETE FILES
        int y;
        for (y = 0; y < 500; y++){
            free(fileArray[y]);
        }
        free(fileArray);
     }
     //FOR SPECIFIC FILE 
     else if(strcmp(cmdBuff, "g") == 0) { 
        //SEND VALIDATOR
        send(feed, valid, strlen(valid), 0);
        
        char fName[100];
        memset(fName, 0, sizeof(fName));
        recv(feed, fName, sizeof(fName) - 1, 0);
        printf("File %s requested on port %d \n", fName, portBuff);
        
        //CREATE ARRAY TO HOLD FILE
        char **fileArray = malloc(500*sizeof(char *));
        int x;
        for(x = 0; x < 500; x++) {
            fileArray[x] = malloc(100*sizeof(char));
            memset(fileArray[x],0,sizeof(fileArray[x]));
        }

        int files = OpenDir(fileArray); 
        
        //CHECK FILE
        int isFile = 0;                
        int y;
        //LOOP THROUGH TO CHECK FOR FILE
        for (y = 0; y < files; y++){             
            if(strcmp(fileArray[y], fName) == 0){
                isFile = 1;
            }
        }
        //IF FILE IS PRESENT
        if(isFile) {
            //SEND VALIDATION MESSAGE
            printf("Sending file %s to client\n", fName);
            char *validFile = "VALID FILE";
            send(feed, validFile, strlen(validFile), 0);
            //HOLDER FOR NEW FILE NAME
            char fNameBuff[100];
            memset(fNameBuff, 0, sizeof(fNameBuff));
            strcpy(fNameBuff, "./");
            //MAKE NEW LINE ENDING
            char *lineEnding = fNameBuff + strlen(fNameBuff);
            lineEnding = lineEnding + sprintf(lineEnding, "%s", fName);
            //TRANSFER FILE
            TransFile(ipBuff, portBuff, fNameBuff);
        } else{
            //PRINT ERROR MESSAGE          
            printf("File not found. Sending error message.\n");
            //SEND VALIDATORY
            char *invalidFile = "INVALID FILE";
            send(feed, invalidFile, 100, 0);
        }
        //DELETE
        int z;
        for (z = 0; z < 500; z++){
            free(fileArray[z]);
        }
        free(fileArray);
    } else {
        //INVALID COMMAND
        printf("Invalid Command Error...\n");
        send(feed, invalid, strlen(invalid), 0);
    }
    //PRINT WAITING MESSAGE
    printf("Waiting For Connection...\n");
}