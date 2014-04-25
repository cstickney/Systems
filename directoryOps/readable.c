/*
 ============================================================================
 Name        : readable.c
 Author      : Chris Stickney
 Description : Readable program
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

void checkDir(char* dirName);
extern int errno;

int main(int argc, char* argv[]) {
	char *dir;

	if(argc >= 2){
		dir = malloc(sizeof(char) * (strlen(argv[1]) + 2));
		strcpy(dir, argv[1]);
		if(dir[strlen(argv[1])-1] != '/'){
			strcat(dir, "/");
		}
	}
	else{
		dir = malloc(sizeof(char) * (strlen(get_current_dir_name()) + 2));
		if(dir == NULL){
			fprintf(stderr, "%s\n",strerror(errno));
			return 1;
		}
		strcpy(dir, get_current_dir_name());
		strcat(dir, "/");
	}
	char path[PATH_MAX];
	realpath(dir, path);
	strcat(path, "/");
	free(dir);
	printf("Readable files:\n");
	checkDir(path);
	return 0;
}
void checkDir(char* dirName) {
	char currentDir[2] = ".", parentDir[3] = "..", *nextDir, *name;
	int pathLength;
	struct dirent directoryEntry, *d = &directoryEntry;
	struct stat area, *s = &area;
	DIR *dir = opendir(dirName);

	if(dirName[0] == '.'){
		dirName = malloc(sizeof(char) * (strlen(get_current_dir_name()) + 2));
		strcpy(dirName, get_current_dir_name());
		strcat(dirName, "/");
		chdir(dirName);
	}
	if (dir == NULL) {
		fprintf(stderr, "Failed to open %s: %s\n", dirName, strerror(errno));
		exit(1);
	}
	chdir(dirName);
	while ((d = readdir(dir)) != NULL) {
		if (lstat(d->d_name, s) == 0) {//lstat(char* path, stat* dataStorageLoc)
			if (S_ISREG(s->st_mode) && !access(d->d_name, R_OK)) {//it's a readable regular file
				printf("%s%s\n", dirName, d->d_name);
			}
			else if (S_ISDIR(s->st_mode) && !access(d->d_name, R_OK)) {//it's a readable directory
				if (strncmp(currentDir, d->d_name, 2) != 0) {//note: access returns 0 for success
					if (strncmp(parentDir, d->d_name, 3) != 0) {
						pathLength = strlen(dirName);
						pathLength = pathLength + strlen(d->d_name);
						nextDir = (char*) malloc(sizeof(char) * (pathLength + 2));
						strcpy(nextDir, dirName);
						strcat(nextDir, d->d_name);
						strcat(nextDir, "/");
						checkDir(nextDir);
						chdir(dirName); //changes dir back to previous, current dir
						free(nextDir);
					}
				}
			}
			else {
				//it's a pipe, terminal, symbolic link, etc.
			}
		}
		else {
			fprintf(stderr, " %s: lstat(d->d_name, s) != 0\n", strerror(errno));
			//file doesn't exist or can't get to it
		}
	}
	closedir(dir);
}
