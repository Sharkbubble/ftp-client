/**
 *  @file ftp_client_connection.h
 *  @author Humayun Kabir, Instructor, CSCI 460, VIU
 *  @version 1.0.1
 *  @date May 29, 2024
 */

#ifndef __FTP_CLIENT_CONNECTION_HEADER__
#define __FTP_CLIENT_CONNECTION_HEADER__



void connectToServer(int& socketDescriptor, const char* serverIP, int serverPort);
//Here, parameter 'socketDescriptor' is a place holder and its current value is expected
//to be -1, i.e., invalid. If 'socketDescriptor' is not -1, it might be already in use
//return from the function.
//If 'socketDescriptor' is -1, call socket() system call to get a socket descriptor from 
//the system and assign it to 'socketDescriptor'.
//If your socket() call is successful, use the socket descriptor to connect to a server 
//process running at 'serverIP' and 'serverPort'. Call connect() system call for this. 


void disconnectFromServer(int& socketDescriptor);
//Close network connection that is represented by 'socketDescriptor' by calling
//close() function and set 'socketDescriptor' value to -1.



int sendToServer(int socketDescriptor, const char* message, int messageLength);
//Send a 'message' of length 'messageLength' bytes to the server 
//through the network connection represented by 'socketDescriptor'. Call send() 
//system call for this.
//Return the number of bytes actually sent by send() function. 


int receiveFromServer(int socketDescriptor, char* message, int messageLength);
//Receive a maximum of 'messageLength' bytes into a 'message' buffer  from the server 
//through the network connection represented by 'socketDescriptor'. Call recv()
//system call for this.
//Return the number of bytes actually received by recv() function.

#endif
