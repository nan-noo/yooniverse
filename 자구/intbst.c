#include <stdlib.h> // malloc, atoi, rand
#include <stdio.h>
#include <ctype.h> // isdigit
#include <assert.h> // assert
#include <time.h> // time

////////////////////////////////////////////////////////////////////////////////
// TREE type definition
typedef struct node
{
	int			data;
	struct node	*left;
	struct node	*right;
} NODE;

typedef struct
{
	NODE	*root;
} TREE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a tree head node and returns its address to caller
return	head node pointer
NULL if overflow
*/
TREE *BST_Create(void) {
	TREE* tree;
	tree = (TREE*)malloc(sizeof(TREE));
	if (tree == NULL) return NULL;
	tree->root = NULL;
	return tree;
};

/* Deletes all data in tree and recycles memory
return	NULL head pointer
*/
static void _destroy(NODE *root) {
	if (root == NULL) return;

	_destroy(root->left);
	_destroy(root->right);

	free(root);
};
TREE *BST_Destroy(TREE *pTree) {
	if (pTree)
	{
		_destroy(pTree->root);
	}

	free(pTree);

	return NULL;
};

static NODE *_makeNode(int data) {
	NODE* nodePtr;
	nodePtr = (NODE*)malloc(sizeof(NODE));
	if (nodePtr == NULL) return NULL;

	nodePtr->data = data;
	nodePtr->left = NULL;
	nodePtr->right = NULL;

	return nodePtr;
};

/* Inserts new data into the tree
return	1 success
0 overflow
*/
int BST_Insert(TREE *pTree, int data) {
	NODE* nNew = _makeNode(data);
	NODE* nCur = pTree->root;
	NODE* nPar = NULL;

	while (nCur != NULL) {
		nPar = nCur;

		if (nCur->data > data)	nCur = nCur->left;
		else nCur = nCur->right;
	}

	if (nPar != NULL) {
		if (data < nPar->data) {
			nPar->left = nNew;
			return 1;
		}
		else {
			nPar->right = nNew;
			return 1;
		}
	}
	else {
		pTree->root = nNew;
		return 1;
	}
	return 0;	
};

/* internal function
success is 1 if deleted; 0 if not
return	pointer to root
*/
NODE *_delete(NODE *root, int dltKey, int *success) {
	NODE* nPar = NULL;
	NODE* nDel = root;
	NODE* nSmall = NULL;
	NODE* nsPar = NULL;
	int data;

	while (nDel != NULL && nDel->data != dltKey) {
		nPar = nDel;

		if (dltKey < nDel->data) {
			nDel = nDel->left;
		}
		else {
			nDel = nDel->right;
		}
	}
	if (nDel == NULL) {
		*success = 0;
		return NULL;
	}
	if (nDel->left == NULL && nDel->right == NULL && nDel == root) {
		*success = 1;
	}
	else if (nDel->left == NULL) {
		if (nDel == root) {
			data = nDel->data;
			nDel = nDel->right;
			root->data = nDel->data;
			root->right = nDel->right;
			root->left = nDel->left;
			nDel->data = data;
		}
		else if (nPar->left == nDel) {
			nPar->left = nDel->right;
		}
		else {
			nPar->right = nDel->right;
		}
		*success = 1;
	}
	else if (nDel->right == NULL) {
		if (nDel == root) {
			data = nDel->data;
			nDel = nDel->left;
			root->data = nDel->data;
			root->right = nDel->right;
			root->left = nDel->left;
			nDel->data = data;
		}
		else if (nPar->left == nDel) {
			nPar->left = nDel->left;
		}
		else {
			nPar->right = nDel->left;
		}
		*success = 1;
	}
	else {
		nsPar = nDel;
		nSmall = nDel->right;
		while (nSmall->left != NULL) {
			nsPar = nSmall;
			nSmall = nSmall->left;
		}
		data = nDel->data;
		nDel->data = nSmall->data;

		if (nsPar->left == nSmall) {
			nsPar->left = nSmall->right;
		}
		else {
			nsPar->right = nSmall->right;
		}
		nDel = nSmall;
		nDel->data = data;
		*success = 1;
	}	

	return nDel;
};

/* Deletes a node with dltKey from the tree
return	1 success
0 not found
*/
int BST_Delete(TREE *pTree, int dltKey) {
	int success;
	NODE* nDel;
	nDel = _delete(pTree->root, dltKey, &success);

	if (nDel == pTree->root) {
		free(pTree->root); 
		pTree->root = NULL;
	}
	else free(nDel);

	if (success == 1) return 1;
	else return 0;
};

static void _traverse(NODE *root) {
	if (root == NULL) return;
		
	_traverse(root->left);
	printf("%d ", root->data);
	_traverse(root->right);
};

/* prints tree using inorder traversal
*/
void BST_Traverse(TREE *pTree) {
	_traverse(pTree->root);

	return;
};

/* internal traversal function
*/
static void _infix_print(NODE *root, int level) {
	if (root == NULL) return;

	_infix_print(root->right, ++level);
	for (int i = 0;i < level - 1;i++) printf("\t");
	printf("%d\n", root->data);
	_infix_print(root->left, level);
};
/* Print tree using inorder right-to-left traversal
*/
void printTree(TREE *pTree) {
	_infix_print(pTree->root, 0);

	return;
};

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	TREE *tree;
	int data;

	// creates a null tree
	tree = BST_Create();

	if (!tree)
	{
		printf( "Cannot create tree\n");
		return 100;
	}

	fprintf(stdout, "Inserting: ");

	srand(time(NULL));
	for (int i = 1; i < 20; i++)
	{
		data = rand() % 100 + 1; // 1 ~ 100 random number

		fprintf(stdout, "%d ", data);

		// insert funtion call
		BST_Insert( tree, data);
	}
	fprintf(stdout, "\n");

	// inorder traversal
	fprintf(stdout, "Inorder traversal: ");
	BST_Traverse( tree);
	fprintf(stdout, "\n");

	// print tree with right-to-left infix traversal
	fprintf(stdout, "Tree representation:\n");
	printTree(tree);

	int ret;
	do
	{
		fprintf(stdout, "Input a number to delete: ");
		int num;
		ret = scanf("%d", &num);
		if (ret != 1) break;

		ret = BST_Delete( tree, num);
		if (!ret) fprintf( stdout, "%d not found\n", num);

		// print tree with right-to-left infix traversal
		fprintf(stdout, "Tree representation:\n");
		printTree(tree);
	} while (1);

	BST_Destroy( tree);

	return 0;
}
