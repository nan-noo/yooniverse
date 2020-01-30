#include <stdlib.h> // malloc
#include <stdio.h>
#include <ctype.h> // toupper

#define QUIT	1
#define INSERT	2
#define DELETE	3
#define PRINT	4
#define SEARCH	5

typedef struct node
{
	int			data;
	struct node	*link;
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
	LIST *list;
	list = malloc(sizeof(LIST));
	if (list != NULL) {
		list->head = malloc(sizeof(NODE));
		list->head->link = NULL;
		list->count = 0;
	}
	else return NULL;

	return list;
}

/* Deletes all data in list and recycles memory
return	NULL head pointer
*/
LIST *destroyList(LIST *pList) {
	NODE *pDel;
	while (pList->head->link != NULL) {
		pDel = pList->head->link;
		pList->head->link = pDel->link;
		free(pDel);
	}
	
	pList->count = 0;
	free(pList->head);
	free(pList);

	return NULL;
}

/* Inserts data into list
return	-1 if overflow
0 if successful
1 if dupe key
*/
int addNode(LIST *pList, int dataIn) {
	NODE *pNew;
	NODE *pPre;
	pNew = malloc(sizeof(NODE));
	
	if (pNew != NULL) {
		pNew->data = dataIn;
		pPre = pList->head;

		while (pPre->link != NULL && pPre->link->data <= dataIn) {
			pPre = pPre->link;
			if (pPre->data == dataIn) {
				free(pNew);
				return 1;
			}
		}
		if (pPre == NULL) {
			pNew->link = pList->head->link;
			pList->head->link = pNew;
		}
		else {
			pNew->link = pPre->link;
			pPre->link = pNew;
		}
		(pList->count)++;
		
		return 0;
	}
	else return -1;
}

/* Removes data from list
return	0 not found
1 deleted
*/
int removeNode(LIST *pList, int Key, int *dataOut) { 
	NODE *pPre;
	NODE *pLoc;
	pLoc = pList->head->link;
	pPre = NULL;

	if (pLoc == NULL) return 0;
	else {
		while (pLoc->data != Key) {
			pPre = pLoc;
			pLoc = pLoc->link;
			if (pLoc == NULL) return 0;
		}
		*dataOut = pLoc->data;
		
		if (pPre == NULL) {
			pList->head->link = pLoc->link;
			free(pLoc);
			return 1;
		}
		else {
			pPre->link = pLoc->link;
			free(pLoc);
			return 1;
		}
	}
	return 0;
}

/* interface to search function
Argu	key being sought
dataOut	contains found data
return	1 successful
0 not found
*/
int searchList(LIST *pList, int Argu, int *dataOut) {
	NODE *pPre;
	NODE *pLoc;
	pLoc = pList->head->link;
	pPre = NULL;

	if (pLoc == NULL) return 0;
	else {
		while (pLoc->data != Argu) {
			pPre = pLoc;
			pLoc = pLoc->link;
			if (pLoc == NULL) return 0;
		}
		*dataOut = pLoc->data;
		return 1;
	}
}

/* returns number of nodes in list
*/
int listCount(LIST *pList);

/* returns	1 empty
0 list has data
*/
int emptyList(LIST *pList);

//int fullList( LIST *pList);

void printList(LIST *pList) {
	NODE *pNow;
	pNow = pList->head->link;
	if (pNow == NULL) {
		printf("NULL\n");
	}
	else {
		while (pNow != NULL) {
			printf("%d->", pNow->data);
			pNow = pNow->link;
		}
		printf("NULL\n");
		(pList->count)--;
	}
}

int get_action()
{
	char ch;

	scanf("%c", &ch);
	ch = toupper(ch);

	switch (ch)
	{
	case 'Q':
		return QUIT;
	case 'P':
		return PRINT;
	case 'I':
		return INSERT;
	case 'D':
		return DELETE;
	case 'S':
		return SEARCH;
	}
	return 0; // undefined action
}

////////////////////////////////////////////////////////////////////////////////
int main(void)
{
	int num;
	LIST *list;
	int data;

	list = createList();
	if (!list)
	{
		printf( "Cannot create list\n");
		return 100;
	}

	fprintf(stdout, "Select Q)uit, P)rint, I)nsert, D)elete, or S)earch: ");

	while (1)
	{
		int action = get_action();

		switch (action)
		{
		case QUIT:
			destroyList( list);
			return 0;

		case PRINT:
			printList( list);
			break;

		case INSERT:
			fprintf(stdout, "Enter a number to insert: ");
			fscanf(stdin, "%d", &num);

			addNode( list, num);
			printList( list);
			break;

		case DELETE:
			fprintf(stdout, "Enter a number to delete: ");
			fscanf(stdin, "%d", &num);

			removeNode( list, num, &data);
			printList( list);
			break;

		case SEARCH:
			fprintf(stdout, "Enter a number to retrieve: ");
			fscanf(stdin, "%d", &num);

			int found;
			found = searchList( list, num, &data);
			if (found) fprintf( stdout, "Found: %d\n", data);
			else fprintf( stdout, "Not found: %d\n", num);

			break;
		}
		if (action) fprintf(stdout, "Select Q)uit, P)rint, I)nsert, D)elete, or S)earch: ");

	}
	return 0;
}