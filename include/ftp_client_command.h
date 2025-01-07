/**
 *  @file ftp_client_command.h
 *  @author Humayun Kabir, Instructor, CSCI 460, VIU
 *  @version 1.0.1
 *  @date May 30, 2024
 */

#ifndef __FTP_CLIENT_COMMAND_HEADER__
#define __FTP_CLIENT_COMMAND_HEADER__


#include <string>
#include "ftp_client_session.h"
#include "ftp_server_response.h"


#define FTP_COMMAND_MAX_LENGTH 512

#define FTP_CLIENT_USER_COMMAND_HELP "help"
#define FTP_CLIENT_USER_COMMAND_USER "user"
#define FTP_CLIENT_USER_COMMAND_PASSWORD "pass"
#define FTP_CLIENT_USER_COMMAND_DIRECTORY "dir"
#define FTP_CLIENT_USER_COMMAND_PRINT_DIRECTORY "pwd"
#define FTP_CLIENT_USER_COMMAND_CHANGE_DIRECTORY "cwd"
#define FTP_CLIENT_USER_COMMAND_CHANGE_DIRECTORY_UP "cdup"
#define FTP_CLIENT_USER_COMMAND_GET "get"
#define FTP_CLIENT_USER_COMMAND_QUIT "quit"


#define FILE_OPEN_MODE "a"
#define DATA_SOCKET_RECEIVE_BUFFER_SIZE 65536 //64K

using namespace std;


void interpretAndHandleUserCommand(string command, ClientFtpSession& clientFtpSession, ServerResponse& serverResponse);
// Interpret and handle user command by calling appropriate handleCommandXXXX() function.


void handleCommandHelp();
// Displays following help text on the user terminal
// Usage: csci460Ftp>> [ help | user | pass | pwd | dir | cwd | cdup | get | quit ]
//          help                    Gives the list of FTP commands available and how to use them.
//          user    <username>      Sumbits the <username> to FTP server for authentication.
//          pass    <password>      Sumbits the <password> to FTP server for authentication.
//          pwd                     Requests FTP server to print current directory.
//          dir                     Requests FTP server to list the entries of the current directory.
//          cwd     <dirname>       Requests FTP server to change current working directory.
//          cdup                    Requests FTP server to change current directory to parent directory.
//          get     <filename>      Requests FTP server to send the file with <filename>.
//          quit                    Requests to end FTP session and quit.


bool isLoggedIn(ClientFtpSession clientFtpSession);
// Display error message if the 'isLoggedIn' flag of 'clientFtpSession' is false
// and return false. Return true, otherwise.


char* handleFtpRequest(string ftpRequest, ClientFtpSession& clientFtpSession);
// This is a utility function, which will be called by almost all other functions in this file to
// send a ftp request to a ftp server and receive the response from the server.
// You can implement this function as follows:
// Send 'ftpRequest' to ftp server using 'controlSocket' of 'clientFtpSession' by calling
// sendToServer() function.
// If send is successful, receive ftp server response using the same control socket by calling
// receiveFromServer() function.
// If reception is successful, return the response.
// Display error message and return nullptr, in case, 'controlSocket' is invalid or send or receive fails. 


void handleCommandQuit(ClientFtpSession& clientFtpSession, ServerResponse& serverResponse);
// Check whether the user is logged in or not by calling isLoggedIn() function.
// If the user is logged in, send a 'QUIT' request message to ftp Server
// and recieve the server response by calling handleFtpRequest() function.
// If successful, put the response into 'responses[]' array of 'serverResponse' data structure at
// the correct index and increment the count of 'serverResponse' data structure.
// If QUIT_RESPONSE message is received from the server stop client's ftp session
// by calling stopClientFtpSession() function.
// If the user is not logged in,, stop client's ftp session by calling stopClientFtpSession() function.


void handleCommandUser(string username, ClientFtpSession& clientFtpSession, ServerResponse& serverResponse);
// Send a 'USER username' request message to ftp Server and recieve the server response by calling 
// handleFtpRequest() function.
// If successful, put the response into 'responses[]' array of 'serverResponse' data structure at
// the correct index and increment the count of 'serverResponse' data structure.
// If USERNAME_OK_RESPONSE message is received from the server set 'isUserAuthenticated' flag of
// 'clientFtpSession' true.
// If INVALID_USERNAME_RESPONSE message is received from the server set 'isUserAuthenticated' flag of
// 'clientFtpSession' false and stop client's ftp session by calling stopClientFtpSession() function.
// Otherwise, stop client's ftp session by calling stopClientFtpSession() function.


void handleCommandPassword(string password, ClientFtpSession& clientFtpSession, ServerResponse& serverResponse);
// Check whether the user is authenticated or not by checking 'isUserAuthenitacted' flag of 'clientFtpSession'.
// If the user is authenticated, send a 'PASS password' request message to ftp Server
// and recieve the server response by calling handleFtpRequest() function.
// If successful, put the response into 'responses[]' array of 'serverResponse' data structure at
// the correct index and increment the count of 'serverResponse' data structure.
// If LOGIN_RESPONSE message is received from the server set 'isLoggedIn' flag of
// 'clientFtpSession' true.
// If NOT_LOGGED_IN_RESPONSE message is received from the server set 'isLoggedIn' flag of
// 'clientFtpSession' false and stop client's ftp session by calling stopClientFtpSession() function.
// Otherwise, stop client's ftp session by calling stopClientFtpSession() function.



void handleCommandPrintDirectory(ClientFtpSession& clientFtpSession, ServerResponse& serverResponse);
// Check whether the user is logged in or not by calling isLoggedIn() function.
// If the user is logged in, send a 'PWD' request message to ftp Server
// and recieve the server response by calling handleFtpRequest() function.
// If successful, put the response into 'responses[]' array of 'serverResponse' data structure at
// the correct index and increment the count of 'serverResponse' data structure.
// If the user is not logged in, stop client's ftp session by calling stopClientFtpSession() function.



void handleCommandChangeDirectory(string path, ClientFtpSession& clientFtpSession, ServerResponse& serverResponse);
// Check whether the user is logged in or not by calling isLoggedIn() function.
// If the user is logged in, send a 'CWD' request message to ftp Server
// and recieve the server response by calling handleFtpRequest() function.
// If successful, put the response into 'responses[]' array of 'serverResponse' data structure at
// the correct index and increment the count of 'serverResponse' data structure.
// If the user is not logged in, stop client's ftp session by calling stopClientFtpSession() function.



void handleCommandChangeDirectoryUp(ClientFtpSession& clientFtpSession, ServerResponse& serverResponse);
// Check whether the user is logged in or not by calling isLoggedIn() function.
// If the user is logged in, send a 'CDUP' request message to ftp Server
// and recieve the server response by calling handleFtpRequest() function.
// If successful, put the response into 'responses[]' array of 'serverResponse' data structure at
// the correct index and increment the count of 'serverResponse' data structure.
// If the user is not logged in, stop client's ftp session by calling stopClientFtpSession() function.



void handleCommandDirectory(ClientFtpSession& clientFtpSession, ServerResponse& serverResponse);
// Check whether the user is logged in or not by calling isLoggedIn() function.
// If the user is logged in, call handlePassive() function to get a data connection opened
// with the server.
// If the data connection is opened successfully, call handleNLST() function to receive directory
// list from the server.
// If the user is not logged in, stop client's ftp session by calling stopClientFtpSession() function.
 


void handleCommandGetFile(string filename, ClientFtpSession& clientFtpSession, ServerResponse& serverResponse);
// Check whether the user is logged in or not by calling isLoggedIn() function.
// If the user is logged in, call handleSize() to get the size of the file.
// If successful and file size is greater than zero, call handlePassive() function to get a data connection 
// opened with the server.
// If the data connection is opened successfully, call handleRETR() function to receive file
// content from the server and to write the content into a local file with the same file name.
// If the user is not logged in, stop client's ftp session by calling stopClientFtpSession() function.
 


void handlePassive(ClientFtpSession& clientFtpSession, ServerResponse& serverResponse );
// Send a 'PASV' request message to ftp server and recieve the server response by calling 
// handleFtpRequest() function.
// If successful, put the response into 'responses[]' array of 'serverResponse' data structure at
// the correct index and increment the count of 'serverResponse' data structure.
// If PASSIVE_ERROR_RESPONSE message is received stop client ftp sesion by
// calling stopClientFtpSession() function,
// If PASSIVE_SUCCESS_RESPONSE message is received, retrieve server ip address and data-connection 
// listener port number form the response by calling getHostIPAndPortFromPassiveSuccessResponse().
// Connect to server's data port by calling connectToServer() function with retrieved ip and port number.
// Recieve server response through 'controlSocket' of 'clientFtpSession' by calling
// receiveFromServer() function.
// If successful, put the response into 'responses[]' array of 'serverResponse' data structure at
// the correct index and increment the count of 'serverResponse' data structure.
// In any failure case, stop client's ftp session by calling stopClientFtpSession() function.



void handleNLIST(ClientFtpSession& clientFtpSession, ServerResponse& serverResponse);
// Send a 'NLST' request message to ftp server and recieve the server response by calling 
// handleFtpRequest() function.
// If successful, put the response into 'responses[]' array of 'serverResponse' data structure at
// the correct index and increment the count of 'serverResponse' data structure.
// If NLST_CONNECTION_CLOSE_RESPONSE message is received, call receiveFromServer() function
// to receive directory list from the server over the 'dataSocket' of 'clientFtpSession', put the 
// directory list into 'responses[]' array of 'serverResponse' data structure at the correct index and 
// increment the count of 'serverResponse' data structure, and close data connection from client side 
// by calling disconnectFromServer() function.



void handleSize(string filename, ClientFtpSession& clientFtpSession, ServerResponse& serverResponse, int& size);
// Send a 'SIZE filename' request message to ftp server and recieve the server response by calling 
// handleFtpRequest() function.
// If successful, put the response into 'responses[]' array of 'serverResponse' data structure at
// the correct index and increment the count of 'serverResponse' data structure.
// If FILE_SIZE_RESPONSE message is received, retrieve the size value from the message and assign it 
// to 'size' parameter. 



void handleRETR(string filename, const int size, ClientFtpSession& clientFtpSession, ServerResponse& serverResponse);
// Send a 'RETR filename' request message to ftp server and recieve the server response by calling 
// handleFtpRequest() function.
// If successful, put the response into 'responses[]' array of 'serverResponse' data structure at
// the correct index and increment the count of 'serverResponse' data structure.
// If RETR_CONNECTION_CLOSE_RESPONSE message is received, open a new file with the filename in write only
// mode, receive the file content DATA_SOCKET_RECEIVE_BUFFER_SIZE chunk each time over 'dataSocket' of 
// 'clientFtpSession' from the server by calling receiveFromServer() function, write the received chunk of the file
// content into the opened file, and continue recieving and writing until 'size' number of bytes have been
// received and written. When full content of the file is received and written into the file close the file and the 
// data connection by calling fclose() library function and disconnectFromSrver() function respectively.



void getHostIPAndPortFromPassiveSuccessResponse(char* response, char* hostIP, int& hostPort);
// Retrieve IP address of FTP Server from the response string into reference 'hostIP'.
// Retrive data-connection listening port number of FTP server from the response string into reference 'hostPort'. 
// The example of a successful response message is "227 Entering Passive Mode (192,168,1,65,202,143)."
// From the above response message 'hostIP' will be 192.168.1.65 and 'hostPort' will be
// (202x256 + 143) or 51855. You can call strchr(), strtok(), and atoi() etc. library functions here.


#endif
