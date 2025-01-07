/**
 *  @file ftp_client_ui.h
 *  @author Humayun Kabir, Instructor, CSCI 460, VIU
 *  @version 1.0.0
 *  @date May 29, 2024
 */

#ifndef __FTP_CLIENT_UI_HEADER__
#define __FTP_CLIENT_UI_HEADER__

#include <string>

#define FTP_CLIENT_PROMT "CSCI460FTP>>"


void getUserCommand(std::string& userCommand);
// Display FTP_CLIENT_PROMT as follows:
//      CSCI460FTP>>
// Clear current content of 'userCommand' string.
// Read user command from the prompt. You can use getline() function for this.


void showFtpResponse(std::string response); 
// Displays server response to the user.
// For example, success response of user command
//      CSCI460FTP>>user csci460
// should be displayes as 
//      331 Username okay, need password.
//

#endif
