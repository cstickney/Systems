/*
 ============================================================================
 Name        : dayserve.c
 Author      : Chris Stickney
 Description : dayserve is a server process that listens on an ephemeral TCP port for one connection.
 When it receives a connection, it logs the hostname of the client to stdout and writes the current date and time to the client via the socket connection.
 See library routines time() and ctime() to obtain and format the time of day
 Suggestion: start by writing some string of your own choosing to the client

 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#define MY_PORT_NUMBER 49999

int main() {
	int listenfd;
	int len;
	struct sockaddr_in servAddr;
	int connectfd;
	int length = sizeof(struct sockaddr_in);
	struct sockaddr_in clientAddr;
	struct hostent* hostEntry;
	const char* hostName;
	time_t _time;
	char* currentTime;

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Server socket failure.");
		return 1;
	}
	printf("Created socket.\n");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(MY_PORT_NUMBER);
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(listenfd, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0) {
		perror("Binding error.");
		close(listenfd);
		return 1;
	}
	printf("Bound port.\n");

	while (1) {
		listen(listenfd, 1);
		printf("Listening...\n");
		connectfd = accept(listenfd, (struct sockaddr*) &clientAddr, &length);

		pid_t childpid = fork();
		if(childpid <0 ){
			perror("fork failed");
			return -1;
		}

		if (childpid > 0) { //child
			printf("parent\n");
			close(connectfd);

		}
		else {
			printf("child\n");
			if ((hostEntry = gethostbyaddr(&(clientAddr.sin_addr),
					sizeof(struct in_addr), AF_INET)) == NULL) {
				herror("server failed to get host name.");
				close(connectfd);
				close(listenfd);
				return 1;
			}
			hostName = hostEntry->h_name;
			printf("Connected to %s\n", hostName);

			//			close(0);
			time(&_time);
			currentTime = ctime(&_time);
			printf("Writing...\n");

			if ((len = write(connectfd, hostName, strlen(hostName))) < 0) {
				perror("Write failed.");
				close(listenfd);
				close(connectfd);
				return 1;
			}
			if ((len = write(connectfd, ": ", 2)) < 0) {
				perror("Write failed.");
				close(listenfd);
				close(connectfd);
				return 1;
			}
			if ((len = write(connectfd, currentTime, strlen(currentTime)))
					< 0) {
				perror("Write failed.");
				close(listenfd);
				close(connectfd);
				return 1;
			}
			printf("Write successful! Closing connection...\n");
			close(connectfd);
			return 0;
		}
	}
}
