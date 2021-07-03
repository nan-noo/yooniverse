#include <stdlib.h> // malloc
#include <stdio.h>
#include <string.h> // strdup, strcmp

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
	LIST* list = (LIST*)malloc(sizeof(LIST));
	list->head = (NODE*)malloc(sizeof(NODE));
	if(list == NULL) return NULL;	
	if(list->head == NULL) return NULL;
	
	list->head->link = NULL;
	list->count = 0;
	
	return list;
}

/* Deletes all data in list and recycles memory
return	NULL head pointer
*/
LIST *destroyList(LIST *pList) {
	NODE *pDel;

	while (pList->head->link) {
		pDel = pList->head->link;
		pList->head->link = pDel->link;

		free(pDel->dataPtr->token);
		free(pDel->dataPtr);
		free(pDel);
	}

	pList->count = 0;
	free(pList->head);
	free(pList);

	return NULL;
};

/* Inserts data into list
return	-1 if overflow
0 if successful
1 if duplicated key
*/
int addNode(LIST *pList, tTOKEN *dataInPtr){
	NODE *pPre = NULL;
	NODE *pNow;
	NODE *pNew = (NODE*)malloc(sizeof(NODE));
	if(pNew == NULL) return -1;
	
	pNew->dataPtr = dataInPtr;
	pNow = pList->head->link;
	
	while(pNow && strcmp(pNow->dataPtr->token,dataInPtr->token)<=0){
		pPre = pNow;
		pNow = pNow->link;
		if(strcmp(pPre->dataPtr->token,dataInPtr->token) == 0){
			free(pNew);
			(pPre->dataPtr->freq)++;
			return 1;
		}
		
	}
	if(pPre == NULL){
		pNew->link = pList->head->link;
		pList->head->link = pNew;
	}
	else{
		
		pNew->link = pPre->link;
		pPre->link = pNew;
	}
	return 0;
};

/* prints data from list
*/
void printList(LIST *pList) {
	NODE* pNow = pList->head->link;
	
	if(pNow == NULL) printf("NULL\n");
	
	while(pNow != NULL){
		printf("%s\t%d\n",pNow->dataPtr->token,pNow->dataPtr->freq);
		pNow = pNow->link;
	}
};

/* Allocates dynamic memory for a token structure, initialize fields(token, freq) and returns its address to caller
return	token structure pointer
NULL if overflow
*/
tTOKEN *createToken(char *str) {
	tTOKEN* tNew = (tTOKEN*)malloc(sizeof(tTOKEN));
	if(tNew == NULL) return NULL;
	
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

////////////////////////////////////////////////////////////////////////////////
int main(void)
{
	LIST *list;
	char str[1024];
	tTOKEN *pToken;
	int ret;

	// creates a null list
	list = createList();
	if (!list)
	{
		printf("Cannot create list\n");
		return 100;
	}

	while (scanf("%s", str) == 1)
	{
		pToken = createToken( str);

		// insert function call
		ret = addNode( list, pToken);
		
		if (ret == 1) {// duplicated 
			destroyToken( pToken);
		}
	}
	// print function call
	printList( list);

	destroyList( list);

	return 0;
}