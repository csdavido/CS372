#!/bin/python
#/******************************************************************************************************************
#** AUTHOR: DAVID RIDER
#** TITLE: ftserver.c
#** CLASS: OSU CS 372
#** LAST MODIFIED: December 2, 2019
#** DESCRIPTION: INITIATES A FILE TRANSFER SERVER
#** CITATION: 
#**     Textbook (Computer Networking: A Top-Down Approach) Pages 164 -166
#**     https://docs.python.org/2/howto/sockets.html
#**     https://stackoverflow.com/questions/166506/finding-local-ip-addresses-using-pythons-stdlib/25850698#25850698  
#*******************************************************************************************************************/
from socket import *
import sys

#/*************************************************************************
#** NAME:
#**    CreateSocket
#** DESCRIPTION:
#**    Creates the socket
#** CITATION:
#**    Textbook (Computer Networking: A Top-Down Approach) Pages 164 -166
#**    https://docs.python.org/2/howto/sockets.html
#**************************************************************************/
def CreateSocket():
    if sys.argv[3] == "-l":
        args = 4
    else:
        args = 5
    sPort = int(sys.argv[args])
    sSocket = socket(AF_INET,SOCK_STREAM)
    sSocket.bind(('', sPort))
    sSocket.listen(1)
    dataSocket, address = sSocket.accept()
    return dataSocket
#/*************************************************************************
#** NAME:
#**    Connect
#** DESCRIPTION:
#**    Creates the connection
#** CITATION:
#**    Textbook (Computer Networking: A Top-Down Approach) Pages 164 -166
#**    https://docs.python.org/2/howto/sockets.html
#**************************************************************************/
def Connect():
    sName = sys.argv[1] + ".engr.oregonstate.edu"
    sPort = int(sys.argv[2])
    cSocket = socket(AF_INET,SOCK_STREAM)
    cSocket.connect((sName, sPort))
    return cSocket
#/*************************************************************************
#** NAME:
#**    GetFiles
#** DESCRIPTION:
#**    Gets the list or individual files
#** CITATION:
#**    Textbook (Computer Networking: A Top-Down Approach) Pages 164 -166
#**    https://docs.python.org/2/howto/sockets.html
#**************************************************************************/
def GetFiles(dataSocket):
    if sys.argv[3] == "-l":
        file = dataSocket.recv(100)
        #WHILE THERE ARE FILES LOOP
        while file != "COMPLETE":
            print file
            file = dataSocket.recv(100)
        return
    else:
        data = dataSocket.recv(1000)
        #OPEN FILE
        f = open(sys.argv[4],'w')
        #LOOP UNTIL END IS REACHED
        while "__done__" not in data:
            #WRITE FILE
            f.write(data)
            data = dataSocket.recv(1000)
#/*************************************************************************
#** NAME:
#**    Transfer
#** DESCRIPTION:
#**    Creates the connection
#** CITATION:
#**    Textbook (Computer Networking: A Top-Down Approach) Pages 164 -166
#**    https://docs.python.org/2/howto/sockets.html
#**************************************************************************/
def Transfer(cSocket):
    if sys.argv[3] == "-l":
        print"Requesting File List..."
        portArg = 4
    else:
        print"Requesting: {}...".format(sys.argv[4])
        portArg = 5

    #SEND COMMAND
    cSocket.send(sys.argv[portArg])
    cSocket.recv(1024)

    if sys.argv[3] == "-l":
        cSocket.send("l")
    else:
        cSocket.send("g")
    #GET IP ADDRESS
    sock = socket(AF_INET, SOCK_DGRAM)
    sock.connect(("8.8.8.8", 80))
    ipAddr = sock.getsockname()[0]

    #SEND IP ADDRESS
    cSocket.send(ipAddr)
    res = cSocket.recv(1024)
    if res[:7] == "INVALID":
        print"Error! Invalid Command..."
        exit(1)

    #GET FILE BY NAME
    if sys.argv[3] == "-g":
        cSocket.send(sys.argv[4])
        res = cSocket.recv(1024)
        if res != "VALID FILE":
            print"Error! File Not Found..."
            return

    #MAKE DATA SOCKET
    dataSocket = CreateSocket()
    #GET LIST OR INDIVIDUAL FILE
    GetFiles(dataSocket)
    #CLOSE SOCKET
    dataSocket.close()

#MAIN PROGRAM FLOW
if __name__ == "__main__":
    #VALIDATE ARGUMENTS LENGTH
    if len(sys.argv) < 5 or len(sys.argv) > 6:
        print "Ivalid Arguments Length..."
        exit(1)
    #VALIDATE HOST NAME
    elif (sys.argv[1] != "flip1" and sys.argv[1] != "flip2" and sys.argv[1] != "flip3"):        
        print "Invalid Server Name..."
        exit(1)
    #VALIDATE PORT NUMBER
    elif (int(sys.argv[2]) < 1024 or int(sys.argv[2]) > 65535):                   
        print "Invalid Port Number..."
        exit(1)
    #VALIDATE COMMAND
    elif (sys.argv[3] != "-g" and sys.argv[3] != "-l"):
        print "Invalid Command..."
        exit(1)
    #VALIDATE RECEIVING PORT NUMBER WITH LIST COMMAND
    elif (sys.argv[3] == "-l" and (int(sys.argv[4])  < 1024 or int(sys.argv[4]) > 65535)):    
        print "Incorrect Port Number..."
        exit(1)
    #VALIDATE RECEIVING PORT NUMBER WITH FILE COMMAND
    elif (sys.argv[3] == "-g" and (int(sys.argv[5]) < 1024 or int(sys.argv[5]) > 65535)):        
        print "Incorrect Port Number..."
        exit(1)
    #CONNECT
    cSocket = Connect()
    #REQUEST FILES AND FILE LIST
    Transfer(cSocket)