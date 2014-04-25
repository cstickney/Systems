/*
 ============================================================================
 Name        : warn.c
 Author      : Chris Stickney
 SID		 : 011385713
 Description : warn program
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <errno.h>
#define BUFFER 2000

void reader();
void writer();
static void handler(int code);

int delay = 5;
int loop = 1;
char* msg;
pid_t pid;
int rdr, wtr;

int main(int argc, char *argv[]){
	int fd[2];
	int status;
	int pipeStatus = pipe(fd);
	msg = "Default message!";

	if(pipeStatus){
	fprintf(stderr, "error 1:%s\n",strerror(errno));
		return 1;
	}
	rdr=fd[0];
	wtr=fd[1];
	pid = fork();
	if(pid){//parent
		signal(SIGFPE, SIG_IGN);//ignore read signals
		close(rdr);
		signal(SIGINT, writer);//process write signals
		wait(&status);//wait until the child has exited
		return 0;
	}
	else{//child
		signal(SIGINT, SIG_IGN);//The child process must ignore SIGINT.
		close(wtr);
		signal(SIGFPE, reader);//When the child process detects a SIGFPE, it will read a single line message from the shared pipe.

		while(loop){
			fprintf(stdout, "%s\n", msg);
			signal (SIGALRM, handler);//set the default for sigalarm
			alarm (delay);//start timer for delay seconds
			pause ();//wait for a signal
			alarm (0);//turn off timer
		}
	    return 0;
	}
}
void reader(){
	char message[BUFFER];
	char intc[2];
	intc[1] = '\0';
	int i;
	int total;
	int length = read(rdr, message, BUFFER);
	if(length == -1){//empty message
		signal(SIGFPE, reader);
		return;
	}
	message[length -1] = '\0';
	while(message[i] == ' '){//skip spaces
		i++;
	}
	if(message[i] >= '0' && message[i] <='9'){//argument is a number
		total = 0;
		while(message[i] >= '0' && message[i] <= '9'){
			intc[0] = message[i];
			total = total*10 + atoi(intc);
			i++;
		}
		delay = total;
	}
	while(message[i] == ' '){//skip spaces
		i++;
	}
	if(!strcmp(&message[i], "exit")){//exit given
		loop = 0;
		return;
	}
	if(i < length -1){//new message
		msg = &message[i];
	}
	signal(SIGFPE, reader);
	return;
}
void writer(){
	char message[BUFFER];

	signal(SIGINT, SIG_IGN);
	fprintf(stdout, "Enter message: ");
	fgets(message, BUFFER, stdin);
	if(strlen(message) == 1 ){//empty command
		signal(SIGINT, writer);
	}
	else{//command
		write(wtr, message, strlen(message));
		kill(pid, SIGFPE);
		signal(SIGINT, writer);
	}
	return;
}
static void handler(int code){
	return;
}
