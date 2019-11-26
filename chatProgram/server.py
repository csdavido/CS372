#!/bin/python
#/***********************************
#** AUTHOR: DAVID RIDER
#** TITLE: server.py
#** CLASS: OSU CS 372
#** LAST MODIFIED: NOVEMBER 3, 2019
#** DESCRIPTION: INITIATES A CHAT SERVER AND LISTENS FOR CLIENTS
#** CITATION: https://docs.python.org/release/2.6.5/library/internet.html
#***********************************/
from socket import *
import sys

#/****************************************************
#** NAME:
#**    Chat
#** DESCRIPTION:
#**    Handles the chat functionality
#****************************************************/
def chat(cSocket, c_name, user):
    
    while 1:
        #GET MESSAGE
        input_msg = cSocket.recv(501)[0:-1]
        #WAIT FOR MESSAGE
        if input_msg == "":
            print "Connection ended..."
            print "Standby..."
            return
        #PROMPT
        print "{}> {}".format(c_name, input_msg)

        #SEND MESSAGE
        output_msg = ""
        while len(output_msg) > 500 or len(output_msg) == 0:
            #SEND MESSAGE
            output_msg = raw_input("{}> ".format(user))
            
            #CHECK FOR TOO LARGE MESSAGE
            if len(output_msg) > 500:
                print "Error! Message exceedes max length!"

            #CHECK FOR EMPTY MESSAGE
            if len(output_msg) < 1:
                print "Error! Message exceedes max length!"

        #QUIT PROGRAM
        if output_msg == "\quit":
            print "Connection ended..."
            print "Standby..."
            break
        #SEND MESSAGE
        cSocket.send(output_msg)

#*****PROGRAM FLOW******
if __name__ == "__main__":
    #CHECK ARGUMENTS
    if len(sys.argv) != 2:
        print "Error! Incorrect arguments!"
        print "Try: server.py [port number]"
        exit(1)
    #GET PORT
    port = sys.argv[1]

    #GET SOCKET
    sSocket = socket(AF_INET, SOCK_STREAM)
    #BIND SOCKET
    sSocket.bind(('', int(port)))
    #LISTEN FOR CLIENT
    sSocket.listen(1)
    #USERNAME
    user = ""
    #GET USERNAME
    while len(user) > 10 or len(user) == 0:
        user = raw_input("Enter username 10 characters or less: ")
        #CHECK LENGTH
        if len(user) < 1 or len(user) > 10:
            print "Error! User name must be 10 characters or less...."
    print "Standby..."

    while 1:
        #ESTABLISH CONNECTION
        cSocket, address = sSocket.accept()
        #PRINT INFO
        print "Connection established!"
        print "Address: {}".format(address)
        print "Port: {}".format(port)

        c_name = cSocket.recv(1024)
        cSocket.send(user)

        #INITIATE CHAT
        chat(cSocket, c_name, user)
        #CLOSE SOCKET
        cSocket.close()