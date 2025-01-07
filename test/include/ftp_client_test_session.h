/**
 * @file ftp_client_session.h
 * @author Humayun Kabir, Instructor, CSCI 460, VIU
 * @version 1.0.1
 * @date June 08, 2024
 *
 */

#ifndef __FTP_CLINET_TEST_SESSION_HEADER__
#define __FTP_CLINET_TEST_SESSION_HEADER__

#include "ftp_client_session.h"

void startClientTestFTPSession(const char* serverIP, int serverPort, ClientFtpSession& clientFtpSession);


void stopClientTestFTPSession(ClientFtpSession& clientFtpSession) ;

#endif





