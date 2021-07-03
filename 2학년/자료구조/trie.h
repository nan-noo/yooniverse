#define MAX_DEGREE	27 // 'a' ~ 'z' and EOW
#define EOW			'$' // end of word
#define getIndex(x)		(((x) == EOW) ? MAX_DEGREE-1 : ((x) - 'a'))

// TRIE type definition
typedef struct trieNode {
	int 			index; // 0, 1, 2, ...
	struct trieNode	*subtrees[MAX_DEGREE];
} TRIE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a trie node and returns its address to caller
	return	node pointer
			NULL if overflow
*/
TRIE *trieCreateNode(void) {
	TRIE* trie = (TRIE*)malloc(sizeof(TRIE));
	if (trie == NULL) return NULL;

	trie->index = -1;

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

	free(root);
	return;
}

/* Inserts new entry into the trie
	return	1 success
			0 failure
*/
// 주의! 엔트리를 중복 삽입하지 않도록 체크해야 함
// 대소문자를 소문자로 통일하여 삽입
// 영문자 외 문자를 포함하는 문자열은 삽입하지 않음
int trieInsert(TRIE *root, char *str, int dic_index) {
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
	if (tCur->index == -1) tCur->index = dic_index;

	return 1;
}

/* Retrieve trie for the requested key
	return	index in dictionary (trie) if key found
			-1 key not found
*/
int trieSearch(TRIE *root, char *str) {
	TRIE* tCur = root;
	int index;

	if (root == NULL) return -1;

	for (int i = 0;str[i] != '\0';i++) {
		if (isupper(str[i])) str[i] = tolower(str[i]);
	}

	for (int i = 0;str[i] != '\0';i++) {
		index = getIndex(str[i]);
		if (!(index >= 0 && index < 27)) return -1;
		tCur = tCur->subtrees[index];

		if (tCur == NULL) return -1;
	}

	if (tCur->index != -1) return tCur->index;
	else return -1;
}

/* prints all entries in trie using preorder traversal
*/
void trieList(TRIE *root) {
	if (root == NULL) return;

	if (root->index != -1) printf("%d\n", root->index);

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

	for (int j = 0;str[length - 1] != '*';j++) {
		char a1 = str[0];
		for (int i = 0;i < length - 1;i++) {
			str[i] = str[i + 1];
		}
		str[length - 1] = a1;
	}

	str = strtok(str, "*");

	triePrefixList(root, str);
}

/* makes a trie for given dictionary file
	return	trie head node pointer
			NULL failure
*/ 
TRIE *dic2trie(char *dicfile) {
	TRIE *trie;
	int ret;
	char str[100];
	FILE *fp;
	int num_p;
	int dic_index = 0;

	fp = fopen(dicfile, "rt");

	trie = trieCreateNode(); // original trie
	
	printf("Inserting to trie...\t");

	while (fscanf(fp, "%s", str) == 1) // words file
	{
		ret = trieInsert(trie, str, dic_index);
		dic_index++;
	}
	
	printf("[done]\n");

	fclose(fp);

	return trie;
}


/* makes a permuterm trie for given dictionary file
	return	trie head node pointer
			NULL failure
*/ 
TRIE *dic2permute_trie(char *dicfile) {
	TRIE *permute_trie;
	int ret;
	char str[100];
	FILE *fp;
	char *permuterms[100];
	int num_p;
	int dic_index = 0;

	fp = fopen(dicfile, "rt");

	permute_trie = trieCreateNode(); // trie for permuterm index

	printf("Inserting to trie...\t");
	while (fscanf(fp, "%s", str) == 1) // words file
	{
		num_p = make_permuterms(str, permuterms);

		for (int i = 0; i < num_p; i++) {
			trieInsert(permute_trie, permuterms[i], dic_index);
		}
		
		dic_index++;
		clear_permuterms(permuterms, num_p);
	}

	fclose(fp);

	return permute_trie;
}
