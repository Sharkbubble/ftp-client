/**
 *  @file ftp_client_ui.cpp
 * @author Vladislav Mazur, 658453550, NO1, CSCI 460, VIU
 * @version 1.0.0
 * @date January 13, 2024
 *
 * Describe the major functionalities that are performed by the code in this file.
 *
 */


//Include required library and custom header files.
#include "../include/ftp_client_ui.h"
#include <iostream>

using namespace std;


//Implement all the functions prototyped in the header file ftp_client_ui.h

void getUserCommand(std::string& userCommand)
{
    // Display FTP_CLIENT_PROMT as follows:
    //      CSCI460FTP>>
    cout << "CSCI460FTP>>";

    // Clear current content of 'userCommand' string.
    userCommand.clear();

    // Read user command from the prompt. You can use getline() function for this.
    getline(cin, userCommand);
}

void showFtpResponse(std::string response)
{
    // Displays server response to the user.
    cout << response << endl;

    // For example, success response of user command
    //      CSCI460FTP>>user csci460
    // should be displayes as 
    //      331 Username okay, need password.
}