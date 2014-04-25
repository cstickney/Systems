/*
 ============================================================================
 Name        : hash.c
 Author      : Chris Stickney
 Description : My main file for HW1
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#define _GNU_SOURCE
#include <string.h>
#include <wchar.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>
#include "hash.h"
#define DEBUG 0


Node* sort(Hashtable *pairTable);

static int compare(const void * a, const void * b);

int main(int argc, char *argv[]) {
	FILE *fd;
	int displayedPairs = -1, firstFileArg = 1;
	if(argc==1){
		fprintf(stderr, "Too few arguments. Exiting\n");
	    return 1;
	}
	if(argv[1][0] == '-'){
		if(argc == 2){
			fprintf(stderr, "Too few arguments. Exiting\n");
			return 1;
		}
		displayedPairs = -1 * strtol(argv[1], NULL, 10);
		firstFileArg++;
	}
	fd = fopen(argv[firstFileArg++], "r");//CHECK IF THIS EXISTS BEFORE TURNING IN
	if(fd == NULL){
		  {
		    perror ("Error");
		    return 1;
		  }
	}
	Hashtable* pairTable = createTable(fd);
	if (fclose(fd) != 0) {
		fprintf(stderr, "Error closing input file.\n");
		return 1;
	}
	while(firstFileArg<argc){
		fd = fopen(argv[firstFileArg++], "r");
		if(fd == NULL){
			  {
			    perror ("Error");
			    return 1;
			  }
		}
		pairTable = addFile(pairTable, fd);

		if (fclose(fd) != 0) {
			fprintf(stderr, "Error closing input file.\n");
			return 1;
		}
	}
	if(pairTable->elements !=0){
		Node* sortedList = sort(pairTable);

		if(displayedPairs == -1){
			if (DEBUG) printf("\n");
			printf("All pairs:\n");
			for(int j=0; j<pairTable->elements; j++){
				printf("%10d %s\n", sortedList[j].occurrences,sortedList[j].wordPair);
			}
		}

		else{
			if (DEBUG) printf("\n");
			printf("%d highest occurring pairs\n", displayedPairs);
			for(int j=0; j<displayedPairs; j++){
				printf("%10d %s\n", sortedList[j].occurrences,sortedList[j].wordPair);
			}
		}
		destroyNode(sortedList);
	}
	else{
		fprintf(stderr, "No word pairs.\n");
	}

	destroy(pairTable);
	return 0;
}
Node* sort(Hashtable *pairTable){
	if (DEBUG) printf("%d",pairTable->elements);
	Node* sortedList = malloc(pairTable->elements*sizeof(Node));
	Node* temp;
	for(int i=0, j=0; i<pairTable->size && j<pairTable->elements; i++){//load array
			if(pairTable->bucket[i].wordPair != NULL){//bucket is full

						if(pairTable->bucket[i].chainedNode != NULL){//next chain exists
							if (DEBUG) printf("Listing at %d: %s(%d)\n",j,pairTable->bucket[i].wordPair, pairTable->bucket[i].occurrences);
							sortedList[j].wordPair=strdup(pairTable->bucket[i].wordPair);
							sortedList[j].occurrences=pairTable->bucket[i].occurrences;
							j++;
							temp= pairTable->bucket[i].chainedNode;
							while(temp != NULL){//iterate through chains until end
								if (DEBUG) printf("Listing at %d: %s(%d)\n",j,temp->wordPair, temp->occurrences);
								sortedList[j].wordPair=strdup(temp->wordPair);
								sortedList[j].occurrences=temp->occurrences;
								j++;
								temp= temp->chainedNode;
							}
						}
						else{
							if (DEBUG) printf("Listing at %d: %s(%d)\n",j,pairTable->bucket[i].wordPair, pairTable->bucket[i].occurrences);
							sortedList[j].wordPair=strdup(pairTable->bucket[i].wordPair);
							sortedList[j].occurrences=pairTable->bucket[i].occurrences;
							j++;
						}
			}
	}
	qsort(sortedList, pairTable->elements, sizeof(Node), compare);//sort array
	if (DEBUG) printf("\nPost sort:\n");
	for(int j=0; j<pairTable->elements; j++){
		if (DEBUG) printf("In slot %d: %s(%d)\n",j,sortedList[j].wordPair, sortedList[j].occurrences);
	}


	return sortedList;
}
static int compare(const void * a, const void * b){
	Node *A = (Node *) a;
	Node *B = (Node *) b;
	if(B->occurrences> A->occurrences)
		return 1;
	else if(A->occurrences>B->occurrences)
		return -1;
	else
		return 0;
}

