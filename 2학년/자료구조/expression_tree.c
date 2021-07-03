#include <stdlib.h> // malloc, atoi
#include <stdio.h>
#include <ctype.h> // isdigit
#include <assert.h> // assert

#define MAX_STACK_SIZE	50

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	char		data;
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
TREE *createTree(void) {
	TREE* tree;
	tree = (TREE*)malloc(sizeof(TREE));
	if (tree == NULL) return NULL;
	tree->root = NULL;
	return tree;
};

////////////////////////////////////////////////////////////////////////////////
/* Deletes all data in tree and recycles memory
return	NULL head pointer
*/
static void _destroy(NODE *root) {
	if (root == NULL) return;

	_destroy(root->left);
	_destroy(root->right);

	free(root);
};

TREE *destroyTree(TREE *pTree)
{
	if (pTree)
	{
		_destroy(pTree->root);
	}

	free(pTree);

	return NULL;
}

/*  Allocates dynamic memory for a node and returns its address to caller
return	node pointer
NULL if overflow
*/
static NODE *_makeNode(char ch) {
	NODE* nodePtr;
	nodePtr = (NODE*)malloc(sizeof(NODE));
	if (nodePtr == NULL) return NULL;

	nodePtr->data = ch;
	nodePtr->left = NULL;
	nodePtr->right = NULL;

	return nodePtr;
};

/* converts postfix expression to binary tree
return	1 success
0 invalid postfix expression
*/
int postfix2tree(char *expr, TREE *pTree) {
	NODE* arr[MAX_STACK_SIZE] = { NULL };
	NODE* nPtr;
	int j = 0;
	
	for (int i = 0;expr[i] != NULL;i++) {
		nPtr = _makeNode(expr[i]);
		if (isdigit(expr[i])) {
			arr[j] = nPtr;
			j++;
		}
		else {
			j--;
			if (j < 0) {
				_destroy(nPtr);
				free(pTree);
				return 0;
			}
			nPtr->right = arr[j];
			arr[j] = NULL;
			j--;
			if (j < 0) {
				destroyTree(pTree);
				return 0;
			}
			nPtr->left = arr[j];

			arr[j] = nPtr;
			j++;
		}
	}
	
	if (arr[1] != NULL) {
		for (j = 0;arr[j] != NULL;j++) {
			_destroy(arr[j]);
		}
		free(pTree);
		return 0;
	}
	else {
		pTree->root = arr[0];
		return 1;
	}
};

/* internal traversal function
an implementation of ALGORITHM 6-6
*/
static void _traverse(NODE *root) {
	if (root == NULL) return;

	if (isdigit(root->data)) {
		printf("%c", root->data);
	}
	else {
		printf("(");
		_traverse(root->left);
		printf("%c", root->data);
		_traverse(root->right);
		printf(")");
	}
};

////////////////////////////////////////////////////////////////////////////////
/* Print node in tree using inorder traversal
*/
void traverseTree(TREE *pTree)
{
	_traverse(pTree->root);

	return;
}

/* internal traversal function
*/
static void _infix_print(NODE *root, int level) {
	if (root == NULL) return;

	_infix_print(root->right, ++level);
	for (int i = 0;i < level - 1;i++) printf("\t");
	printf("%c\n", root->data);
	_infix_print(root->left, level);

};

/* evaluate postfix expression
return	value of expression
*/
float evalPostfix(char *expr) {
	float arr[1024];
	int j = 0;
	for (int i = 0;expr[i] != NULL;i++) {
		if (isdigit(expr[i])) {
			arr[j] = expr[i] - '0';
			j++;
		}
		else {
			j--;
			float op2 = arr[j];
			j--;
			float op1 = arr[j];

			switch (expr[i]) {
			case '+':
				arr[j] = op1 + op2;
				break;
			case '*':
				arr[j] = op1 * op2;
				break;
			case '/':
				arr[j] = op1 / op2;
				break;
			case '-':
				arr[j] = op1 - op2;
				break;
			}
			j++;
		}
	}
	return arr[0];
};

////////////////////////////////////////////////////////////////////////////////
/* Print tree using inorder right-to-left traversal
*/
void printTree(TREE *pTree)
{
	_infix_print(pTree->root, 0);

	return;
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	TREE *tree;
	char expr[1024];

	fprintf(stdout, "\nInput an expression (postfix): ");

	while (fscanf(stdin, "%s", expr) == 1)
	{
		// creates a null tree
		tree = createTree();

		if (!tree)
		{
			printf("Cannot create tree\n");
			return 100;
		}

		// postfix expression -> expression tree
		int ret = postfix2tree(expr, tree);
		if (!ret)
		{
			fprintf(stdout, "invalid expression!\n");
			continue;
		}

		// expression tree -> infix expression
		fprintf(stdout, "\nInfix expression : ");
		traverseTree(tree);

		// print tree with right-to-left infix traversal
		fprintf(stdout, "\n\nTree representation:\n");
		printTree(tree);

		// evaluate postfix expression
		float val = evalPostfix(expr);
		fprintf(stdout, "\nValue = %f\n", val);

		// destroy tree
		destroyTree(tree);

		fprintf(stdout, "\nInput an expression (postfix): ");
	}
	return 0;
}