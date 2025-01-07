/**
 * @file ftp_client_test_connection.h
 * @author Humayun Kabir, Instructor, CSCI 460, VIU
 * @version 1.0.1
 * @date June 08, 2024
 *
 */

#ifndef __FTP_CLINET_TEST_CONNECTION_HEADER__
#define __FTP_CLINET_TEST_CONNECTION_HEADER__

void connectToTestServer(int& socketDescriptor, const char* serverIP, int serverPort) ;

void disconnectFromTestServer(int& socketDescriptor) ;

bool isTestSocketClosed(const int socketDescriptor) ;

int sendToTestServer(int sockDescriptor, const char* message, int messageLength) ;

int receiveFromTestServer(int sockDescriptor, char* message, int messageLength) ;

#endif