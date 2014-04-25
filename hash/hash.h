/*
 ============================================================================
 Name        : hash.h
 Author      : Chris Stickney
 Description : My hashtable header for HW1
 ============================================================================
 */
#ifndef HASH_H
#define HASH_H
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

Hashtable* rehash(Hashtable *pairTable);
void destroyNode(Node* sortedList);
void destroy(Hashtable *pairTable);
Hashtable* addFile(Hashtable* pairTable, FILE* fd);
Hashtable* createTable(FILE* fd);
void insert(Hashtable *pairTable, char *wordPair);
unsigned int getHashValue(const char *str, int tableSize);
char* getNextWord(FILE* fd);
char* strdup(const char *str);
#endif /* LUDECOMP_H */
