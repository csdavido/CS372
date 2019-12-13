/***********************************************
** AUTHOR: DAVID RIDER
** TITLE: ftserver.c
** CLASS: OSU CS 372
** LAST MODIFIED: December 2, 2019
** DESCRIPTION: INITIATES A FILE TRANSFER SERVER
** CITATION: Beej's Guide
************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>


/*****FUNCTION DECLARATIONS*****/
struct addrinfo* GetAddResults(char *address, char *port);
int CreateSocket(struct addrinfo *res);
int Connect(int sockFeed, struct addrinfo *res);
void Bind(int sockFeed, struct addrinfo *res);
void Listen(int sockFeed);
void AllowConnection(int sockFeed);
int OpenDir(char **dirContents);
void Transfer(int sockFeed);

/*****PROGRAM FLOW******/
int main(int args, char *argv[]){
    
    if(args != 2) {
        //PRINT ERROR
        fprintf(stderr, "Error! Incorrect number of arguments...\n");
        //EXIT
        exit(1);
    }
    //PRINT MESSAGE
    printf("Server open on port %s\n", argv[1]);

    char* start = NULL;
    struct addrinfo *res = GetAddResults(start, argv[1]);
    //MAKE SOCKET
    int sockFeed;
    sockFeed = CreateSocket(res);
    //BIND PORT
    Bind(sockFeed, res);
    //LISTEN ON PORT
    Listen(sockFeed);
    //ALLOW CONNECTIONS
    AllowConnection(sockFeed);
    //FREE
    freeaddrinfo(res);
}

/*****HELPER FUNCTIONS*******/
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
        status = getaddrinfo(address, port, &hints, &IP);
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
**    GetAddResults
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
**    GetAddResults
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
**    AllowConnection
** DESCRIPTION:
**    Allows connection to occur from client
****************************************************/
void AllowConnection(int sockFeed){
    //CONNCECTION SETUP
    struct sockaddr_storage client;
    socklen_t conn;
    //NEW FEED FROM CLIENT
    int feed;
    //LOOP UNTIL A SIGNAL INTERUPTION OCCURS
    while(1) {
        
        conn = sizeof(client);
        
        //GET CONNECTION FROM CLIENT
        feed = accept(sockFeed, (struct sockaddr *)&client, &conn);
        //IF NO LINK
        if(feed == -1) {
            //RETURN NOTHING
            return;
        }
        
        //MOVE TO FILE TRANSFER
        Transfer(feed);
        //CLOSE SOCKET
        close(feed);
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
/*************************************************************************************************************************************************
** NAME:
**    OpenDir
** DESCRIPTION:
**    Opens directory, copies the file name to the
**    dirContents array, returns the number of files
** CITATION:
**    https://www.linuxquestions.org/questions/programming-9/tcp-file-transfer-in-c-with-socket-server-client-on-linux-help-with-code-4175413995/
*************************************************************************************************************************************************/
void Transfer(int sockFeed) {
   //messages for client validation
    char* validPort = "valid port";
    char* invalidCommand = "invalid command";
    char* validCommand = "valid command";
    char* fileNotFound = "no such file in directory";


    int i = 0;
    
    //buffers for sent client arguments
    char portNum[100];
    char commandBuffer[100];
    char ipBuffer[100];
    
    //collect the port from the tcp connection
    memset(portNum,0,sizeof(portNum));
    recv(sockFeed,portNum,sizeof(portNum) - 1 ,0);
    //********************************************************************
    send(sockFeed,validPort,strlen(validPort) ,0);
    
    //collect the command flag from the tcp connection
    memset(commandBuffer,0,sizeof(commandBuffer));
    recv(sockFeed,commandBuffer,sizeof(commandBuffer) -1 ,0);
    

    //collect the ip address from the tcp connection
    memset(ipBuffer,0,sizeof(ipBuffer));
    recv(sockFeed,ipBuffer,sizeof(ipBuffer) - 1 ,0);
    
    printf("Connection from: %s\n",ipBuffer);
    
    //if client requests a list of files in the server directory
    if(strcmp(commandBuffer,"l") == 0)
    {
        
        //send command validation message
        //##########################################################
        send(sockFeed, validCommand, strlen(validCommand),0);
        printf("List directory requested on port %s\n", portNum);
        printf("Sending directory contents to %s :%s\n", ipBuffer, portNum);
        
        //create array of strings for file names
        i = 0;
        char ** files = malloc(100*sizeof(char *));
        for(;i < 100; i++)
        {
            files[i] = malloc(100*sizeof(char));
            memset(files[i],0,sizeof(files[i]));
        }
        
        //call getFiles to count files and fill the files array with the
        // collected file names
        int fileCount = OpenDir(files);

        sleep(1); //sleep for race condition that was breaking the connection

        //build ftp connection
        struct addrinfo * IP = GetAddResults(ipBuffer, portNum);
        int dataSocket = CreateSocket(IP);
        Connect(dataSocket, IP);
        
        //send files over ftp connection
        int i = 0;
        for (; i < fileCount; i++)
        {
            send(dataSocket, files[i], 100,0);
        }

        //printf(" This is how many files im sending back: %i\n",i);
        
        //end of file list flag for signaling to client the end
        // of the list
        char * eofList = "eof_list";
        send(dataSocket, eofList, strlen(eofList),0);
        
        //free the file array 
        for(i=0;i<100;i++)
            free(files[i]);
        free(files);

        //close the ftp connection and free address object
        close(dataSocket);
        freeaddrinfo(IP);
    }
    //if client requests a file from the server
    else if(strcmp(commandBuffer, "g") == 0)
    {
        //printf("This is in the portNum: %s\n",portNum);
        //validation for command 
        //*********************************************************************
        send(sockFeed, validCommand, strlen(validCommand),0);
        
        //accept the file name the client passed as an argument
        // and sent over the tcp connection
        char fileName[100];
        memset(fileName, 0, sizeof(fileName));
        recv(sockFeed, fileName, sizeof(fileName)-1,0);
        printf("File %s requested on port %s\n", fileName, portNum);

        //create array of string to hold file list
        i = 0;
        char ** files = malloc(100*sizeof(char *));
        for(;i < 100; i++)
        {
            files[i] = malloc(100*sizeof(char));
            memset(files[i],0,sizeof(files[i]));
        }

        //collect files and count files
        int fileCount = OpenDir(files);

        // search for file in list
        // set fileFound flag if found
        int fileFound = 0;
        i = 0;
        for (; i < fileCount; i++)
        {
            if(strcmp(files[i], fileName) == 0){
                fileFound = 1;
            }
        }
        // If client requests a file that is in the directory
        // send the file to the client
        if(fileFound == 1)
        {
            
            //File was found, validate found file in the directory to client
            printf("Sending %s to %s: %s\n", fileName,ipBuffer,portNum);
            char * foundMsg = "found";
            send(sockFeed, foundMsg, strlen(foundMsg),0);
         
            //build command to open file
            // http://www.cplusplus.com/reference/cstdio/sprintf/
            char newFile[100];
            memset(newFile,0,sizeof(newFile));
            strcpy(newFile, "./");
            char * commandLine = newFile + strlen(newFile);
            commandLine = commandLine + sprintf(commandLine, "%s", fileName);

            sleep(1); //sleep for race condition that was breaking the connection

            //build ftp connection
            struct addrinfo * IP = GetAddResults(ipBuffer, portNum);
            int dataSocket = CreateSocket(IP);
            Connect(dataSocket, IP);
            
            //hold read file in temp buffer
            char tempBuffer[1000];
            memset(tempBuffer, 0, sizeof(tempBuffer));
            
            //open and read file 
            int file = open(fileName, O_RDONLY);
            while (1) {
                
                int fileRead = read(file, tempBuffer, sizeof(tempBuffer)-1);
                //if file is empty
                if (fileRead < 0)
                {
                    printf("Error reading the file.\n");
                    return;
                }

                //If the file has finished being read
                // break the loop
                if(fileRead == 0)
                {
                    break;
                }

                //buffer for the contents of the file to be sent
                void * fileContents = tempBuffer;
                while (fileRead > 0) 
                {
                    //send file contents to client
                    int fileWritten = send(dataSocket, fileContents, sizeof(tempBuffer),0);
                    if (fileWritten < 0) 
                    {
                        fprintf(stderr, "Error writing to socket\n");
                        return;
                    }
                    //Initializes buffers to new sizes  
                    // breaks the file into sections based on how 
                    // much the client will accept
                    // See section 6.3 in BEEJ's guide to network programming
                    // for handling partial sends
                    fileRead = fileRead - fileWritten;
                    fileContents = fileContents + fileWritten;
                }
                
                //clear buffer
                memset(tempBuffer, 0, sizeof(tempBuffer));
            }
            
            //concatenate a eof flag for the client
            // to find the end of file
            memset(tempBuffer, 0, sizeof(tempBuffer));
            strcpy(tempBuffer, "__eof__");
            send(dataSocket, tempBuffer, sizeof(tempBuffer),0);
            
            //close connection
            close(dataSocket);
            freeaddrinfo(IP);
        }
        // if no file was found log debug script
        else
        {
            //******************************************************************************************
            printf("File not found, sending error message to %s: %s\n",ipBuffer,portNum);
            send(sockFeed, fileNotFound, strlen(fileNotFound), 0);

        }
       
        //free files array
        for(i=0;i<100;i++)
            free(files[i]);
        free(files);
    }
    //if command flag was invalid
    else
    {   
        //******************************************************************
        send(sockFeed, invalidCommand, strlen(invalidCommand), 0);
        printf("Invalid command sent\n");
    }
    printf("Waiting for connection...\n");
}