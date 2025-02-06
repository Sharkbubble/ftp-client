/**
 * @file ftp_client_command.cpp
 * @author Vladislav Mazur, 658453550, NO1, CSCI 460, VIU
 * @version 1.0.0
 * @date February 5, 2024
 *
 * This cpp file is used to interpret and use all the commands ftp server uses
 * Both deliverable 1 and 2 commands are implemented and handled by the program
 * 
 * 
 */



//Include required library and custom header files.
#include "../include/ftp_client_command.h"
#include "../include/ftp_client_ui.h"
#include "../include/ftp_client_connection.h"
#include "../include/ftp_client_session.h"
#include <iostream>
#include <cstring>

using namespace std;



//Implement all the functions prototyped in the header file ftp_client_command.h

void interpretAndHandleUserCommand(string command, ClientFtpSession& clientFtpSession, ServerResponse& serverResponse)
{
    // Interpret and handle user command by calling appropriate handleCommandXXXX() function.

    // help command
    if (command == "help")
    {
        handleCommandHelp();
    }

    // user command
    else if (command.substr(0, 5) == "user ") // substring of first 5 chars
    {
        // name is after 5 characters
        string username = command.substr(5);
        handleCommandUser(username, clientFtpSession, serverResponse);
    }

    // pass command
    else if (command.substr(0, 5) == "pass ")
    {
        string password = command.substr(5);
        handleCommandPassword(password, clientFtpSession, serverResponse);
    }

    // pwd command
    else if (command == "pwd")
    {
        handleCommandPrintDirectory(clientFtpSession, serverResponse);
    }

    // dir command
    else if (command == "dir")
    {
        handleCommandDirectory(clientFtpSession, serverResponse);
    }

    // cwd command
    else if (command.substr(0, 4) == "cwd ")
    {
        string path = command.substr(4);
        handleCommandChangeDirectory(path, clientFtpSession, serverResponse);
    }

    // cdup command
    else if (command == "cdup")
    {
        handleCommandChangeDirectoryUp(clientFtpSession, serverResponse);
    }

    // get command
    else if (command.substr(0, 4) == "get ")
    {
        string filename = command.substr(4);
        handleCommandGetFile(filename, clientFtpSession, serverResponse);
    }

    // quit command
    else if (command == "quit")
    {
        handleCommandQuit(clientFtpSession, serverResponse);
    }

    // unknown command
    else
    {
        cout << "CSCI460FTP>> Invalid command!" << endl;
        handleCommandHelp();
    }
}



void handleCommandHelp()
{
    //displays the help menu to user
    cout << "Usage: csci460Ftp>> [ help | user | pass | pwd | dir | cwd | cdup | get | quit ]" << endl;
    cout << "          help                    Gives the list of FTP commands available and how to use them." << endl;
    cout << "          user    <username>      Submits the <username> to FTP server for authentication." << endl;
    cout << "          pass    <password>      Submits the <password> to FTP server for authentication." << endl;
    cout << "          pwd                     Requests FTP server to print current directory." << endl;
    cout << "          dir                     Requests FTP server to list the entries of the current directory." << endl;
    cout << "          cwd     <dirname>       Requests FTP server to change current working directory." << endl;
    cout << "          cdup                    Requests FTP server to change current directory to parent directory." << endl;
    cout << "          get     <filename>      Requests FTP server to send the file with <filename>." << endl;
    cout << "          quit                    Requests to end FTP session and quit." << endl;
}


bool isLoggedIn(ClientFtpSession clientFtpSession)
{
    //client is logged in
    if(clientFtpSession.isLoggedIn)
    {
        return true;
    }

    // Display error message if the 'isLoggedIn' flag of 'clientFtpSession' is false
    else
    {
        cout << "Error: User is not logged in" << endl;
        return false;
    }

}


// This is a utility function, which will be called by almost all other functions in this file to
// send a ftp request to a ftp server and receive the response from the server.
// You can implement this function as follows:
char* handleFtpRequest(string ftpRequest, ClientFtpSession& clientFtpSession)
{
    // Display error message and return nullptr, in case, 'controlSocket' is invalid 
    if (clientFtpSession.controlSocket == -1)
    {
        cout << "Error: socket is invalid." << endl;
        return nullptr;
    }
    // Send 'ftpRequest' to ftp server using 'controlSocket' of 'clientFtpSession' by calling
    // sendToServer() function.
    int bytesSent = sendToServer(clientFtpSession.controlSocket, ftpRequest.c_str(), ftpRequest.length());
    
    // send failed
    if (bytesSent <= 0)
    {
        cout << "Error: Sending to Server Failed." << endl;
        return nullptr;
    }

    // If send is successful, receive ftp server response using the same control socket by calling
    // receiveFromServer() function.
    int receiveSize = FTP_RESPONSE_MAX_LENGTH; //1024 
    char* buffer = new char[receiveSize]; // dynamic allocation 
    memset(buffer, 0, receiveSize); //init to 0
    int bytesReceived = receiveFromServer(clientFtpSession.controlSocket, buffer, receiveSize);

    // recieve failed
    if (bytesReceived <= 0)
    {
        cout << "Error: Recieving from server Failed." << endl;
        delete[] buffer; // free mem
        return nullptr;
    }
    // If reception is successful, return the response.
    return buffer;
}


void handleCommandQuit(ClientFtpSession& clientFtpSession, ServerResponse& serverResponse)
{
    // Check whether the user is logged in or not by calling isLoggedIn() function.
    if (isLoggedIn(clientFtpSession) == false) 
    {
        // If the user is not logged in,, stop client's ftp session by calling stopClientFtpSession() function.
        cout << "Error: User is not logged in." << endl;
        stopClientFTPSession(clientFtpSession);
        return;
    }

    // If the user is logged in, send a 'QUIT' request message to ftp Server
    // and recieve the server response by calling handleFtpRequest() function.
    string request = "QUIT"; 
    char* response = handleFtpRequest(request, clientFtpSession);

    if (response == nullptr)
    {
        cout << "Error: Response Failed." << endl;
        return;
    }

    // If successful, put the response into 'responses[]' array of 'serverResponse' data structure at
    // the correct index and increment the count of 'serverResponse' data structure.
    if (serverResponse.count < MAX_NUMBER_OF_RESPONSES)
    {
        serverResponse.responses[serverResponse.count] = response;
        serverResponse.count++;
    }
    else
    {
        cout << "Error: No space for responses left." << endl;
        //free mem
        delete[] response;
        return;
    }
    // If QUIT_RESPONSE message is received from the server stop client's ftp session
    // by calling stopClientFtpSession() function.
    if (string(response) == QUIT_RESPONSE)
    {
        stopClientFTPSession(clientFtpSession);
    }
    delete[] response;
}


void handleCommandUser(string username, ClientFtpSession& clientFtpSession, ServerResponse& serverResponse)
{
    // Send a 'USER username' request message to ftp Server and recieve the server response by calling 
    // handleFtpRequest() function.
    string request = "USER " + username;
    char* response = handleFtpRequest(request, clientFtpSession);

    //error occured
    if (response == nullptr)
    {
        cout << "Error: USER cmd failed." << endl;
        stopClientFTPSession(clientFtpSession);
        return;
    }

    // If successful, put the response into 'responses[]' array of 'serverResponse' data structure at
    // the correct index and increment the count of 'serverResponse' data structure.
    if (serverResponse.count < MAX_NUMBER_OF_RESPONSES)
    {
        serverResponse.responses[serverResponse.count] = response;
        serverResponse.count++;
    }
    else
    {
        cout << "Error: No space for responses left." << endl;
        //free mem
        delete[] response;
        return;
    }

    // If USERNAME_OK_RESPONSE message is received from the server set 'isUserAuthenticated' flag of
    // 'clientFtpSession' true.
    if (serverResponse.responses[serverResponse.count - 1] == USERNAME_OK_RESPONSE) //-1 to get the latest response since we incremented it earlier 
    {
        clientFtpSession.isUserAuthenticated = true;
    }

    // If INVALID_USERNAME_RESPONSE message is received from the server set 'isUserAuthenticated' flag of
    // 'clientFtpSession' false and stop client's ftp session by calling stopClientFtpSession() function.
    else if (serverResponse.responses[serverResponse.count - 1] == INVALID_USERNAME_RESPONSE)
    {
        clientFtpSession.isUserAuthenticated = false;
        stopClientFTPSession(clientFtpSession);
    }

    // Otherwise, stop client's ftp session by calling stopClientFtpSession() function.
    else
    {
        cout << "Error: Stopping the session" << endl;
        stopClientFTPSession(clientFtpSession);
    }

    //free mem
    delete[] response; 
}


void handleCommandPassword(string password, ClientFtpSession& clientFtpSession, ServerResponse& serverResponse)
{
    // Check whether the user is authenticated or not by checking 'isUserAuthenitacted' flag of 'clientFtpSession'.
    if (clientFtpSession.isUserAuthenticated == false)
    {
        cout << "Error: User not authenticated." << endl;
        return; 
    }

    // If the user is authenticated, send a 'PASS password' request message to ftp Server
    // and recieve the server response by calling handleFtpRequest() function.
    string request = "PASS " + password; 
    char* response = handleFtpRequest(request, clientFtpSession);

    //error 
    if (response == nullptr)
    {
        cout << "Error: PASS cmd failed." << endl;
        return;
    }

    // If successful, put the response into 'responses[]' array of 'serverResponse' data structure at
    // the correct index and increment the count of 'serverResponse' data structure.
    if (serverResponse.count < MAX_NUMBER_OF_RESPONSES)
    {
        serverResponse.responses[serverResponse.count] = response;
        serverResponse.count++;
    }

    // If LOGIN_RESPONSE message is received from the server set 'isLoggedIn' flag of
    // 'clientFtpSession' true.
    if (string(response) == LOGIN_RESPONSE)
    {
        clientFtpSession.isLoggedIn = true;
    }

    // If NOT_LOGGED_IN_RESPONSE message is received from the server set 'isLoggedIn' flag of
    // 'clientFtpSession' false and stop client's ftp session by calling stopClientFtpSession() function.
    else if (string(response) == NOT_LOGGED_IN_RESPONSE)
    {
        clientFtpSession.isLoggedIn = false;
        stopClientFTPSession(clientFtpSession);
    }

    // Otherwise, stop client's ftp session by calling stopClientFtpSession() function.
    else
    {
        cout << "Error: Stopping the session." << endl;
        stopClientFTPSession(clientFtpSession);
    }

    // free mem
    delete[] response;
}


/*
////////////////////////////////////////////////////////////////////////////////////////

Functions for Deliverable 2

////////////////////////////////////////////////////////////////////////////////////////
*/

void handleCommandPrintDirectory(ClientFtpSession& clientFtpSession, ServerResponse& serverResponse)
{
    // Check whether the user is logged in or not by calling isLoggedIn() function.
    if (clientFtpSession.isLoggedIn == false)
    {
        // If the user is not logged in, stop client's ftp session by calling stopClientFtpSession() function.
        stopClientFTPSession(clientFtpSession);
        return;
    }

    // If the user is logged in, send a 'PWD' request message to FTP Server
    string request = "PWD"; 
    
    // Receive the server response by calling handleFtpRequest() function.
    string response = handleFtpRequest(request,clientFtpSession);

    // If successful, put the response into 'responses[]' array of 'serverResponse' data structure
    // at the correct index and increment the count of 'serverResponse' data structure.
    if (serverResponse.count < MAX_NUMBER_OF_RESPONSES)
    {
        serverResponse.responses[serverResponse.count] = response;
        serverResponse.count++;
    }
    else
    {
        cout << "Error: No space for responses left." << endl;
        return;
    }
}


void handleCommandChangeDirectory(string path, ClientFtpSession& clientFtpSession, ServerResponse& serverResponse)
{
    // Check whether the user is logged in or not by calling isLoggedIn() function.
    if (clientFtpSession.isLoggedIn == false)
    {
        // If the user is not logged in, stop client's ftp session by calling stopClientFtpSession() function.
        stopClientFTPSession(clientFtpSession);
        return;
    }

    // If the user is logged in, send a 'CWD' request message to FTP Server
    string request = "CWD " + path;

    // Receive the server response by calling handleFtpRequest() function.
    string response = handleFtpRequest(request, clientFtpSession);

    // If successful, put the response into 'responses[]' array of 'serverResponse' data structure
    // at the correct index and increment the count of 'serverResponse' data structure.
    if (serverResponse.count < MAX_NUMBER_OF_RESPONSES)
    {
        serverResponse.responses[serverResponse.count] = response;
        serverResponse.count++;
    }
    else
    {
        cout << "Error: No space for responses left." << endl;
        return;
    }
}


void handleCommandChangeDirectoryUp(ClientFtpSession& clientFtpSession, ServerResponse& serverResponse)
{
    // Check whether the user is logged in or not by calling isLoggedIn() function.
    if (clientFtpSession.isLoggedIn == false)
    {
        // If the user is not logged in, stop client's ftp session by calling stopClientFtpSession() function.
        stopClientFTPSession(clientFtpSession);
        return;
    }

    // If the user is logged in, send a 'CDUP' request message to FTP Server
    string request = "CDUP";

    // Receive the server response by calling handleFtpRequest() function.
    string response = handleFtpRequest(request, clientFtpSession);

    // If successful, put the response into 'responses[]' array of 'serverResponse' data structure
    // at the correct index and increment the count of 'serverResponse' data structure.
    if (serverResponse.count < MAX_NUMBER_OF_RESPONSES)
    {
        serverResponse.responses[serverResponse.count] = response;
        serverResponse.count++;
    }
    else
    {
        cout << "Error: No space for responses left." << endl;
        return;
    }
}


void handleCommandDirectory(ClientFtpSession& clientFtpSession, ServerResponse& serverResponse)
{
    // Check whether the user is logged in or not by calling isLoggedIn() function.
    if (!isLoggedIn(clientFtpSession))
    {
        // If the user is not logged in, stop client's FTP session.
        stopClientFTPSession(clientFtpSession);
        return;
    }

    // If the user is logged in, call handlePassive() function to get a data connection opened.
    handlePassive(clientFtpSession, serverResponse);

    // If the data connection is opened successfully, call handleNLST() function to receive directory list.
    handleNLIST(clientFtpSession, serverResponse);
}


void handleCommandGetFile(string filename, ClientFtpSession& clientFtpSession, ServerResponse& serverResponse)
{
    // Check whether the user is logged in or not by calling isLoggedIn() function.
    if (!isLoggedIn(clientFtpSession))
    {
        // If not logged in, stop client's FTP session.
        stopClientFTPSession(clientFtpSession);
        cout << "ERROR: User is not logged in. FTP session stopped." << endl;
        return;
    }

    // If logged in, call handleSize() to get the size of the file.
    int fileSize = -1;
    handleSize(filename, clientFtpSession, serverResponse, fileSize);

    // Check if the file size was successfully retrieved and is greater than zero.
    if (fileSize > 0)
    {
        // If successful, call handlePassive() to establish a data connection with the server.
        handlePassive(clientFtpSession, serverResponse);

        // If the data connection is opened successfully, call handleRETR() to receive file content.
        handleRETR(filename, fileSize, clientFtpSession, serverResponse);
    }
    else
    {
        cout << "ERROR: Failed to retrieve file size or invalid file size." << endl;
    }
}

 
void handlePassive(ClientFtpSession& clientFtpSession, ServerResponse& serverResponse)
{
    // Send a 'PASV' request message to ftp server and recieve the server response by calling 
    // handleFtpRequest() function.    
    string request = "PASV";
    string response = handleFtpRequest(request, clientFtpSession);

    // If successful, put the response into 'responses[]' array of 'serverResponse' data structure at
    // the correct index and increment the count of 'serverResponse' data structure.
    if (serverResponse.count < MAX_NUMBER_OF_RESPONSES)
    {
        serverResponse.responses[serverResponse.count] = response;
        serverResponse.count++;
    }
    else
    {
        cout << "Error: No space for responses left." << endl;
        return;
    }

    // If PASSIVE_ERROR_RESPONSE message is received stop client ftp sesion by
    // calling stopClientFtpSession() function,
    if (string(response) == PASSIVE_ERROR_RESPONSE)
    {
        stopClientFTPSession(clientFtpSession);
        return;
    }

    // If PASSIVE_SUCCESS_RESPONSE message is received, retrieve server ip address and data-connection 
    // listener port number form the response by calling getHostIPAndPortFromPassiveSuccessResponse().
    if (response.find("227") == 0) // error when trying "PASSIVE_SUCCESS_RESPONSE" so parse to find 227 at pos 0 instead
    {
        char hostIP[16]; // XXX.XXX.XXX.XXX + null terminator
        int hostPort = 0;

        //retrieve server ip address and data-connection  listener port number 
        //form the response by calling getHostIPAndPortFromPassiveSuccessResponse().
        getHostIPAndPortFromPassiveSuccessResponse(response.data(), hostIP, hostPort);

        //Connect to server's data port by calling connectToServer() function with retrieved ip and port number.
        connectToServer(clientFtpSession.dataSocket, hostIP, hostPort);

        // Recieve server response through 'controlSocket' of 'clientFtpSession' by calling
        // receiveFromServer() function.
        char buffer[FTP_RESPONSE_MAX_LENGTH]; //1024 in this case
        int receivedBytes = receiveFromServer(clientFtpSession.controlSocket, buffer, sizeof(buffer));

        string dataResponse;
        if (receivedBytes > 0)
        {
            dataResponse = string(buffer, receivedBytes);
        }

        // If successful, put the response into 'responses[]' array of 'serverResponse' data structure at
        // the correct index and increment the count of 'serverResponse' data structure.
        if (serverResponse.count < MAX_NUMBER_OF_RESPONSES)
        {
            serverResponse.responses[serverResponse.count] = dataResponse;
            serverResponse.count++;
        }
        else
        {
            cout << "Error: No space for responses left." << endl;
            stopClientFTPSession(clientFtpSession);
        }
    }
    // In any failure case, stop client's ftp session by calling stopClientFtpSession() function.
    else
    {
        stopClientFTPSession(clientFtpSession);
    }
}


void handleNLIST(ClientFtpSession& clientFtpSession, ServerResponse& serverResponse)
{
    // Send a 'NLST' request message to ftp server and recieve the server response by calling 
    // handleFtpRequest() function.
    string request = "NLST";
    string response = handleFtpRequest(request, clientFtpSession);

    // If successful, put the response into 'responses[]' array of 'serverResponse' data structure at
    // the correct index and increment the count of 'serverResponse' data structure.
    if (serverResponse.count < MAX_NUMBER_OF_RESPONSES)
    {
        serverResponse.responses[serverResponse.count] = response;
        serverResponse.count++;
    }
    else
    {
        cout << "Error: No space for responses left." << endl;
        return;
    }

    // some error, stop session
    if (response == NLST_INVALID_OPTION_ERROR_RESPONSE || response == NLST_UNAVAILABLE_ERROR_RERSPONSE)
    {
        stopClientFTPSession(clientFtpSession);
        return;
    }

    // If NLST_CONNECTION_CLOSE_RESPONSE message is received
    if (response.find("226") == 0) // putting "NLST_CONNECTION_CLOSE_RESPONSE" casued errors so parse to find 226 at pos 0
    {
        char buffer[4096]; // 4096 filesystem operations - matches common disk block sizes

        //call receiveFromServer() function to receive directory list from the server over the 'dataSocket' of 'clientFtpSession'
        int receivedBytes = receiveFromServer(clientFtpSession.dataSocket, buffer, sizeof(buffer));

        string directoryList;
        if (receivedBytes > 0)
        {
            directoryList = string(buffer, receivedBytes);
        }

        if (serverResponse.count < MAX_NUMBER_OF_RESPONSES)
        {
            //put the directory list into 'responses[]' array of 'serverResponse
            //data structure at the correct index and increment the count of 'serverResponse' data structure
            serverResponse.responses[serverResponse.count] = directoryList;
            serverResponse.count++;
        }
        else
        {
            cout << "Error: No space for responses left." << endl;
        }

        // close data connection from client side by calling disconnectFromServer() function
        disconnectFromServer(clientFtpSession.dataSocket);
    }
    //failed to get 226
    else
    {
        stopClientFTPSession(clientFtpSession);
    }
}


void handleSize(string filename, ClientFtpSession& clientFtpSession, ServerResponse& serverResponse, int& size)
{
    // Send a 'SIZE filename' request message to ftp server and recieve the server response by calling 
    // handleFtpRequest() function.
    string request = "SIZE " + filename;
    string response = handleFtpRequest(request, clientFtpSession);

    // If successful, put the response into 'responses[]' array of 'serverResponse' data structure at
    // the correct index and increment the count of 'serverResponse' data structure.
    if (serverResponse.count < MAX_NUMBER_OF_RESPONSES)
    {
        serverResponse.responses[serverResponse.count] = response;
        serverResponse.count++;
    }
    else
    {
        cout << "Error: No space for responses left." << endl;
        return;
    }

    // If FILE_SIZE_RESPONSE message is received, retrieve the size value from the message and assign it 
    // to 'size' parameter. 
    if (response.find("213") == 0) // FILE_SIZE_RESPONSE gives error so check if 213 is at pos 1 
    {
        string sizeStr;

        //finds where ( begins and ends )
        int start = response.find('(');
        int end = response.find(')');

        // Extract number inside '(%d)'
        sizeStr = response.substr(start + 1, end - start - 1);

        //string --> int
        size = stoll(sizeStr);
    }
    else
    {
        cout << "ERROR: Invalid FTP response for SIZE command." << endl;
        size = -1;
    }
}





void handleRETR(string filename, const int size, ClientFtpSession& clientFtpSession, ServerResponse& serverResponse)
{
    // see if user provided a filename
    if (filename.empty())
    {
        cout << "ERROR: Filename is empty." << endl;
        return;
    }

    // Send a 'RETR filename' request message to ftp server and recieve the server response by calling 
    // handleFtpRequest() function.
    string request = "RETR " + filename;
    string response = handleFtpRequest(request, clientFtpSession);


    // If successful, put the response into 'responses[]' array of 'serverResponse' data structure at
    // the correct index and increment the count of 'serverResponse' data structure.
    if (serverResponse.count < MAX_NUMBER_OF_RESPONSES)
    {
        serverResponse.responses[serverResponse.count] = response;
        serverResponse.count++;
    }
    else
    {
        cout << "Error: No space for responses left." << endl;
        return;
    }

    // If RETR_CONNECTION_CLOSE_RESPONSE message is received
    if (string(response) == RETR_CONNECTION_CLOSE_RESPONSE)
    {
        //open a new file with the filename in write only mode
        FILE* file = fopen(filename.c_str(), "wb");
        if (file == nullptr)
        {
            cout << "ERROR: Failed to open file for writing." << endl;
            return;
        }

        // buffer for recieving
        char buffer[DATA_SOCKET_RECEIVE_BUFFER_SIZE];
        int totalReceived = 0;

        // continue recieving and writing until 'size' number of bytes have been received and written.
        while (totalReceived < size)
        {
            // receive the file content DATA_SOCKET_RECEIVE_BUFFER_SIZE chunk each time over 'dataSocket' of 
            // 'clientFtpSession' from the server by calling receiveFromServer() function
            int bytesReceived = receiveFromServer(clientFtpSession.dataSocket, buffer, sizeof(buffer));
            if (bytesReceived <= 0)
            {
                cout << "ERROR: Failed to receive data or connection closed prematurely." << endl;
                fclose(file);
                disconnectFromServer(clientFtpSession.dataSocket); 
                return;
            }

            // write the received chunk of the file content into the opened file
            fwrite(buffer, 1, bytesReceived, file);
            totalReceived += bytesReceived;
        }

        //When full content of the file is received and written into the file close the file and the 
        // data connection by calling fclose() library function and disconnectFromSrver() function respectively.
        fclose(file);

        if (clientFtpSession.dataSocket != -1)
        {
            disconnectFromServer(clientFtpSession.dataSocket);
        }
        else
        {
            cout << "ERROR: Invalid data socket." << endl;
        }
    }
    else
    {
        cout << "ERROR: Invalid FTP response for RETR command." << endl;
    }
}


void getHostIPAndPortFromPassiveSuccessResponse(char* response, char* hostIP, int& hostPort)
{    
    //array to hold the 6 values we extarct
    int values[6];
    
    // find '(' since it marks the beggining of IP
    char* start = strchr(response, '(');
    
    // '(' not found, exit
    if (start == NULL) 
    {
        return; 
    }

    // move one over to get to the first number
    start++;

    // get the number thats before the ','
    char* number = strtok(start, ",");
    
    //go through for each of the sections and store it 
    for(int i = 0; i < 6; i++)
    {
        //make sure no garbage gets in
        if(number == nullptr)
        {
            return;
        }
        //store it
        values[i] = atoi(number);
        
        //move onto next num
        number = strtok(nullptr, ",");
    }

    /// Retrieve IP address of FTP Server from the response string into reference 'hostIP'.
    sprintf(hostIP, "%d.%d.%d.%d", values[0], values[1], values[2], values[3]); //formats the string into hostIP

    // Retrieve data-connection listening port number of FTP server from the response string into reference 'hostPort'.  
    // Port is calculated as (202 * 256 + 143) from the example response.
    hostPort = (values[4] * 256) + values[5]; //last two values 
}
