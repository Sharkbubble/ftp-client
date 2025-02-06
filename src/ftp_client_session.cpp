/**
 * @file ftp_client_session.cpp
 * @author Vladislav Mazur, 658453550, NO1, CSCI 460, VIU
 * @version 1.0.0
 * @date January 17, 2024
 *
 * The functions implemented in this file control the starting and ending of ftp sessions.
 * Creates a data structure to log in/out and to check/authenticate a user.
 * 
 */



//Include required library and custom header files.
#include "../include/ftp_client_session.h"
#include "../include/ftp_client_connection.h"
#include "../include/ftp_client_command.h"
#include "../include/ftp_client_ui.h"
#include "../include/ftp_server_response.h"

#include <iostream>

using namespace std;


//Implement all the functions prototyped in the header file ftp_client_session.h

void startClientFTPSession(const char* serverIP, int serverPort, ClientFtpSession& clientFtpSession)
{
    //Establish a control connection with the ftp server specified by 'serverIP' and 
    //'serverPort' by calling connectToServer() function. 
    connectToServer(clientFtpSession.controlSocket, serverIP, serverPort);

    //error occured
    if (clientFtpSession.controlSocket == -1)
    {
        cout << "Error: Couldn't to connect to server." << endl;
        return;
    }

    //If connection establishment is successful, receive a response from the 
    //connected server by calling receiveFromServer() function.
    int receiveSize = FTP_RESPONSE_MAX_LENGTH; //1024 
    char buffer[receiveSize];
    int responseLength = receiveFromServer(clientFtpSession.controlSocket, buffer, sizeof(buffer));

    //error 
    if (responseLength <= 0)
    {
        cout << "Error: Response error." << endl;
        return;
    }

    //If reception is successful, show the response by calling showFtpResponse() function.
    showFtpResponse(buffer); 
}


void stopClientFTPSession(ClientFtpSession& clientFtpSession)
{
    //Close both control and data connection sockets of 'clientFtpSession' by calling disconnectFromServer() function.
    disconnectFromServer(clientFtpSession.controlSocket);
    disconnectFromServer(clientFtpSession.dataSocket);

    //Set false to both 'isUserAuthenticated' and 'isLoggedIn' flags.
    clientFtpSession.isUserAuthenticated = false;
    clientFtpSession.isLoggedIn = false;
}