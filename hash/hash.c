/*
 ============================================================================
 Name        : hash.c
 Author      : Chris Stickney
 Description : My hashtable implementation for HW1
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>
#include "getWord.h"
#define DEBUG 0

typedef struct _node{
	char* wordPair;
	int occurrences;
	struct _node* chainedNode;
} Node;

typedef struct table{
	Node* bucket;
	int size;
	int elements;
}Hashtable;

extern char* strdup(const char *s);
extern char* getNextWord(FILE* fd);

Hashtable* rehash(Hashtable *pairTable);
void destroyNode(Node* sortedList);
void destroy(Hashtable *pairTable);
Hashtable* addFile(Hashtable* pairTable, FILE* fd);
Hashtable* createTable(FILE* fd);
void insert(Hashtable *pairTable, char *wordPair);
unsigned int getHashValue(const char *str, int tableSize);


Hashtable* rehash(Hashtable *pairTable){
	Hashtable *newTable = malloc(sizeof(Hashtable));
	Node* temp;

	newTable->size = pairTable->size * 3;
	if (DEBUG) printf("\nnewTable size: %d\n",newTable->size);
	newTable->bucket = malloc(newTable->size*sizeof(Node));
	newTable->elements = 0;
	if (DEBUG) printf("\n");
	for(int i=0; i<pairTable->size; i++){
		if(pairTable->bucket[i].wordPair != NULL){//bucket is full

					char* wordPair = strdup(pairTable->bucket[i].wordPair);
					if(pairTable->bucket[i].chainedNode != NULL){//next chain exists
						if (DEBUG) printf("Rehashing: %s\n",pairTable->bucket[i].wordPair);
						insert(newTable, pairTable->bucket[i].wordPair);
						newTable->bucket[getHashValue(pairTable->bucket[i].wordPair, newTable->size)].occurrences =pairTable->bucket[i].occurrences;
						temp= pairTable->bucket[i].chainedNode;
						while(temp != NULL){
							if (DEBUG) printf("Rehashing: %s\n",temp->wordPair);
							insert(newTable, temp->wordPair);
							newTable->bucket[getHashValue(temp->wordPair, newTable->size)].occurrences = temp->occurrences;
							temp= temp->chainedNode;
						}
					}
					else{
						if (DEBUG) printf("Rehashing: %s\n",pairTable->bucket[i].wordPair);
						insert(newTable, pairTable->bucket[i].wordPair);
						newTable->bucket[getHashValue(pairTable->bucket[i].wordPair, newTable->size)].occurrences =pairTable->bucket[i].occurrences;
					}
		}
	}
	if (DEBUG) printf("exit loop\n");
	if (DEBUG) printf("\nnewTable size: %d\n",newTable->size);
	destroy(pairTable);
	if (DEBUG) printf("\n");
	return newTable;
}
void destroyNode(Node* sortedList){
	int n=sizeof(sortedList)/sizeof(sortedList[0]);
	for(int i=0; i<n; i++){
		free(sortedList->wordPair);
	}
	free(sortedList);
}
void destroy(Hashtable* pairTable){
	Node* temp, *temp2;
	for(int i =0; i<pairTable->size; i++){//parse entire hashtable
		if(&pairTable->bucket[i] != NULL){//bucket is full
			if(pairTable->bucket[i].chainedNode != NULL){//next chain exists
				temp= pairTable->bucket[i].chainedNode;
				free(pairTable->bucket[i].wordPair);
				while(temp->chainedNode != NULL){
					temp2= temp->chainedNode;
					free(temp->wordPair);
					free(temp);
					temp=temp2;
				}
			}
			else{
				free(pairTable->bucket[i].wordPair);
			}
		}
	}
	free(pairTable->bucket);
	free(pairTable);
}
Hashtable* addFile(Hashtable* pairTable, FILE* fd){
	char *firstWord, *secondWord, *wordPair;

	firstWord = getNextWord(fd);//get first word of first wordpair
	if(firstWord == NULL){//no words
		fprintf(stderr, "File is empty\n");
		return pairTable;
	}
	else{
		while(true){
			if(pairTable->elements / pairTable->size >=.75){//resize when above maximum load factor
				if (DEBUG) printf("\nRehashing...");
				pairTable = rehash(pairTable);

			}
			secondWord = getNextWord(fd);//get second word of odd wordpairs, AUTO MALLOCS
			if(secondWord == NULL){//no more word pairs
				if (DEBUG) printf("END OF FILE\n");
				return pairTable;
			}
			wordPair = malloc(sizeof(char)*(strlen(firstWord) + strlen(secondWord) + 2));//malloc a char array which has enough room for "firstword secondword"
			strcpy(wordPair, firstWord);
			strcat(wordPair, " ");
			strcat(wordPair, secondWord);
			strcat(wordPair, "\0");//combine strings

			if (DEBUG) printf("\n%s  -->   Bucket:", wordPair);
			insert(pairTable, wordPair);//insert the wordpair into the hashtable
			free(firstWord);//free firstword, save secondword as new first word
			free(wordPair);//free the wordpair

			firstWord = getNextWord(fd);//get second word of even wordpairs, AUTO MALLOCS
			if(firstWord == NULL){//no more word pairs
				if (DEBUG) printf("END OF FILE\n");
				return pairTable;
			}
			wordPair =  malloc(sizeof(char)*(strlen(firstWord) + strlen(secondWord) + 2)); //malloc a char array which has enough room for "secondword firstword"
			strcpy(wordPair, secondWord);
			strcat(wordPair, " ");
			strcat(wordPair, firstWord);
			strcat(wordPair, "\0");//combine strings
			if (DEBUG) printf("\n%s  -->   Bucket:", wordPair);
			insert(pairTable, wordPair);//insert the wordpair into the hashtable
			free(secondWord);//free the first word(secondword), save the second word (firstword) as the new first word
			free(wordPair);//free the wordpair
		}
		return pairTable;
	}
}
Hashtable* createTable(FILE* fd){
	char *firstWord, *secondWord, *wordPair;

	Hashtable *pairTable = malloc(sizeof(Hashtable));// MALLOC Hashtable
	pairTable->size = 50;
	pairTable->bucket = malloc(pairTable->size*sizeof(Node));//MALLOC size Nodes
	pairTable->elements = 0;


	firstWord = getNextWord(fd);//get first word of first wordpair
	if(firstWord == NULL){//no words
		fprintf(stderr, "File is empty\n");
		return pairTable;
	}
	else{
		while(true){
			if(pairTable->elements / pairTable->size >=.75){//resize when above maximum load factor
				if (DEBUG) printf("\nRehashing...");
				pairTable = rehash(pairTable);

			}
			secondWord = getNextWord(fd);//get second word of odd wordpairs, AUTO MALLOCS
			if(secondWord == NULL){//no more word pairs
				if (DEBUG) printf("END OF FILE\n");
				return pairTable;
			}
			wordPair = malloc(sizeof(char)*(strlen(firstWord) + strlen(secondWord) + 2));//malloc a char array which has enough room for "firstword secondword"
			strcpy(wordPair, firstWord);
			strcat(wordPair, " ");
			strcat(wordPair, secondWord);
			strcat(wordPair, "\0");//combine strings

			if (DEBUG) printf("\n%s  -->   Bucket:", wordPair);
			insert(pairTable, wordPair);//insert the wordpair into the hashtable
			free(firstWord);//free firstword, save secondword as new first word
			free(wordPair);//free the wordpair

			firstWord = getNextWord(fd);//get second word of even wordpairs, AUTO MALLOCS
			if(firstWord == NULL){//no more word pairs
				if (DEBUG) printf("END OF FILE\n");
				return pairTable;
			}
			wordPair =  malloc(sizeof(char)*(strlen(firstWord) + strlen(secondWord) + 2)); //malloc a char array which has enough room for "secondword firstword"
			strcpy(wordPair, secondWord);
			strcat(wordPair, " ");
			strcat(wordPair, firstWord);
			strcat(wordPair, "\0");//combine strings
			if (DEBUG) printf("\n%s  -->   Bucket:", wordPair);
			insert(pairTable, wordPair);//insert the wordpair into the hashtable
			free(secondWord);//free the first word(secondword), save the second word (firstword) as the new first word
			free(wordPair);//free the wordpair
		}
		return pairTable;
	}
}
void insert(Hashtable *pairTable, char *wordPair){
	int hash = getHashValue(wordPair, pairTable->size);
	if (DEBUG) printf(" Inserting at  %u  \n", getHashValue(wordPair, pairTable->size));
	Node* current, *chainedNode;
	chainedNode = malloc(sizeof(Node));
	chainedNode->occurrences = 0;
	chainedNode->chainedNode = NULL;
	current = &pairTable->bucket[hash];
	while(1){
		if(current->occurrences == 0){//bucket is empty
			pairTable->elements++;
			current->occurrences++;
			current->wordPair = strdup(wordPair); //mallocs the bucket's word pair automatically
			return;
		}
		else if(strncmp(wordPair, current->wordPair, strlen(wordPair)) == 0){//word pair exists in bucket
			current->occurrences++;
			return;
		}
		else{//word pair DNE in this node, check chained nodes.
			if(current->chainedNode  == NULL){
				current->chainedNode = chainedNode;
				if (DEBUG) printf("chaining to linkedlist at (%x) \n", chainedNode);
			}
			current = current->chainedNode;
		}
	}

}
unsigned int getHashValue(const char *str, int tableSize){
	int length = strlen(str);
	unsigned int key = 277;// a prime number

	for(int i=0; i<length; i++){
		key = key*31+str[i];
	}
	return key%tableSize;
}

