/*
 ============================================================================
 Name        : daytime.c
 Author      : Chris Stickney
 Description : daytime is a client program that takes one argument on the command line.  That argument is the server’s host name or IP address (in dotted decimal).  It makes a TCP connection to the indicated host and dayserve’s port number (or indicates the error, if it cannot) and then writes the information received from dayserve to stdout.
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

#define MY_PORT_NUMBER 49999
#define BUFFER 512

int main(int argc, char* argv[]) {
	int socketfd, len;
	char* buf = malloc(sizeof(char) * BUFFER);

	if (argc != 2) {
		printf("Invalid host name. Exiting.\n");
		return 1;
	}
	if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {//try create socket
		perror("Client failed to create socket.");
		return 1;
	}
	printf("Created socket!\n");

	struct sockaddr_in servaddr;
	struct hostent* hostEntry;
	struct in_addr **pptr;

	memset( &servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(MY_PORT_NUMBER);

	/* test for error using herror() */
	if ((hostEntry = gethostbyname(argv[1])) == NULL) {
		herror("Client failed to get host name.");
		close(socketfd);
		return 1;
	}
	printf("Client got host name.\n");

	/* this is magic, unless you want to dig into the man pages */
	pptr = (struct in_addr **) hostEntry->h_addr_list;
	memcpy(&servaddr.sin_addr, *pptr, sizeof(struct in_addr));

	if (connect(socketfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
		perror("Client failed to connect.");
		close(socketfd);
		return 1;
	}
	printf("Connection established!\n");

	printf("Reading...\n");
	while ((len = read(socketfd, buf, BUFFER)) > 0) {//read from server, 512 at a time
		write(1, buf, len);
	}
	printf("\nFinished reading.\n");
	return 0;
}
