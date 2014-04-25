/*
 ============================================================================
 Name        : connect.c
 Author      : Chris Stickney
 SID		 : 011385713
 Description : connect program
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
//#include <fcntl.h>

int main(int argc, char *argv[]) {
	int fd[2];
	int pipeStatus = pipe(fd);
	if(pipeStatus){
		fprintf(stderr, "error 1:%s\n",strerror(errno));
		return 1;
	}
	int rdr = fd[0], wtr=fd[1];
	int colon = 0, i;
	while(strcmp(argv[++colon], ":") && argv[colon] != NULL);
	if(argv[colon] == NULL){
		fprintf(stderr, "Missing colon.\n");
		return 1;
	}
	else if(colon==1){
		fprintf(stderr, "Missing first argument.\n");
		return 1;
	}
	else if(argv[colon+1] == NULL){
		fprintf(stderr, "Missing second argument\n");
		return 1;
	}
	argv[colon] = NULL;

	if(fork()){//parent
		close(wtr);
		close(0); dup(rdr); close(rdr);//redirect stdin to pipe
		execvp(argv[colon+1], &argv[colon+1]);
		fprintf(stderr, "error 2: %s\n",strerror(errno));
		wait(1);
		return 1;
	}
	else{//child
		close(rdr);
		if(argv[1] == NULL){
			fprintf(stderr, "Missing first argument.\n");
			return 1;
		}
		close(1); dup(wtr); close(wtr);//redirect stdout to pipe
		execvp(argv[1], &argv[1]);
		fprintf(stderr, "error 3: %s\n",strerror(errno));
		return 1;
	}
}
