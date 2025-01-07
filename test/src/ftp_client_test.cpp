/**
 * @file ftp_client_test.cpp
 * @author Humayun Kabir, Instructor, CSCI 460, VIU
 * @version 1.0.0
 * @date June 24, 2020
 *
 */


#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ftp_client_test
#include <boost/test/unit_test.hpp>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "ftp_client_connection.h"
#include "ftp_client_session.h"
#include "ftp_client_command.h"
#include "ftp_client_ui.h"
#include "ftp_server_response.h"
#include "ftp_client_test_net_util.h"
#include "ftp_client_test_connection.h"
#include "ftp_client_test_session.h"

using namespace std;

/**
 * FTP Client Test Fixture
 * At the beginning of each test case, fixture's constructor is invloked
 * to start ftp server as a child process of the test program.
 * At the end of each test case, fixture's destructor is invoked to
 * stop the ftp server by terminating the child process.
 */
class FTPClientTestFixture {
	private:
		pid_t serverPID; // Remembers ftpserver process ID
	public:
		char* testHostIP;
		FTPClientTestFixture() {
			char *args[] = {const_cast<char*>("ftpserver"), const_cast<char*>("3030"), 0};
			char *env[] = {0};
			testHostIP = getTestHostIPAddress();
			serverPID = -1;
			
			serverPID = fork(); // Creates a child process
			if(serverPID == 0) {
				// Inside the child process.
				execve("./example/bin/ftpserver", args, env); // Child process is set to run ftpserver code
				exit(1);
			}	
			else { // Parent process continues
				if (serverPID == -1) {
					exit(1);
				}
				cout<<"FTP Server PID: "<<serverPID<<endl;
				sleep(3);
			}
			
		}	
		~FTPClientTestFixture() {
			if (serverPID != -1) {
				// Valid FTP Server PID, needs to stop the server by
				// sending signals to the server process.
				cout<<"Stopping FTP Server (PID: "<<serverPID<<")"<<endl;
				kill(serverPID, SIGTERM); // Sends terminate signal to ftpserver process
				sleep(2);
				kill(serverPID, SIGKILL); // Sends kill signal to ftpserver process
			}
		}
};

BOOST_FIXTURE_TEST_SUITE(ftp_client_deliverable1, FTPClientTestFixture)

/**
 * Test Case to test the functions in ftp_client_connection.cpp file.
 */
BOOST_AUTO_TEST_CASE(ftp_client_connection) {

	
	/*
	 * Test connecToServer() function.
	 */
	int socketDescriptor = -1;
	connectToServer(socketDescriptor, testHostIP, 3030);
	BOOST_CHECK_MESSAGE(socketDescriptor != -1 &&
						isTestSocketClosed(socketDescriptor) == false, "TEST: connectToServer()");
	char response[FTP_RESPONSE_MAX_LENGTH];
	memset(response, 0, FTP_RESPONSE_MAX_LENGTH);
	int count = receiveFromTestServer(socketDescriptor, response, FTP_RESPONSE_MAX_LENGTH);
	sleep(1);
	disconnectFromTestServer(socketDescriptor);
	

	/*
	 * Test disconnectFromServer() function.
	 */
	sleep(1);
	connectToTestServer(socketDescriptor, testHostIP, 3030);
	
	BOOST_REQUIRE(socketDescriptor != -1 &&
						isTestSocketClosed(socketDescriptor) == false);
	memset(response, 0, FTP_RESPONSE_MAX_LENGTH);
	count = receiveFromTestServer(socketDescriptor, response, FTP_RESPONSE_MAX_LENGTH);
	sleep(1);
	disconnectFromServer(socketDescriptor);
	BOOST_CHECK_MESSAGE(socketDescriptor == -1, "TEST: disconnectFromServer()");
	
	
	/*
	 * Test receiveFromServer() function.
	 */
	connectToTestServer(socketDescriptor, testHostIP, 3030);	
	BOOST_REQUIRE(socketDescriptor != -1 &&
						isTestSocketClosed(socketDescriptor) == false);
	
	memset(response, 0, FTP_RESPONSE_MAX_LENGTH);
	count = -1;
	count = receiveFromServer(socketDescriptor, response, FTP_RESPONSE_MAX_LENGTH);
	sleep(1);
	BOOST_CHECK_MESSAGE(count==19 &&
						 strncmp(response, CONNECTED_RESPONSE, count-1)==0,
						 "TEST: receiveFromServer() - connected");


	/*
	 * Test sendToServer() function.
	 */
	int sendCount= -1;
	sendCount = sendToServer(socketDescriptor, "Hello", 5 );
	memset(response, 0, FTP_RESPONSE_MAX_LENGTH);
	count = -1;
	count = receiveFromServer(socketDescriptor, response, FTP_RESPONSE_MAX_LENGTH);
	BOOST_CHECK_MESSAGE(sendCount == 5 && count == 25  &&
						strncmp(response, UNSUPPORTED_COMMAND_RESPONSE, count-1)==0, 
						"TEST: sendToServer() - unsupported command response");

	disconnectFromTestServer(socketDescriptor);
	
}

/**
 * Test Case to test functions in ftp_client_control.cpp.
 */
BOOST_AUTO_TEST_CASE(ftp_client_session) {
	/*
	 * Test startClientFTPSession() function.
	 */
	ClientFtpSession clientFtpSession;
		
	startClientFTPSession(testHostIP, 3030, clientFtpSession);
	
	BOOST_CHECK_MESSAGE(clientFtpSession.controlSocket > 0 &&
						isTestSocketClosed(clientFtpSession.controlSocket) == false, "TEST: startClientFTPSession()");

	
	
	int sendCount= -1;
	sendCount = sendToServer(clientFtpSession.controlSocket, "Hello", 5 );
	
	char response[FTP_RESPONSE_MAX_LENGTH];
	memset(response, 0, FTP_RESPONSE_MAX_LENGTH);
	int count = -1;
	count = receiveFromServer(clientFtpSession.controlSocket, response, FTP_RESPONSE_MAX_LENGTH);
	BOOST_CHECK_MESSAGE(sendCount == 5 && count == 25 && 
						strncmp(response, UNSUPPORTED_COMMAND_RESPONSE, count-1)==0, 
						"TEST: startClientFTPSession() - unsupported command response");

	/*
	 * Test stopClientFTPSession() function.
	 */
	stopClientFTPSession(clientFtpSession);
	
	BOOST_CHECK_MESSAGE(clientFtpSession.controlSocket  == -1 &&
						clientFtpSession.dataSocket  == -1 &&
						clientFtpSession.isUserAuthenticated  == false &&
						clientFtpSession.isLoggedIn  == false,
						"TEST: stopClientFTPSession()");

}

BOOST_AUTO_TEST_CASE(ftp_client_command) {
	
	ClientFtpSession clientFtpSession;
	startClientTestFTPSession(testHostIP, 3030, clientFtpSession);

	/*
	 * Test handleFtpRequest() function with invalid username.
	 */
	ServerResponse serverResponse;	
	char* response = nullptr;
	response = handleFtpRequest("Hello", clientFtpSession);
	BOOST_CHECK_MESSAGE(response != nullptr &&
						strncmp(response, UNSUPPORTED_COMMAND_RESPONSE, 5)==0, 
						"TEST: handleFtpRequest() - unsupported command");
	if(response != nullptr) {
		delete [] response;
		response = nullptr;
	}
					
	response = handleFtpRequest("USER csci460", clientFtpSession);
	BOOST_CHECK_MESSAGE(response != nullptr &&
						strcmp(response, USERNAME_OK_RESPONSE)==0, 
						"TEST: handleFtpRequest() - username ok");
	if(response != nullptr) {
		delete [] response;
		response = nullptr;
	}

	response = handleFtpRequest("PASS 460pass", clientFtpSession);
	BOOST_CHECK_MESSAGE(response != nullptr &&
						strncmp(response, LOGIN_RESPONSE, 27)==0, 
						"TEST: handleFtpRequest() - logged in");
	if(response != nullptr) {
		delete [] response;
		response = nullptr;
	}

	response = handleFtpRequest(string("QUIT"), clientFtpSession);
	BOOST_CHECK_MESSAGE(response != nullptr &&
						strcmp(response, QUIT_RESPONSE) == 0, 
						"TEST: handleFtpRequest() - quit");

	
	stopClientTestFTPSession(clientFtpSession);
	startClientTestFTPSession(testHostIP, 3030, clientFtpSession);
	
	/*
	 * Test handleCommandUser() function.
	 */
	string username= "csci460";
	serverResponse.count = 0;
	handleCommandUser(username, clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1 && 
						strcmp(serverResponse.responses[0].c_str(), USERNAME_OK_RESPONSE) == 0, 
						"TEST: handleCommandUser() - username ok");

	/*
	 * Test handleCommandPassword() fucntion.
	 */
	string password= "460pass";
	serverResponse.count = 0;
	handleCommandPassword(password, clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1 &&
						strcmp(serverResponse.responses[0].c_str(), LOGIN_RESPONSE) == 0, 
						"TEST: handleCommandPassword() - logged in");

	/*
	 * Test handleCommandQuit() function.
	 */
	serverResponse.count = 0;
	handleCommandQuit(clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1 &&
						strcmp(serverResponse.responses[0].c_str(), QUIT_RESPONSE) == 0,
						"TEST: handleCommandQuit() - quit");

	stopClientTestFTPSession(clientFtpSession);
	startClientTestFTPSession(testHostIP, 3030, clientFtpSession);

	/*
	 * Test interpretAndHandleUserCommand() function
	 */
	interpretAndHandleUserCommand("help", clientFtpSession, serverResponse);

	interpretAndHandleUserCommand("usr csci460", clientFtpSession, serverResponse);

	serverResponse.count = 0;
	interpretAndHandleUserCommand("user csci460", clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1 && 
						strcmp(serverResponse.responses[0].c_str(), USERNAME_OK_RESPONSE) == 0, 
						"TEST: interpretAndHandleCommandUser() - username ok");

	serverResponse.count = 0;
	interpretAndHandleUserCommand("pass 460pass", clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1 &&
						strcmp(serverResponse.responses[0].c_str(), LOGIN_RESPONSE) == 0, 
						"TEST: interpretAndHandleCommandUser() - logged in");

	serverResponse.count = 0;
	interpretAndHandleUserCommand("quit", clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1 &&
						strcmp(serverResponse.responses[0].c_str(), QUIT_RESPONSE) == 0,
						"TEST: interpretAndHandleCommandUser() - quit");

}
BOOST_AUTO_TEST_SUITE_END()



BOOST_FIXTURE_TEST_SUITE(ftp_client_deliverable2, FTPClientTestFixture) 

BOOST_AUTO_TEST_CASE(ftp_client_command) {

	ClientFtpSession clientFtpSession;
	ServerResponse serverResponse;
	startClientTestFTPSession(testHostIP, 3030, clientFtpSession);
	
	
	string username= "csci460";
	serverResponse.count = 0;
	handleCommandUser(username, clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1 && 
						strcmp(serverResponse.responses[0].c_str(), USERNAME_OK_RESPONSE) == 0, 
						"TEST: handleCommandUser() - username ok");

	/*
	 * Test handleCommandPassword() fucntion.
	 */
	string password= "460pass";
	serverResponse.count = 0;
	handleCommandPassword(password, clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1 &&
						strcmp(serverResponse.responses[0].c_str(), LOGIN_RESPONSE) == 0, 
						"TEST: handleCommandPassword() - logged in");


	/*
	 * Test handleCommandPrintDirectory() function.
	 */
	serverResponse.count = 0;
	handleCommandPrintDirectory(clientFtpSession, serverResponse);
	char currentDir[1024];
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != nullptr) {
		BOOST_CHECK_MESSAGE(serverResponse.count == 1 &&
				strncmp(serverResponse.responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: handleCommandPrintDirectory()");
		BOOST_TEST_MESSAGE(serverResponse.responses[0].insert(0, "TEST: Current Directory: "));
	}	
	
	
	/*
	 * Test handleCommandChangeDirectoryUp() function, while CDUP is not permitted.
	 */
	serverResponse.count = 0;
	handleCommandChangeDirectoryUp(clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1 &&
						strcmp(serverResponse.responses[0].c_str(),CDUP_FAIL_RESPONSE) == 0,
						"TEST: handleCommandChangeDirectoryUp() - cdup fail");

	
	/*
	 * Test PWD after unsuccessful CDUP
	 */
	serverResponse.count = 0;
	handleCommandPrintDirectory(clientFtpSession, serverResponse);
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != nullptr) {
		BOOST_CHECK_MESSAGE(serverResponse.count == 1 &&
				strncmp(serverResponse.responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: handleCommandPrintDirectory() - after failed cdup");
		BOOST_TEST_MESSAGE(serverResponse.responses[0].insert(0, "TEST: Current Directory: "));
	}	


		
	/*
	 * Test handleCommandChangeDirectory() function with invalid path.
	 */
	serverResponse.count = 0;
	handleCommandChangeDirectory("nodir", clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1 &&
						strcmp(serverResponse.responses[0].c_str(),CWD_FAIL_RESPONSE) == 0,
						"TEST: handleCommandChangeDirectory() - cwd fail");

	/*
	 * Test PWD after unsuccessful CWD
	 */
	serverResponse.count = 0;
	handleCommandPrintDirectory(clientFtpSession, serverResponse);
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != nullptr) {
		BOOST_CHECK_MESSAGE(serverResponse.count == 1 &&
				strncmp(serverResponse.responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: handleCommandPrintDirectory() - failed cwd");
		BOOST_TEST_MESSAGE(serverResponse.responses[0].insert(0, "TEST: Current Directory: "));
	}	


	/*
	 * Test handleCommandChangeDirectory() function with invalid path.
	 */
	serverResponse.count = 0;
	handleCommandChangeDirectory("./", clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1 &&
						strcmp(serverResponse.responses[0].c_str(), INVALID_PATH_RESPONSE) == 0,
			"TEST: handleCommandChangeDirectory() - invalid path");

	/*
	 * Test PWD after unsuccessful CWD
	 */
	serverResponse.count = 0;
	handleCommandPrintDirectory(clientFtpSession, serverResponse);
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != nullptr) {
		BOOST_CHECK_MESSAGE(serverResponse.count == 1 &&
				strncmp(serverResponse.responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: handleCommandPrintDirectory() - after failed cwd");
		BOOST_TEST_MESSAGE(serverResponse.responses[0].insert(0, "TEST: Current Directory: "));
	}	
	
	
	/*
	 * Test handleCommandChangeDirectory() function with invalid path.
	 */
	serverResponse.count = 0;
	handleCommandChangeDirectory("../", clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1 &&
						strcmp(serverResponse.responses[0].c_str(), INVALID_PATH_RESPONSE) == 0,
			"TEST: handleCommandChangeDirectory() - invalid path");

	/*
	 * Test PWD after unsuccessful CWD
	 */
	serverResponse.count = 0;
	handleCommandPrintDirectory(clientFtpSession, serverResponse);
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != nullptr) {
		BOOST_CHECK_MESSAGE(serverResponse.count == 1 &&
				strncmp(serverResponse.responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: handleCommandPrintDirectory() - after failed cwd");
		BOOST_TEST_MESSAGE(serverResponse.responses[0].insert(0, "TEST: Current Directory: "));
	}	

	/*
	 * Test handleCommandChangeDirectory() function with invalid path.
	 */
	serverResponse.count = 0;
	handleCommandChangeDirectory("resource/../..", clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1 &&
						strcmp(serverResponse.responses[0].c_str(), INVALID_PATH_RESPONSE) == 0,
						"TEST: handleCommandChangeDirectory() - invalid path");

	/*
	 * Test PWD after unsuccessful CWD
	 */
	serverResponse.count = 0;
	handleCommandPrintDirectory(clientFtpSession, serverResponse);
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != nullptr) {
		BOOST_CHECK_MESSAGE(serverResponse.count == 1 &&
				strncmp(serverResponse.responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: handleCommandPrintDirectory() - after failed cwd");
		BOOST_TEST_MESSAGE(serverResponse.responses[0].insert(0, "TEST: Current Directory: "));
	}	

	/*
	 * Test handleCommandChangeDirectory() function with a valid path.
	 */
	serverResponse.count = 0;
	handleCommandChangeDirectory("resource", clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1 &&
						strcmp(serverResponse.responses[0].c_str(),CHANGE_DIRECTORY_RESPONSE) == 0,
						"TEST: handleCommandChangeDirectory() - change directory");

	/*
	 * Test PWD after successful CWD
	 */
	serverResponse.count = 0;
	memset(currentDir, 0, 1024);
	handleCommandPrintDirectory(clientFtpSession, serverResponse);
	if(getcwd(currentDir, 1024) != nullptr) {
		BOOST_CHECK_MESSAGE(serverResponse.count == 1 &&
				strncmp(serverResponse.responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: handleCommandPrintDirectory() - after successful cwd");
		BOOST_TEST_MESSAGE(serverResponse.responses[0].insert(0, "TEST: Current Directory: "));
	}	


	/*
	 * Test handleCommandChangeDirectoryUp() function, while CDUP is permitted.
	 */
	serverResponse.count = 0;
	handleCommandChangeDirectoryUp(clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1 &&
						strcmp(serverResponse.responses[0].c_str(),CHANGE_TO_PARENT_DIRECTORY_RESPONSE) == 0,
						"TEST: handleCommandChangeDirectoryUp() - change to parent directory");

	/*
	 * Test PWD after CDUP
	 */
	serverResponse.count = 0;
	memset(currentDir, 0, 1024);
	handleCommandPrintDirectory(clientFtpSession, serverResponse);
	if(getcwd(currentDir, 1024) != nullptr) {
		BOOST_CHECK_MESSAGE(serverResponse.count == 1 &&
				strncmp(serverResponse.responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: handleCommandPrintDirectory() - after successful cdup");
		BOOST_TEST_MESSAGE(serverResponse.responses[0].insert(0, "TEST: Current Directory: "));
	}	

	/*
	 * Test getHostIPAndPortFromPAssiveSuccessResponse() function.
	 */
	char hostIP[20];
	memset(hostIP, 0, 20);
	int  hostPort = -1;
	const int RESPONSE_SIZE = 50;
	char response[RESPONSE_SIZE];
	memset(response, 0, RESPONSE_SIZE);
	strcpy(response, "227 Entering Passive Mode (192,168,1,65,202,143).");
	getHostIPAndPortFromPassiveSuccessResponse(response, hostIP, hostPort);

	BOOST_CHECK_MESSAGE(strncmp(hostIP, "192.168.1.65", 12) == 0, "TEST: getHostIPAndPortFromPassiveSuccessResponse() - IP");
	BOOST_CHECK_MESSAGE(hostPort ==51855, "TEST: getHostIPAndPortFromPassiveSuccessResponse() - Port");


	/*
	 * Test handlePassive() function
	 */
	serverResponse.count = 0;
	handlePassive(clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 2 &&
						strncmp(serverResponse.responses[0].c_str(), PASSIVE_SUCCESS_RESPONSE, 25) == 0 &&
						strncmp(serverResponse.responses[1].c_str(), DATA_CONNECTION_SUCCESS_RESPONSE, 
													strlen(DATA_CONNECTION_SUCCESS_RESPONSE)) == 0 && 
						clientFtpSession.dataSocket > 0,
						"TEST: handlePassive() - data connection open");


	/*
	 * Test handleNLIST() function
	 */
	serverResponse.count = 0;
	handleNLIST(clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 2 &&
						strncmp(serverResponse.responses[0].c_str(), NLST_CONNECTION_CLOSE_RESPONSE,21) == 0,
						"TEST: handleNLST() - data connection close");
	BOOST_CHECK_MESSAGE(serverResponse.responses[1].empty() == false, 
						serverResponse.responses[1].insert(0, "TEST: handleNLST() - Directory List\n"));

						
	/*
	 * Test handleCommandDirectory() function.
	 */
	serverResponse.count = 0;
	handleCommandDirectory(clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 4 &&
						strncmp(serverResponse.responses[0].c_str(), PASSIVE_SUCCESS_RESPONSE, 25)==0,
						"TEST: handleCommandDirectory() - Response passive success");
	BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[1].c_str(), DATA_CONNECTION_SUCCESS_RESPONSE, 
						strlen(DATA_CONNECTION_SUCCESS_RESPONSE))==0,
						"TEST: handleCommandDirectory() - data donnection open");
	BOOST_CHECK_MESSAGE(serverResponse.responses[3].empty() == false, 
			serverResponse.responses[3].insert(0, "TEST: handleCommandDirectory() - Directory List\n"));

	BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[2].c_str(), NLST_CONNECTION_CLOSE_RESPONSE,21)==0,
			"TEST: handleCommandDirectory() - data connection close");
	
	/*
	 * Test handleCommandChangeDirectory() function with a valid path.
	 */
	serverResponse.count = 0;
	handleCommandChangeDirectory("resource", clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1 &&
				 strncmp(serverResponse.responses[0].c_str(),CHANGE_DIRECTORY_RESPONSE, strlen(CHANGE_DIRECTORY_RESPONSE)) == 0, 
				 "TEST - handleCommandChangeDirectory() - 'resource'");

	
	/*
	 * Test handleSize() function with invalid filename.
	 */
	serverResponse.count = 0;
	int fileSize = -1;
	handleSize("nofile", clientFtpSession, serverResponse, fileSize);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1 && fileSize == -1 && 
						strncmp(serverResponse.responses[0].c_str(), RETR_UNAVAILABLE_ERROR_RESPONSE, 
						strlen(RETR_UNAVAILABLE_ERROR_RESPONSE))==0,
						"TEST: handleSize() - file unavailable");

	/*
	 * Test handleSize() function with valid filename.
	 */
	serverResponse.count = 0;
	fileSize = -1;
	handleSize("duck.jpeg", clientFtpSession, serverResponse, fileSize);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1 && fileSize == 5594 && 
						strncmp(serverResponse.responses[0].c_str(), FILE_SIZE_RESPONSE, 13)==0,
						"TEST: handleSize() - file size");

	/*
	 * Call handlePassive() to open data connection
	 */
	serverResponse.count = 0;
	handlePassive(clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 2 &&
						strncmp(serverResponse.responses[0].c_str(), PASSIVE_SUCCESS_RESPONSE, 25) == 0 &&
						strncmp(serverResponse.responses[1].c_str(), DATA_CONNECTION_SUCCESS_RESPONSE, 
													strlen(DATA_CONNECTION_SUCCESS_RESPONSE)) == 0 && 
						clientFtpSession.dataSocket > 0,
						"TEST: handlePassive() - data connection opened");

	/*
	 * Test handleRETR() function
	 */
	serverResponse.count = 0;
	const char* fname = "duck.jpeg";
	remove(fname);
	handleRETR(fname, fileSize, clientFtpSession, serverResponse);
	struct stat statbuf;
	stat(fname, &statbuf);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1 && access(fname, F_OK) == 0 && statbuf.st_size == 5594 && 
						strncmp(serverResponse.responses[0].c_str(), RETR_CONNECTION_CLOSE_RESPONSE, 
						strlen(RETR_CONNECTION_CLOSE_RESPONSE))==0,
						"TEST: handleRETR() - Response file");
	remove(fname);
	
	
	/*
	 * Test handleCommandGetFile() function with invalid filename.
	 */
	serverResponse.count = 0;
	handleCommandGetFile("nofile", clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1 && 
						strncmp(serverResponse.responses[0].c_str(), RETR_UNAVAILABLE_ERROR_RESPONSE, 
						strlen(RETR_UNAVAILABLE_ERROR_RESPONSE))==0,
						"TEST: handleCommandGetFile() - Response file unavailable");

	/*
	 * Test handleCommandGetFile() function with valid filename.
	 */
	serverResponse.count = 0;
	handleCommandGetFile(fname, clientFtpSession, serverResponse);
	memset(&statbuf, 0, sizeof(statbuf));
	stat(fname, &statbuf);
	BOOST_CHECK_MESSAGE(serverResponse.count == 4 &&
						strncmp(serverResponse.responses[0].c_str(), FILE_SIZE_RESPONSE, 13) == 0 &&
						strncmp(serverResponse.responses[1].c_str(), PASSIVE_SUCCESS_RESPONSE, 25) == 0 &&
						strncmp(serverResponse.responses[2].c_str(), DATA_CONNECTION_SUCCESS_RESPONSE, 
								strlen(DATA_CONNECTION_SUCCESS_RESPONSE)) == 0 &&
						strncmp(serverResponse.responses[3].c_str(), RETR_CONNECTION_CLOSE_RESPONSE,40) == 0 &&
						access("duck.jpeg", F_OK) != -1 &&
						statbuf.st_size == 5594 ,
						"TEST: handleCommandGetFile() - Response file");
	remove(fname);	


	/*
	 * Test handleCommandChangeDirectoryUp() function, while CDUP is permitted.
	 */
	serverResponse.count = 0;
	handleCommandChangeDirectoryUp(clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1 &&
				strncmp(serverResponse.responses[0].c_str(),CHANGE_TO_PARENT_DIRECTORY_RESPONSE, 
				strlen(CHANGE_TO_PARENT_DIRECTORY_RESPONSE)) == 0,
				"TEST: Change current directory to parent directory");


	/*
	 * Test handleCommandQuit() function.
	 */
	serverResponse.count = 0;
	handleCommandQuit(clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1 &&
						strncmp(serverResponse.responses[0].c_str(), QUIT_RESPONSE, 23) == 0 &&
						clientFtpSession.controlSocket  == -1 &&
						clientFtpSession.dataSocket  == -1 &&
						clientFtpSession.isUserAuthenticated  == false &&
						clientFtpSession.isLoggedIn  == false,
						"TEST: handleCommandQuit() - Response quit");
}



BOOST_AUTO_TEST_CASE(ftp_client_command_interpret_and_handle) {
	
	ClientFtpSession clientFtpSession;
	ServerResponse serverResponse;

	startClientFTPSession(testHostIP, 3030, clientFtpSession);
	BOOST_CHECK_MESSAGE(clientFtpSession.controlSocket  > 0 &&
						clientFtpSession.dataSocket  == -1 &&
						clientFtpSession.isUserAuthenticated  == false &&
						clientFtpSession.isLoggedIn  == false,
						"TEST: FTP Session started");


	interpretAndHandleUserCommand("user csci460", clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1 &&
						strncmp(serverResponse.responses[0].c_str(), USERNAME_OK_RESPONSE,33)==0, 
						"TEST: interpretAndHandleUserCommand() - 'user csci460'");



	serverResponse.count = 0;
	interpretAndHandleUserCommand("pass 460pass", clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1  && 
						strncmp(serverResponse.responses[0].c_str(), LOGIN_RESPONSE,33)==0, 
						"TEST: interpretAndHandleUserCommand() - 'pass 460pass'");



	serverResponse.count = 0;
	interpretAndHandleUserCommand("pwd", clientFtpSession, serverResponse);
	char currentDir[1024];
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != nullptr) {
		BOOST_CHECK_MESSAGE(serverResponse.count == 1 && 
							strncmp(serverResponse.responses[0].c_str(), currentDir, strlen(currentDir))==0,
							"TEST: interpretAndHandleUserCommand() - 'pwd'");
	}	
	
	


	serverResponse.count = 0;
	interpretAndHandleUserCommand("dir", clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 4 &&
						strncmp(serverResponse.responses[0].c_str(), PASSIVE_SUCCESS_RESPONSE, 25) == 0 &&
						strncmp(serverResponse.responses[1].c_str(), DATA_CONNECTION_SUCCESS_RESPONSE, 
								strlen(DATA_CONNECTION_SUCCESS_RESPONSE)) == 0 &&
								strncmp(serverResponse.responses[2].c_str(), NLST_CONNECTION_CLOSE_RESPONSE,21) == 0 &&
								serverResponse.responses[3].empty() == false,
								serverResponse.responses[3].insert(0, "TEST: interpretAndHandleUserCommand() - 'dir'\n"));

		

	serverResponse.count = 0;
	interpretAndHandleUserCommand("cwd resource", clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1 && 
						strncmp(serverResponse.responses[0].c_str(),CHANGE_DIRECTORY_RESPONSE, strlen(CHANGE_DIRECTORY_RESPONSE)) == 0,
						"TEST: interpretAndHandleUserCommand() - 'cwd resource'");




	serverResponse.count = 0 ;
	interpretAndHandleUserCommand("pwd", clientFtpSession, serverResponse);
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != nullptr) {
		BOOST_CHECK_MESSAGE(serverResponse.count == 1 && 
							strncmp(serverResponse.responses[0].c_str(), currentDir, strlen(currentDir))==0,
							"TEST: interpretAndHandleUserCommand() - 'pwd' after 'cwd resource'");
	}	
	
	

	serverResponse.count = 0;
	remove("duck.jpeg");	
	interpretAndHandleUserCommand("get duck.jpeg", clientFtpSession, serverResponse);
	struct stat statbuf2;
	stat("duck.jpeg", &statbuf2);
	BOOST_CHECK_MESSAGE(serverResponse.count == 4 && 
						strncmp(serverResponse.responses[0].c_str(), FILE_SIZE_RESPONSE, 13) == 0 &&
						strncmp(serverResponse.responses[1].c_str(), PASSIVE_SUCCESS_RESPONSE, 25) == 0 &&
						strncmp(serverResponse.responses[2].c_str(), DATA_CONNECTION_SUCCESS_RESPONSE, 
								strlen(DATA_CONNECTION_SUCCESS_RESPONSE)) == 0 &&
						access("duck.jpeg", F_OK) != -1 &&
       					statbuf2.st_size == 5594 && 
						strncmp(serverResponse.responses[3].c_str(), RETR_CONNECTION_CLOSE_RESPONSE,40)==0,
						"TEST: interpretAndHandleUserCommand( ) - 'get duck.jpeg'");


	remove("duck.jpeg");	

	serverResponse.count = 0;
	interpretAndHandleUserCommand("cdup", clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1 && 
						strncmp(serverResponse.responses[0].c_str(),CHANGE_TO_PARENT_DIRECTORY_RESPONSE, 
								strlen(CHANGE_TO_PARENT_DIRECTORY_RESPONSE)) == 0,
						"TEST: interpretAndHandleUserCommand() - 'cdup'");

	serverResponse.count = 0;
	interpretAndHandleUserCommand("pwd", clientFtpSession, serverResponse);
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != nullptr) {
		BOOST_CHECK_MESSAGE(serverResponse.count == 1 && 
							strncmp(serverResponse.responses[0].c_str(), currentDir, strlen(currentDir))==0,
							"TEST: interpretAndHandleUserCommand() - 'pwd' after 'cdup'");
	}	
	

	serverResponse.count = 0;
	interpretAndHandleUserCommand("quit", clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1 && 
						strncmp(serverResponse.responses[0].c_str(), QUIT_RESPONSE, 23) == 0 &&
						clientFtpSession.controlSocket  == -1 &&
						clientFtpSession.dataSocket  == -1 &&
						clientFtpSession.isUserAuthenticated  == false &&
						clientFtpSession.isLoggedIn  == false,
						"TEST: interpretAndHandleUserCommand() - 'quit'");

}

BOOST_AUTO_TEST_SUITE_END()

