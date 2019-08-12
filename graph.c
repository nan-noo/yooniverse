#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct graph {
	int** mat; //엣지저장어레이
	int n; //노드개수
}GRAPH;

// 어레이 생성 size가 노드개수(1~size)
GRAPH* createGraph(int size);

//그래프, 어레이들 free
void destroyGraph(GRAPH* graph);

GRAPH* insertEdges(GRAPH* graph, int a, int b);

int depthFirst(GRAPH* graph);

void depth(GRAPH* graph, int* flag, int* stack);

int breadthFirst(GRAPH* graph);

void breadth(GRAPH* graph, int* flag, int* queue);

int main(int argc, char **argv) {
	FILE* fp;
	int size; //파일에서 vertices 개수
	int a, b; //엣지 a-b
	char str[100];
	GRAPH* graph;

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

	fscanf(fp, "%s %d", str, &size);

	graph = createGraph(size);
	fscanf(fp, "%s", str);

	while (fscanf(fp, "  %d   %d", &a, &b) != -1) {
		graph = insertEdges(graph, a, b);
	}

	fclose(fp);

	printf("DFS : "); //깊이우선

	depthFirst(graph);

	printf("BFS : "); //너비우선

	breadthFirst(graph);

	destroyGraph(graph);

	return 0;
}

// 어레이 생성 size가 노드개수(1~size)
GRAPH* createGraph(int size) {
	GRAPH* graph = (GRAPH*)malloc(sizeof(GRAPH));
	if (graph == NULL) return NULL;
	graph->mat = (int**)malloc(size * sizeof(int*));
	if (graph->mat == NULL) return NULL;

	for (int i = 0;i < size;i++) {
		*(graph->mat + i) = (int*)malloc(size * sizeof(int));
		if (*(graph->mat) + i == NULL) return NULL;
	}

	for (int i = 0;i < size;i++) {
		for (int j = 0;j < size;j++) {
			graph->mat[i][j] = 0;
		}
	}

	graph->n = size;

	return graph;
}

//그래프, 어레이들 free
void destroyGraph(GRAPH* graph) {
	for (int i = 0;i < graph->n;i++) {
		free(graph->mat[i]);
	}

	free(graph->mat);
	free(graph);
}

GRAPH* insertEdges(GRAPH* graph, int a, int b) {
	for (int i = 0;i < graph->n;i++) {
		for (int j = 0;j < graph->n;j++) {
			if (i == a - 1 && j == b - 1) graph->mat[i][j] = 1;
			if (i == b - 1 && j == a - 1) graph->mat[i][j] = 1;
		}
	}

	return graph;
}

int depthFirst(GRAPH* graph) {
	int* stack = (int*)malloc(graph->n * sizeof(int));
	if (stack == NULL) return 0;
	int *flag = (int*)malloc(graph->n * sizeof(int));
	if (flag == NULL) return 0;

	for (int i = 0;i < graph->n;i++) {
		stack[i] = -1;
		flag[i] = 0;
	}

	depth(graph, flag, stack);

	free(stack);
	free(flag);

	return 1;
}

void depth(GRAPH* graph, int* flag, int* stack) {
	int top = -1;
	int des;

	for (int i = 0;i < graph->n;i++) {
		if (flag[i] == 0) { //pushstack, flag=1
			top++;
			stack[top] = i;
			flag[i] = 1;

			while (stack[0] != -1) {//popstack, flag=2
				des = stack[top];
				stack[top] = -1;
				top--;
				flag[des] = 2;
				printf("%d ", des + 1);
				for (int j = 0;j < graph->n;j++) {
					if (graph->mat[des][j] == 1 && flag[j] == 0) {
						top++;
						stack[top] = j;
						flag[j] = 1;
					}
				}
			}
		}
	}
	
	printf("\n");
}

int breadthFirst(GRAPH* graph) {
	int* queue = (int*)malloc(graph->n * sizeof(int));
	if (queue == NULL) return 0;
	int *flag = (int*)malloc(graph->n * sizeof(int));
	if (flag == NULL) return 0;

	for (int i = 0;i < graph->n;i++) {
		queue[i] = -1;
		flag[i] = 0;
	}

	breadth(graph, flag, queue);

	free(queue);
	free(flag);

	return 1;
}

void breadth(GRAPH* graph, int* flag, int* queue) {
	int front = 0;
	int rear = -1;
	int des;
				
	
	for (int i = 0;i < graph->n;i++) {
		if (flag[i] == 0) { //enqueue, flag=1
			rear++;
			queue[rear] = i;
			flag[i] = 1;
				
			while (queue[front] != -1) {//dequeue, flag=2
				des = queue[front];
				queue[front] = -1;
				front++;
				flag[des] = 2;
				printf("%d ", des + 1);
				for (int j = 0;j < graph->n;j++) {
					if (graph->mat[des][j] == 1 && flag[j] == 0) {
						rear++;
						queue[rear] = j;
						flag[j] = 1;
					}
				}
				if (front >= graph->n) break;
			}
		}
	}
	printf("\n");
}

