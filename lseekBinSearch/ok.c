/*
 ============================================================================
 Name        : ok.c
 Author      : Chris Stickney
 Description :
 ============================================================================
 */

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char* argv[]) {
	int fd;
	if(argv[1] == NULL){
		//error state: no word
		write(2,"Error: no word.\n",16);
		return 1;
	}
	if(strlen(argv[1]) > 15){
		write(2,"Error: word is >15 letters.\n",28);
		//error state: word is too long
		return 1;
	}
	fd =open("/cs_Share/class/cs360/lib/webster", O_RDONLY, 0); //open the dictionary
	if(fd<0){
		write(2,"Error: File not found.\n",23);
		return 1;
	}

	char* buffer= malloc(sizeof(char)*(strlen(argv[1])));
	int attempt=strlen(argv[1]);//always attempt to read the length of the word

	long i = 0;
	i=lseek(fd, i, SEEK_END);//find EOF position
	long dictLength = i;//EOF position
	long first=0, last=dictLength/16, middle= last/2;//first: least word in search range, last: greatest word, middle: middle word
	i=middle*16;// move i 16 chars by middle # of words
	lseek (fd, i, SEEK_SET); //move to i

	while(1){//loop until word is found or not found
			int actual=read(fd, buffer, attempt);//read begins from the current offset
			if(actual< attempt){//cant read all chars
				//error state: cant read that many chars
				write(2,"Error: buffer underflow.\n",25);
				free(buffer);//free yo variables
				return 1;
			}
			if(first == last){//couldnt find the word
				write(1,"no\n",3);
				close(fd);//close yo files
				free(buffer);//free yo variables
				return 0;
			}
			if(strcmp(buffer, argv[1])<0){//earlier than the word
				first = middle;
			}
			else if(strcmp(buffer, argv[1])>0){//later than the word
				last = middle;

			}
			else{//found the word
				write(1,"yes\n",4);
				close(fd);//close yo files
				free(buffer);//free yo variables
				return 0;
			}
			if(middle == (first+last)/2 && middle == first && middle != last){//kick middle over to the second word in a 2 word search range
				middle=(first+last)/2 +1;
				first++;
			}
			else//set middle to the approximate middle
				middle = (first + last)/2;

			i=middle*16;// move i 16 chars by middle # of words
			lseek (fd, i, SEEK_SET); //move to i
	}
}
