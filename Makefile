#Makefile for CSCI 460 Project 1

#Students should clean and build using following make calls
#	make clean
#	make

#Instructor should clean and build using following make calls
#	make clean instructor=yes
#	make instructor=yes

CC = g++
WFLAG = -Wall
ifndef instructor
DFLAG = -ggdb
endif
CFLAG = -c


SOURCES = src
INCLUDES = include
BUILDS = build
BINARY = bin
TESTS = test
EXAMPLE = example



all: ${BINARY}/ftpclient ${TESTS}/${BINARY}/ftpclienttest


${BINARY}/ftpclient: ${BUILDS}/ftp_client.o \
			${BUILDS}/ftp_client_ui.o \
			${BUILDS}/ftp_client_session.o \
			${BUILDS}/ftp_client_connection.o \
			${BUILDS}/ftp_client_command.o
	${CC} ${WFLAG} ${DFLAG} -o ${BINARY}/ftpclient \
			${BUILDS}/ftp_client.o \
			${BUILDS}/ftp_client_ui.o \
			${BUILDS}/ftp_client_session.o \
			${BUILDS}/ftp_client_connection.o \
			${BUILDS}/ftp_client_command.o

${TESTS}/${BINARY}/ftpclienttest: ${TESTS}/${BUILDS}/ftp_client_test.o \
		${TESTS}/${BUILDS}/ftp_client_test_net_util.o \
		${TESTS}/${BUILDS}/ftp_client_test_connection.o \
		${TESTS}/${BUILDS}/ftp_client_test_session.o \
		${BUILDS}/ftp_client_connection.o \
		${BUILDS}/ftp_client_session.o \
		${BUILDS}/ftp_client_command.o \
		${BUILDS}/ftp_client_ui.o
	${CC} ${WFLAG} -o ${TESTS}/${BINARY}/ftpclienttest \
		${TESTS}/${BUILDS}/ftp_client_test.o \
		${TESTS}/${BUILDS}/ftp_client_test_net_util.o \
		${TESTS}/${BUILDS}/ftp_client_test_connection.o \
		${TESTS}/${BUILDS}/ftp_client_test_session.o \
		${BUILDS}/ftp_client_connection.o \
		${BUILDS}/ftp_client_session.o \
		${BUILDS}/ftp_client_command.o \
		${BUILDS}/ftp_client_ui.o \
		-lboost_unit_test_framework


${BUILDS}/ftp_client.o: ${SOURCES}/ftp_client.cpp \
			${INCLUDES}/ftp_client_ui.h \
       		${INCLUDES}/ftp_client_session.h \
			${INCLUDES}/ftp_client_command.h	
	${CC} ${WFLAG} ${DFLAG} ${CFLAG} -I${INCLUDES} -o ${BUILDS}/ftp_client.o ${SOURCES}/ftp_client.cpp


${BUILDS}/ftp_client_ui.o: ${SOURCES}/ftp_client_ui.cpp \
			${INCLUDES}/ftp_client_ui.h
	${CC} ${WFLAG} ${DFLAG} ${CFLAG} -I${INCLUDES} -o ${BUILDS}/ftp_client_ui.o ${SOURCES}/ftp_client_ui.cpp


${BUILDS}/ftp_client_session.o: ${SOURCES}/ftp_client_session.cpp \
				${INCLUDES}/ftp_client_session.h \
				${INCLUDES}/ftp_client_ui.h \
				${INCLUDES}/ftp_client_command.h \
				${INCLUDES}/ftp_client_connection.h \
				${INCLUDES}/ftp_server_response.h
	${CC} ${WFLAG} ${DFLAG} ${CFLAG} -I${INCLUDES} -o ${BUILDS}/ftp_client_session.o ${SOURCES}/ftp_client_session.cpp


${BUILDS}/ftp_client_connection.o: ${SOURCES}/ftp_client_connection.cpp ${INCLUDES}/ftp_client_connection.h
	${CC} ${WFLAG} ${DFLAG} ${CFLAG} -I${INCLUDES} -o ${BUILDS}/ftp_client_connection.o ${SOURCES}/ftp_client_connection.cpp


${BUILDS}/ftp_client_command.o: ${SOURCES}/ftp_client_command.cpp \
				${INCLUDES}/ftp_client_command.h \
				${INCLUDES}/ftp_client_ui.h \
				${INCLUDES}/ftp_client_connection.h \
				${INCLUDES}/ftp_client_session.h \
				${INCLUDES}/ftp_server_response.h
	${CC} ${WFLAG} ${DFLAG} ${CFLAG} -I${INCLUDES} -o ${BUILDS}/ftp_client_command.o ${SOURCES}/ftp_client_command.cpp



ifdef instructor
${TESTS}/${BUILDS}/ftp_client_test.o: ${TESTS}/${SOURCES}/ftp_client_test.cpp \
			${TESTS}/${INCLUDES}/ftp_client_test_net_util.h \
			${TESTS}/${INCLUDES}/ftp_client_test_connection.h \
			${TESTS}/${INCLUDES}/ftp_client_test_session.h \
			${INCLUDES}/ftp_client_connection.h \
			${INCLUDES}/ftp_client_session.h \
			${INCLUDES}/ftp_client_command.h \
			${INCLUDES}/ftp_server_response.h 
	${CC} ${WFLAG} ${CFLAG} -I${INCLUDES} -I${TESTS}/${INCLUDES} -o ${TESTS}/${BUILDS}/ftp_client_test.o \
		${TESTS}/${SOURCES}/ftp_client_test.cpp


${TESTS}/${BUILDS}/ftp_client_test_net_util.o: ${TESTS}/${SOURCES}/ftp_client_test_net_util.cpp \
	${TESTS}/${INCLUDES}/ftp_client_test_net_util.h
	${CC} ${WFLAG} ${CFLAG} -I${INCLUDES} -I${TESTS}/${INCLUDES} -o ${TESTS}/${BUILDS}/ftp_client_test_net_util.o \
		${TESTS}/${SOURCES}/ftp_client_test_net_util.cpp

${TESTS}/${BUILDS}/ftp_client_test_connection.o: ${TESTS}/${SOURCES}/ftp_client_test_connection.cpp \
	${TESTS}/${INCLUDES}/ftp_client_test_connection.h
	${CC} ${WFLAG} ${CFLAG} -I${INCLUDES} -I${TESTS}/${INCLUDES} -o ${TESTS}/${BUILDS}/ftp_client_test_connection.o \
		${TESTS}/${SOURCES}/ftp_client_test_connection.cpp

${TESTS}/${BUILDS}/ftp_client_test_session.o: ${TESTS}/${SOURCES}/ftp_client_test_session.cpp \
	${TESTS}/${INCLUDES}/ftp_client_test_connection.h ${TESTS}/${INCLUDES}/ftp_client_test_session.h 
	${CC} ${WFLAG} ${CFLAG} -I${INCLUDES} -I${TESTS}/${INCLUDES} -o ${TESTS}/${BUILDS}/ftp_client_test_session.o \
		${TESTS}/${SOURCES}/ftp_client_test_session.cpp

endif

.PHONY: clean \
	run-client \
	run-example-client \
	run-example-server \
	test test-deliverable1 \
	test-deliverabl2 \
	test-example \
	test-example-deliverable1 \
	test-example-deliverabl2 \
	list-server kill-server


run-client: ${BINARY}/ftpclient
	${BINARY}/ftpclient `hostname -I` 2020


run-example-client: ${EXAMPLE}/${BINARY}/ftpclient
	${EXAMPLE}/${BINARY}/ftpclient `hostname -I` 2020


run-example-server: ${EXAMPLE}/${BINARY}/ftpserver
	${EXAMPLE}/${BINARY}/ftpserver 2020



test-deliverable1: ${TESTS}/${BINARY}/ftpclienttest ${EXAMPLE}/${BINARY}/ftpserver
	${TESTS}/${BINARY}/ftpclienttest --log_level=all --run_test=ftp_client_deliverable1

test-deliverable2: ${TESTS}/${BINARY}/ftpclienttest ${EXAMPLE}/${BINARY}/ftpserver
	${TESTS}/${BINARY}/ftpclienttest --log_level=all --run_test=ftp_client_deliverable2

test: test-deliverable1 test-deliverable2




test-example-deliverable1: ${TESTS}/${EXAMPLE}/${BINARY}/ftpclienttest ${EXAMPLE}/${BINARY}/ftpserver
	${TESTS}/${EXAMPLE}/${BINARY}/ftpclienttest --log_level=all --run_test=ftp_client_deliverable1

test-example-deliverable2: ${TESTS}/${EXAMPLE}/${BINARY}/ftpclienttest ${EXAMPLE}/${BINARY}/ftpserver
	${TESTS}/${EXAMPLE}/${BINARY}/ftpclienttest --log_level=all --run_test=ftp_client_deliverable2

test-example: test-example-deliverable1 test-example-deliverable2


clean:
	@-rm -rf --verbose ${BINARY}/*
	@-rm -rf --verbose ${TESTS}/${BINARY}/*
	@-rm -rf --verbose ${BUILDS}/*
ifdef instructor
	@-rm -rf --verbose ${EXAMPLE}/${BINARY}/ftpclient	
	@-rm -rf --verbose ${TESTS}/${EXAMPLE}/${BINARY}/ftpclienttest
	@-rm -rf --verbose ${TESTS}/${BUILDS}/*
endif


list-server:
	@-ps -aux | grep ftpserver | grep -v grep

kill-server:
	@-killall -9 ftpserver

