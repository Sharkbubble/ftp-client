/**
 *  @file ftp_client_session.h
 *  @author Humayun Kabir, Instructor, CSCI 460, VIU
 *  @version 1.0.1
 *  @date May 29, 2024
 */


#ifndef __FTP_CLIENT_SESSION_HEADER__
#define __FTP_CLIENT_SESSION_HEADER__

#define FTP_CLIENT_DEAFULT_SERVER_IP "127.0.0.1"
#define FTP_CLIENT_DEFAULT_SERVER_PORT 21
#define BUFFER_SIZE 512

/**
 * Data structure to hold information related to a client ftp session with a server.
 */ 
struct ClientFtpSession {
	int controlSocket = -1; 			//Socket descriptor for control connection
	int dataSocket = -1;				//Socket descriptor for data connection
	bool isUserAuthenticated = false;	//Flag to represent whether the username has been authenticated or not
	bool isLoggedIn = false;			//Flag to represent whether the user has been logged in or not
};


void startClientFTPSession(const char* serverIP, int serverPort, ClientFtpSession& clientFtpSession);
//Establish a control connection with the ftp server specified by 'serverIP' and 'serverPort' by calling
//connectToServer() function. 
//If connection establishment is successful, receive a response from the connected server by calling
//receiveFromServer() function.
//If reception is successful, show the response by calling showFtpResponse() function.


void stopClientFTPSession(ClientFtpSession& clientFtpSession);
//Close both control and data connection sockets of 'clientFtpSession' by calling disconnectFromServer() function.
//Set false to both 'isUserAuthenticated' and 'isLoggedIn' flags.


#endif
