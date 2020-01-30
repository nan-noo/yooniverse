#include <stdlib.h> // malloc
#include <stdio.h>
#include <string.h> // strdup, strcmp
#include <ctype.h> // toupper

#define QUIT			1
#define FORWARD_PRINT	2
#define BACKWARD_PRINT	3
#define DELETE			4

// User structure type definition
typedef struct
{
	char	*token;
	int		freq;
} tTOKEN;

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	tTOKEN		*dataPtr;
	struct node	*llink;
	struct node	*rlink;
} NODE;

typedef struct
{
	int		count;
	NODE	*pos;
	NODE	*head;
	NODE	*rear;
} LIST;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a list head node and returns its address to caller
return	head node pointer
NULL if overflow
*/
LIST *createList(void) {
	LIST* list = (LIST*)malloc(sizeof(LIST));
	list->head = (NODE*)malloc(sizeof(NODE));
	list->rear = (NODE*)malloc(sizeof(NODE));
	if (list == NULL) return NULL;
	if (list->head == NULL) return NULL;
	if (list->rear == NULL) return NULL;

	list->head->llink = NULL;
	list->head->rlink = NULL;
	list->rear->llink = list->head;
	list->rear->rlink = NULL;
	list->count = 0;

	return list;
};

/* Deletes all data in list and recycles memory
return	NULL head pointer
*/
LIST *destroyList(LIST *pList) {
	NODE *pDel;

	while (pList->head->rlink) {
		pDel = pList->head->rlink;
		pList->head->rlink = pDel->rlink;

		free(pDel->dataPtr->token);
		free(pDel->dataPtr);
		free(pDel);
	}

	pList->count = 0;
	free(pList->head);
	free(pList->rear);
	free(pList);

	return NULL;
};

/* Inserts data into list
return	-1 if overflow
0 if successful
1 if duplicated key
*/
int addNode(LIST *pList, tTOKEN *dataInPtr) {
	NODE *pPre = NULL;
	NODE *pNow;
	NODE *pNew = (NODE*)malloc(sizeof(NODE));
	if (pNew == NULL) return -1;

	pNew->dataPtr = dataInPtr;
	pNow = pList->head->rlink;

	while (pNow && strcmp(pNow->dataPtr->token, dataInPtr->token) <= 0) {
		pPre = pNow;
		pNow = pNow->rlink;
		if (strcmp(pPre->dataPtr->token, dataInPtr->token) == 0) {
			free(pNew);
			(pPre->dataPtr->freq)++;
			return 1;
		}
	}
	if (pPre == NULL) {
		if(pList->head->rlink) pList->head->rlink->llink = pNew;
		
		pNew->rlink = pList->head->rlink;
		pList->head->rlink = pNew;
	}
	else {
		if (pPre->rlink) pPre->rlink->llink = pNew;
		pNew->rlink = pPre->rlink;
		pPre->rlink = pNew;
		pNew->llink = pPre;
	}
	while (pList->rear->llink->rlink) {
		pList->rear->llink = pList->rear->llink->rlink;
	}

	return 0;
};

/* Removes data from list
return	0 not found
1 deleted
*/
int removeNode(LIST *pList, char *keyPtr, tTOKEN **dataOut) {
	NODE *pPre;
	NODE *pLoc;
	pLoc = pList->head->rlink;
	pPre = NULL;

	if (pLoc == NULL) return 0;
	else {
		while (strcmp(pLoc->dataPtr->token,keyPtr) != 0) {
			pPre = pLoc;
			pLoc = pLoc->rlink;
			if (pLoc == NULL) return 0;
		}
		*dataOut = pLoc->dataPtr;

		if (pPre == NULL) {
			pList->head->rlink = pLoc->rlink;
			if(pLoc->rlink) pLoc->rlink->llink = NULL;
			free(pLoc);
			return 1;
		}
		else {
			pPre->rlink = pLoc->rlink;
			if(pLoc->rlink) pLoc->rlink->llink = pPre;
			free(pLoc);
			
			pList->rear->llink = pList->head;
			while (pList->rear->llink->rlink) {
				pList->rear->llink = pList->rear->llink->rlink;
			}
			return 1;
		}
	}
	return 0;
};

//int fullList( LIST *pList);

/* prints data from list (forward)
*/
void printList(LIST *pList) {
	NODE* pNow = pList->head->rlink;

	if (pNow == NULL) printf("NULL\n");

	while (pNow != NULL) {
		printf("%s\t%d\n", pNow->dataPtr->token, pNow->dataPtr->freq);
		pNow = pNow->rlink;
	}
};

/* prints data from list (backward)
*/
void printListR(LIST *pList) {
	NODE* pNow = pList->rear->llink;

	if (pNow == NULL) printf("NULL\n");

	while (pNow != NULL) {
		printf("%s\t%d\n", pNow->dataPtr->token, pNow->dataPtr->freq);
		pNow = pNow->llink;
	}
};

/* Allocates dynamic memory for a token structure, initialize fields(token, freq) and returns its address to caller
return	token structure pointer
NULL if overflow
*/
tTOKEN *createToken(char *str) {
	tTOKEN* tNew = (tTOKEN*)malloc(sizeof(tTOKEN));
	if (tNew == NULL) return NULL;

	tNew->token = strdup(str);
	tNew->freq = 1;

	return tNew;
};

/* Deletes all data in token structure and recycles memory
return	NULL head pointer
*/
tTOKEN *destroyToken(tTOKEN *pToken) {
	free(pToken->token);
	free(pToken);

	return NULL;
};

/* gets user's input
*/
int get_action()
{
	char ch;
	scanf("%c", &ch);
	ch = toupper(ch);
	switch (ch)
	{
	case 'Q':
		return QUIT;
	case 'F':
		return FORWARD_PRINT;
	case 'B':
		return BACKWARD_PRINT;
	case 'D':
		return DELETE;
	}
	return 0; // undefined action
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	LIST *list;
	char str[1024];
	tTOKEN *pToken;
	int ret;
	FILE *fp;

	if (argc != 2)
	{
		fprintf(stderr, "usage: %s FILE\n", argv[0]);
		return 1;
	}

	fp = fopen(argv[1], "rt");
	if (!fp)
	{
		fprintf(stderr, "Error: cannot open file [%s]\n", argv[1]);
		return 2;
	}

	// creates a null list
	list = createList();
	if (!list){
		printf( "Cannot create list\n");
		return 100;
	}

	while (fscanf(fp, "%s", str) == 1)
	{
		pToken = createToken( str);

		// insert function call
		ret = addNode( list, pToken);

		if (ret == 1) // duplicated 
		destroyToken( pToken);
	}

	fclose(fp);

	fprintf(stdout, "Select Q)uit, F)orward print, B)ackward print, D)elete: ");

	while (1)
	{
		int action = get_action();

		switch (action)
		{
		case QUIT:
			destroyList( list);
			return 0;

		case FORWARD_PRINT:
			printList( list);
			break;

		case BACKWARD_PRINT:
			printListR( list);
			break;

		case DELETE:
			fprintf(stdout, "Input a string to delete: ");
			fscanf(stdin, "%s", str);
			int ret = removeNode( list, str, &pToken);
			if (ret){
				fprintf( stdout, "%s deleted\n", pToken->token);
				destroyToken( pToken);
			}
			else fprintf( stdout, "%s not found\n", str);
			break;
		}

		if (action) fprintf(stdout, "Select Q)uit, F)orward print, B)ackward print, D)elete: ");
	}
	return 0;
}