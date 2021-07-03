//#define DEBUG 1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "trie.h"

// 역색인 헤더 정보에 대한 구조체
typedef struct {
	int		index;	// starting position in posting.idx
	int		df;		// document frequency
} tHEADER;

////////////////////////////////////////////////////////////////////////////////
// 헤더 정보가 저장된 파일(예) "header.idx")을 읽어 메모리에 저장한다.
// 헤더 구조체 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
tHEADER *load_header(char *filename);

// 포스팅 리스트가 저장된 파일(예) "posting.idx")를 읽어 메모리에 저장한다.
// 포스팅 리스트(int arrary) 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
int *load_posting(char *filename);

// 문서 집합을 화면에 출력한다.
void showDocuments(int *docs, int numdocs);

// 두 문서 집합의 교집합을 구한다.
// 교집합을 위한 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
// 교집합의 문서 수는 newnumdocs에 저장한다.
int *intersectDocuments(int *docs, int numdocs, int *docs2, int numdocs2, int *newnumdocs);

// 두 문서 집합의 합집합을 구한다.
// 합집합을 위한 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
// 합집합의 문서 수는 newnumdocs에 저장한다.
int *unionDocuments(int *docs, int numdocs, int *docs2, int numdocs2, int *newnumdocs);

// 단일 텀(single term)을 검색하여 문서를 찾는다.
// 문서 집합을 위한 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
// 검색된 문서 수는 newnumdocs에 저장한다.
int *getDocuments(tHEADER *header, int *posting, TRIE *trie, char *term, int *numdocs);

// 질의(query)를 검색하여 문서를 찾는다.
// 질의는 단일 텀 또는 하나 이상의 불린 연산자('&' 또는 '|')를 포함한 질의가 될 수 있다.
// 문서 집합을 위한 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
// 검색된 문서 수는 newnumdocs에 저장한다.
int *searchDocuments(tHEADER *header, int *posting, TRIE *trie, char *query, int *numdocs);

////////////////////////////////////////////////////////////////////////////////
static char *rtrim(char *str)
{
	char *p = str + strlen(str) - 1;

	while (p >= str)
	{
		if (*p == '\n' || *p == ' ' || *p == '\t') *p = 0;
		else return str;
		p--;
	}
}

static char *ltrim(char *str)
{
	char *p = str;

	while (*p)
	{
		if (*p == '\n' || *p == ' ' || *p == '\t') p++;
		else
		{
			return p;
		}
	}
	return NULL;
}

static char *trim(char *str)
{
	if (str == NULL || *str == 0) return str;

	return rtrim(ltrim(str));
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	tHEADER *header;
	int *posting;
	TRIE *trie;
	char query[100];
	int index;

	header = load_header( "header.idx");
	if (header == NULL) return 1;

	posting = load_posting( "posting.idx");
	if (posting == NULL) return 1;

	trie = dic2trie( "dic.txt");

	printf("\nQuery: ");
	while (fgets(query, 100, stdin) != NULL)
	{
		int numdocs;
		int *docs = searchDocuments( header, posting, trie, query, &numdocs);

		if (docs == NULL) printf( "not found!\n");
		else 
		{
			showDocuments( docs, numdocs);
			free( docs);
		}
		printf("\nQuery: ");
	}

	free( header);
	free( posting);
	trieDestroy( trie);

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// 헤더 정보가 저장된 파일(예) "header.idx")을 읽어 메모리에 저장한다.
// 헤더 구조체 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
tHEADER *load_header(char *filename) {
	FILE* fp;

	fp = fopen(filename, "r");
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	tHEADER *head = (tHEADER*)malloc(size);
	if (head == NULL) return NULL;

	
	fread(head, size, 1, fp);
	
	fclose(fp);

	return head;
}

// 포스팅 리스트가 저장된 파일(예) "posting.idx")를 읽어 메모리에 저장한다.
// 포스팅 리스트(int arrary) 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
int *load_posting(char *filename) {
	FILE* fp;

	fp = fopen(filename, "r");
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	int *arr = (int*)malloc(size);
	if (arr == NULL) return NULL;

	fread(arr, size, 1, fp);
	
	fclose(fp);
	
	return arr;
}

// 문서 집합을 화면에 출력한다.
void showDocuments(int *docs, int numdocs) {
	for (int i = 0;i < numdocs;i++) {
		printf("%d ", docs[i]);
	}
	printf("\n");
}

// 두 문서 집합의 교집합을 구한다.
// 교집합을 위한 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
// 교집합의 문서 수는 newnumdocs에 저장한다.
int *intersectDocuments(int *docs, int numdocs, int *docs2, int numdocs2, int *newnumdocs) {
	int *doc3 = (int*)malloc(200 * sizeof(int));
	if (doc3 == NULL) return NULL;
	int i = 0, j = 0, k = 0;

	if(docs == NULL || docs2 == NULL) {
		free(docs);
		free(docs2);
		free(doc3);
		return NULL;
	}
	
	while (1) {
		if (i < numdocs && j < numdocs2) {
			if (docs[i] > docs2[j]) {
				j++;
			}
			else if (docs[i] < docs2[j]) {
				i++;
			}
			else {
				doc3[k] = docs[i];
				i++;
				j++;
				k++;
			}
		}
		else break;
	}
	if(k == 0) {
		free(docs);
		free(docs2);
		free(doc3);
		return NULL;
	}
	*newnumdocs = k;
	free(docs);
	free(docs2);
	
	return doc3;
}

// 두 문서 집합의 합집합을 구한다.
// 합집합을 위한 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
// 합집합의 문서 수는 newnumdocs에 저장한다.
int *unionDocuments(int *docs, int numdocs, int *docs2, int numdocs2, int *newnumdocs) {
	int *doc3 = (int*)malloc((numdocs + numdocs2) * sizeof(int));
	if (doc3 == NULL) return NULL;
	int i = 0, j = 0, k = 0;
	
	if(docs == NULL) {
		free(docs);
		free(doc3);
		*newnumdocs = numdocs2;
		return docs2;
	}
	if(docs2 == NULL) {
		free(docs2);
		free(doc3);
		*newnumdocs = numdocs;
		return docs;
	}
	
	while (1) {
		if (i < numdocs && j < numdocs2) {
			if (docs[i] > docs2[j]) {
				doc3[k] = docs2[j];
				j++;
			}
			else if (docs[i] < docs2[j]) {
				doc3[k] = docs[i];
				i++;
			}
			else {
				doc3[k] = docs[i];
				i++;
				j++;
			}
			k++;
		}
		else if (i >= numdocs && j < numdocs2) {
			doc3[k] = docs2[j];
			j++;
			k++;
		}
		else if (j >= numdocs2 && i < numdocs) {
			doc3[k] = docs[i];
			i++;
			k++;
		}
		else break;
		
	}
	*newnumdocs = k;
	free(docs);
	free(docs2);
	
	return doc3;
}

// 단일 텀(single term)을 검색하여 문서를 찾는다.
// 문서 집합을 위한 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
// 검색된 문서 수는 numdocs에 저장한다.
int *getDocuments(tHEADER *header, int *posting, TRIE *trie, char *term, int *numdocs) {
	int term_index = trieSearch(trie, term);
	if(term_index==-1) return NULL;
	
	*numdocs = header[term_index].df;
	int start_pos = header[term_index].index;
	
	int* docs = (int*)malloc(*numdocs * sizeof(int));
	if(docs==NULL) return NULL;

	for (int i = 0;i < *numdocs;i++) {
		docs[i] = posting[start_pos];
		start_pos++;
	}
	
	return docs;
}

// 질의(query)를 검색하여 문서를 찾는다.
// 질의는 단일 텀 또는 하나 이상의 불린 연산자('&' 또는 '|')를 포함한 질의가 될 수 있다.
// 문서 집합을 위한 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
// 검색된 문서 수는 numdocs에 저장한다.
int *searchDocuments(tHEADER *header, int *posting, TRIE *trie, char *query, int *numdocs) {
	int *docs, *docs2;
	int numdocs1, numdocs2;
	char bl[10];
	int j = 0;
	char *ptr;

	query = trim(query);

	int length = strlen(query);
	
	if (strchr(query, '|') || strchr(query, '&')) {
		for (int i = 0;i < length;i++) {
			if (query[i] == '&' || query[i] == '|') {
				bl[j] = query[i];
				j++;
			}
		}
		int k = 0;
		ptr = strtok(query, "|&");
		ptr = trim(ptr);
		docs = getDocuments(header, posting, trie, ptr, &numdocs1);
		if(docs == NULL) {
			return NULL;
		}
		while (k < j) {
			if (bl[k] == '|') {
				ptr = strtok(NULL, "|&");
				ptr = trim(ptr);
				docs2 = getDocuments(header, posting, trie, ptr, &numdocs2);
				if(docs2 == NULL) {
					free(docs);
					return NULL;
				}
				docs = unionDocuments(docs, numdocs1, docs2, numdocs2, numdocs);
				numdocs1 = *numdocs;
			}
			else if (bl[k] == '&') {
				ptr = strtok(NULL, "|&");
				ptr = trim(ptr);
				docs2 = getDocuments(header, posting, trie, ptr, &numdocs2);
				if(docs2 == NULL) {
					free(docs);
					return NULL;
				}
				docs = intersectDocuments(docs, numdocs1, docs2, numdocs2, numdocs);
				numdocs1 = *numdocs;
			}
			k++;
		}
	}
	else docs = getDocuments(header, posting, trie, query, numdocs);

	return docs;
}
