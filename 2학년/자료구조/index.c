#define DEBUG 0

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

// 토큰-문서 구조체
typedef struct {
	char	*token;	// 토큰
	int		docid;	// 문서번호(document ID)
} tTokenDoc;

typedef struct {
	int		index;	// starting position in posting.idx
	int		df;		// 문서 빈도(document frequency)
} tHEADER;

////////////////////////////////////////////////////////////////////////////////
// 토큰 구조체로부터 역색인 파일을 생성한다.
void invertedIndex(tTokenDoc *tokens, int num_tokens,
	char *dicfilename, char *headerfilename, char *postingfilename);

// 입력 파일로부터 토큰-문서 구조체를 생성한다.
tTokenDoc *get_tokens(char *filename, int *num_tokens);

// qsort를 위한 비교함수 (첫번째 정렬 기준: 토큰 문자열, 두번째 정렬 기준: 문서 번호)
static int _compare(const void *n1, const void *n2);

////////////////////////////////////////////////////////////////////////////////
static void print_tokens(tTokenDoc *tokens, int num_tokens)
{
	int i;

	for (i = 0; i < num_tokens; i++)
	{
		printf("%s\t%d\n", tokens[i].token, tokens[i].docid);
	}
}

////////////////////////////////////////////////////////////////////////////////
void destroy_tokens(tTokenDoc *tokens, int num_tokens)
{
	int i;

	for (i = 0; i < num_tokens; i++)
	{
		free(tokens[i].token);
	}
	free(tokens);
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	tTokenDoc *tokens;
	int num_tokens = 0;

	if (argc != 2)
	{
		printf("Usage: %s FILE\n", argv[0]);
		return 2;
	}

	tokens = get_tokens( argv[1], &num_tokens);

	assert( tokens != NULL && num_tokens > 0);

#if DEBUG	
	print_tokens(tokens, num_tokens);
#endif
	int i=0;
	// 정렬 (첫번째 정렬 기준: 토큰 문자열, 두번째 정렬 기준: 문서 번호)
	qsort( tokens, num_tokens, sizeof( tTokenDoc), _compare);

	invertedIndex( tokens, num_tokens, "dic.txt", "header.idx", "posting.idx");

#if DEBUG	
	print_tokens(tokens, num_tokens);
#endif	

	destroy_tokens( tokens, num_tokens);

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// 토큰 구조체로부터 역색인 파일을 생성한다.
void invertedIndex(tTokenDoc *tokens, int num_tokens,
	char *dicfilename, char *headerfilename, char *postingfilename) {
	FILE *fp1, *fp2, *fp3;
	int count = 1;
	int j = 0;
	int k = 0;
	int index = 0;

	fp1 = fopen(dicfilename, "w");
	fp2 = fopen(postingfilename, "w");
	fp3 = fopen(headerfilename, "w");

	for (int i = 0;i < num_tokens;i++) {
		if (i + 1 < num_tokens) {
			if (strcmp(tokens[i].token, tokens[i + 1].token) == 0) continue;
		}
		fwrite(tokens[i].token, strlen(tokens[i].token), 1, fp1);
		if (i < num_tokens - 1) fwrite("\n", sizeof(char), 1, fp1);
		count++;
	}

	tHEADER* head = (tHEADER*)malloc(count * sizeof(tHEADER));
	if (head == NULL) return;
	for (int i = 0;i < count;i++) {
		head[i].df = 1;
	}
	for (int i = 0;i < num_tokens;i++) {
		if (i + 1 < num_tokens) {
			if (strcmp(tokens[i].token, tokens[i + 1].token) == 0 && tokens[i].docid == tokens[i + 1].docid) continue;
			else if (strcmp(tokens[i].token, tokens[i + 1].token) == 0) {
				head[j].df++;
				head[j].index = index;
			}
			else if (strcmp(tokens[i].token, tokens[i + 1].token) != 0) {
				index = index + head[j].df;
				j++;
				head[j].index = index;
			}
		}
		else {
			if (strcmp(tokens[i].token, tokens[i - 1].token) != 0) {
				j++;
				head[j].index = index + 1;
			}
		}
	}

	int *arr = (int*)malloc((index + 1) * sizeof(int));
	for (int i = 0;i < num_tokens;i++) {
		if (i + 1 < num_tokens) {
			if (strcmp(tokens[i].token, tokens[i + 1].token) == 0 && tokens[i].docid == tokens[i + 1].docid) continue;
		}
		arr[k] = tokens[i].docid;
		k++;
	}
	
	fwrite(head, sizeof(tHEADER), count, fp3);
	fwrite(arr, sizeof(int), index + 1, fp2);
	
	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
	free(arr);
	free(head);
}

// 입력 파일로부터 토큰-문서 구조체를 생성한다.
tTokenDoc *get_tokens(char *filename, int *num_tokens) {
	FILE* fp;
	int id = 1;
	int i = 0;
	int n = 620000;
	char word[5000];
	tTokenDoc* td = (tTokenDoc*)malloc(n*sizeof(tTokenDoc));
	if (td == NULL) return NULL;

	fp = fopen(filename, "rt");
	if (fp == NULL)
	{
		fprintf(stderr, "File open error: %s\n", filename);
		return NULL;
	}
	
	while (fgets(word,5000,fp)!=NULL) {
		char *ptr = strtok(word, " \n");
		while (ptr != NULL) {
			td[i].docid = id;
			td[i].token = strdup(ptr);
			i++;
			
			if (i >= n) {
				n = n + 100;
				td = (tTokenDoc*)realloc(td, n * sizeof(tTokenDoc));
			}
			ptr = strtok(NULL, " \n");
		}
		id++;
	}

	*num_tokens = i;
	fclose(fp);

	return td;
}

// qsort를 위한 비교함수 (첫번째 정렬 기준: 토큰 문자열, 두번째 정렬 기준: 문서 번호)
static int _compare(const void *n1, const void *n2) {
	tTokenDoc* first = (tTokenDoc*)n1;
	tTokenDoc* second = (tTokenDoc*)n2;

	if (strcmp(first->token, second->token) != 0) {
		return strcmp(first->token, second->token);
	}
	else return first->docid - second->docid;
}

