#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // isupper, tolower

#define MAX_DEGREE	27 // 'a' ~ 'z' and EOW
#define EOW			'$' // end of word

// used in the following functions: trieInsert, trieSearch, triePrefixList
#define getIndex(x)		(((x) == EOW) ? MAX_DEGREE-1 : ((x) - 'a'))

// TRIE type definition
typedef struct trieNode {
	char 			*entry;
	struct trieNode	*subtrees[MAX_DEGREE];
} TRIE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a trie node and returns its address to caller
return	node pointer
NULL if overflow
*/
TRIE *trieCreateNode(void);

/* Deletes all data in trie and recycles memory
*/
void trieDestroy(TRIE *root);

/* Inserts new entry into the trie
return	1 success
0 failure
*/
// 주의! 엔트리를 중복 삽입하지 않도록 체크해야 함
// 대소문자를 소문자로 통일하여 삽입
// 영문자 외 문자를 포함하는 문자열은 삽입하지 않음
int trieInsert(TRIE *root, char *str);

/* Retrieve trie for the requested key
return	1 key found
0 key not found
*/
int trieSearch(TRIE *root, char *str);

/* prints all entries in trie using preorder traversal
*/
void trieList(TRIE *root);

/* prints all entries starting with str (as prefix) in trie
ex) "abb" -> "abbas", "abbasid", "abbess", ...
using trieList function
*/
void triePrefixList(TRIE *root, char *str);

/* makes permuterms for given str
ex) "abc" -> "abc$", "bc$a", "c$ab", "$abc"
return	number of permuterms
*/
int make_permuterms(char *str, char *permuterms[]);

/* recycles memory for permuterms
*/
void clear_permuterms(char *permuterms[], int size);

/* wildcard search
ex) "ab*", "*ab", "a*b", "*ab*"
using triePrefixList function
*/
void trieSearchWildcard(TRIE *root, char *str);

int main(int argc, char **argv)
{
	TRIE *trie;
	TRIE *permute_trie;
	int ret;
	char str[100];
	FILE *fp;
	char *permuterms[100];
	int num_p;

	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s FILE\n", argv[0]);
		return 1;
	}

	fp = fopen(argv[1], "rt");
	if (fp == NULL)
	{
		fprintf(stderr, "File open error: %s\n", argv[1]);
		return 1;
	}

	trie = trieCreateNode(); // original trie
	permute_trie = trieCreateNode(); // trie for permuterm index

	printf("Inserting to trie...\t");
	while (fscanf(fp, "%s", str) == 1) // words file
	{
		ret = trieInsert( trie, str);

		if (ret)
		{
			num_p = make_permuterms( str, permuterms);
			
			for (int i = 0; i < num_p; i++){
				trieInsert( permute_trie, permuterms[i]);
			}
			
			clear_permuterms( permuterms, num_p);
		}
	}

	printf("[done]\n"); // Inserting to trie

	fclose(fp);
	
	printf("\nQuery: ");
	while (fscanf(stdin, "%s", str) == 1)
	{
		if (strchr(str, '*')) // wildcard search term
		{
			trieSearchWildcard( permute_trie, str);
		}
		else // search term
		{
			ret = trieSearch( trie, str);
			printf( "[%s]%s found!\n", str, ret ? "": " not");
		}
		printf("\nQuery: ");
	}
	
	trieDestroy( trie);
	trieDestroy( permute_trie);

	return 0;
}

/* Allocates dynamic memory for a trie node and returns its address to caller
return	node pointer
NULL if overflow
*/
TRIE *trieCreateNode(void) {
	TRIE* trie = (TRIE*)malloc(sizeof(TRIE));
	if (trie == NULL) return NULL;

	trie->entry = NULL;
	
	for (int i = 0;i < MAX_DEGREE;i++) {
		trie->subtrees[i] = NULL;
	}

	return trie;
}

/* Deletes all data in trie and recycles memory
*/
void trieDestroy(TRIE *root) {
	if (root == NULL) return;
	
	for (int i = 0;i < MAX_DEGREE;i++) {
		trieDestroy(root->subtrees[i]);
	}
	
	free(root->entry);
	free(root);
	return;
}


/*
isupper-대문자면 참, tolower(대문자) 소문자로 바꿔줌

#define MAX_DEGREE	27 // 'a' ~ 'z' and EOW
#define EOW			'$' // end of word

// used in the following functions: trieInsert, trieSearch, triePrefixList
#define getIndex(x)		(((x) == EOW) ? MAX_DEGREE-1 : ((x) - 'a'))

/* Inserts new entry into the trie
return	1 success
0 failure
*/
// 주의! 엔트리를 중복 삽입하지 않도록 체크해야 함
// 대소문자를 소문자로 통일하여 삽입
// 영문자 외 문자를 포함하는 문자열은 삽입하지 않음
int trieInsert(TRIE *root, char *str) {
	TRIE* tCur = root;
	int index;

	for (int i = 0;str[i] != '\0';i++) {
		if (isupper(str[i])) str[i] = tolower(str[i]);
	}

	for (int i = 0;str[i] != '\0';i++) {
		index = getIndex(str[i]);
		if (!(index >= 0 && index < 27)) return 0;
		if (tCur->subtrees[index] == NULL) {
			tCur->subtrees[index] = trieCreateNode();
		}
		tCur = tCur->subtrees[index];
	}
	if (tCur->entry == NULL) tCur->entry = strdup(str);

	return 1;
}

/* Retrieve trie for the requested key
return	1 key found
0 key not found
*/
int trieSearch(TRIE *root, char *str) {
	TRIE* tCur = root;
	int index;

	if (root == NULL) return 0;

	for (int i = 0;str[i] != '\0';i++) {
		if (isupper(str[i])) str[i] = tolower(str[i]);
	}

	for (int i = 0;str[i] != '\0';i++) {
		index = getIndex(str[i]);
		if (!(index >= 0 && index < 27)) return 0;
		tCur = tCur->subtrees[index];

		if (tCur == NULL) return 0;
	}

	if (tCur->entry != NULL) return 1;
	else return 0;
}

/* prints all entries in trie using preorder traversal
*/
void trieList(TRIE *root) {
	if (root == NULL) return;

	if (root->entry != NULL) printf("%s\n", root->entry);

	for (int i = 0;i < MAX_DEGREE;i++) {
		trieList(root->subtrees[i]);
	}
	return;
}

/* prints all entries starting with str (as prefix) in trie
ex) "abb" -> "abbas", "abbasid", "abbess", ...
using trieList function
*/
void triePrefixList(TRIE *root, char *str) {
	TRIE* tCur = root;
	int index;

	for (int i = 0;str[i] != '\0';i++) {
		if (isupper(str[i])) str[i] = tolower(str[i]);
	}

	for (int i = 0;str[i] != '\0';i++) {
		index = getIndex(str[i]);
		if (!(index >= 0 && index < 27)) return;
		tCur = tCur->subtrees[index];

		if (tCur == NULL) return;
	}
	
	trieList(tCur);
}

/* makes permuterms for given str
ex) "abc" -> "abc$", "bc$a", "c$ab", "$abc"
return	number of permuterms
*/
int make_permuterms(char *str, char *permuterms[]) {
	str = strcat(str, "$");
	int length = strlen(str);

	for (int j = 0;j < length;j++) {
		char a1 = str[0];
		for (int i = 0;i < length - 1;i++) {
			str[i] = str[i + 1];
		}
		str[length - 1] = a1;

		permuterms[j] = strdup(str);
	}
	return length;
}

/* recycles memory for permuterms
*/
void clear_permuterms(char *permuterms[], int size) {
	for (int i = 0;i < size;i++) {
		free(permuterms[i]);
	}

	return;
}

/* wildcard search
ex) "ab*", "*ab", "a*b", "*ab*"
using triePrefixList function
*/
void trieSearchWildcard(TRIE *root, char *str) {
	str = strcat(str, "$");
	int length = strlen(str);

	for (int j = 0;str[length-1] != '*';j++) {
		char a1 = str[0];
		for (int i = 0;i < length - 1;i++) {
			str[i] = str[i + 1];
		}
		str[length - 1] = a1;
	}
	
	str = strtok(str, "*");

	triePrefixList(root, str);
}