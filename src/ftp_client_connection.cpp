/**
 * @file ftp_client_connection.cpp
 * @author Vladislav Mazur, 658453550, NO1, CSCI 460, VIU
 * @version 1.0.0
 * @date January 16, 2024
 *
 * Functions in this code are used to implement connection and disconnection from the server using sockets.
 * Also has the implementation of sending and recieving messages from/to the server.
 * 
 */



//Include required library and custom header files.
#include "../include/ftp_client_connection.h"
#include "sys/socket.h"
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
using namespace std;

void connectToServer(int& socketDescriptor, const char* serverIP, int serverPort)
{
    //Here, parameter 'socketDescriptor' is a place holder and its current value is expected
    //to be -1, i.e., invalid. If 'socketDescriptor' is not -1, it might be already in use
    //return from the function.
    if(socketDescriptor != -1)
    {
        cout << "Socket Already In Use, Aborting..." << endl;
        return;
    }
    //If 'socketDescriptor' is -1, call socket() system call to get a socket descriptor from 
    //the system and assign it to 'socketDescriptor'.
    if(socketDescriptor == -1)
    {     
        //afnet gives ipv4, sockstream gives tcp, 0 is default protocol
        socketDescriptor = socket(AF_INET, SOCK_STREAM, 0); //int socket(int domain, int type, int protocol);
    }
    
    //create a struct to handle the address - IP and Port
    struct sockaddr_in serverAddr;
    
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    serverAddr.sin_addr.s_addr = inet_addr(serverIP);


    //If your socket() call is successful, use the socket descriptor to connect to a server 
    //process running at 'serverIP' and 'serverPort'. Call connect() system call for this. 
    if((connect(socketDescriptor, (const struct sockaddr*) &serverAddr, (socklen_t) sizeof(serverAddr))) == -1) //connect(int sockfd, const struct sockaddr * addr, socklen_t addrlen);
    {
        cout<<"Error: Can't connect to server."<<endl;
        return;
    }

}

void disconnectFromServer(int& socketDescriptor)
{
    //Close network connection that is represented by 'socketDescriptor' by calling
    //close() function and set 'socketDescriptor' value to -1.
    close(socketDescriptor);
    socketDescriptor = -1;
}


int sendToServer(int socketDescriptor, const char* message, int messageLength)
{
    //Send a 'message' of length 'messageLength' bytes to the server through the network connection 
    //represented by 'socketDescriptor'. Call send() system call for this.
    int bytesSent = send(socketDescriptor, message, messageLength, 0); //send(int sockfd, const void * buf, size_t len, int flags);
   
    //check for error
    if(bytesSent == -1)
    {
        cout << "Error: Unable to send to server.";
        return -1;
    }
    
    //Return the number of bytes actually sent by send() function. 
    return bytesSent;
}

int receiveFromServer(int socketDescriptor, char* message, int messageLength)
{
    //Receive a maximum of 'messageLength' bytes into a 'message' buffer  from the server  
    //through the network connection represented by 'socketDescriptor'. Call recv() system call for this.
    int bytesReceived = recv(socketDescriptor, message, messageLength - 1, 0); //recv(int sockfd, void * buf, size_t len, int flags);

    //some error occured
    if (bytesReceived == -1)
    {
        cout << "Error: can't receive message from server." << endl;
        return 0;
    }

    //Return the number of bytes actually received by recv() function.
    return bytesReceived;
}