#include <stdio.h>
#include <stdlib.h> // malloc, rand
#include <time.h> // time
#include <assert.h>

#define MAX_ELEM	20

typedef struct
{
	int *heapArr;
	int	last;
	int	capacity;
} HEAP;

HEAP *heapCreate(int capacity) {
	HEAP* heap;
	heap = (HEAP*)malloc(sizeof(HEAP));
	if (heap == NULL) return NULL;

	heap->heapArr = (int*)malloc(sizeof(int)*capacity);
	if (heap->heapArr == NULL) return NULL;
	
	heap->last = 0;
	heap->capacity = capacity;

	return heap;
};

void heapDestroy(HEAP *heap) {
	free(heap->heapArr);
	free(heap);
};

static void _reheapUp(HEAP *heap, int index) { //부모보다 크면 자리바꿈
	int temp = heap->heapArr[index];
	if (index == 1) return;

	if (heap->heapArr[index] > heap->heapArr[index / 2]) {
		heap->heapArr[index] = heap->heapArr[index / 2];
		heap->heapArr[index / 2] = temp;
		_reheapUp(heap, index / 2);
	}
	else return;
};

int heapInsert(HEAP *heap, int data) { //맨 끝 자리에 넣음
	heap->last = heap->last + 1;
	heap->heapArr[heap->last] = data;

	_reheapUp(heap, heap->last);

	return 1;
};

static void _reheapDown(HEAP *heap, int index) { //자식이랑 크기비교해서 바꿈
	int temp = heap->heapArr[index];

	if (heap->last == 0) return;

	if (index * 2 <= heap->last) {
		if (index * 2 + 1 <= heap->last) {
			if (heap->heapArr[index * 2] > heap->heapArr[index * 2 + 1]) { //왼쪽이 더 큼
				if (heap->heapArr[index * 2] > heap->heapArr[index]) {
					heap->heapArr[index] = heap->heapArr[index * 2];
					heap->heapArr[index * 2] = temp;

					_reheapDown(heap, index * 2);
				}
				else return;
			}
			else {
				if (heap->heapArr[index * 2 + 1] > heap->heapArr[index]) {
					heap->heapArr[index] = heap->heapArr[index * 2 + 1];
					heap->heapArr[index * 2 + 1] = temp;

					_reheapDown(heap, index * 2 + 1);
				}
				else return;
			}
		}
		else {
			if (heap->heapArr[index * 2] > heap->heapArr[index]) {
				heap->heapArr[index] = heap->heapArr[index * 2];
				heap->heapArr[index * 2] = temp;

				_reheapDown(heap, index * 2);
			}
			else return;
		}
	}
	
	return;
};

int heapDelete(HEAP *heap, int* data) { //루트삭제 작은거랑 바꿈
	*data = heap->heapArr[1];

	heap->heapArr[1] = heap->heapArr[heap->last];
	heap->last = heap->last - 1;

	_reheapDown(heap, 1);

	return 1;
};

void heapPrint(HEAP *heap) {
	for (int i = 1;i <= heap->last;i++) {
		printf("   %d", heap->heapArr[i]);
	}
	printf("\n");
};

int main(void)
{
	HEAP *heap;
	int data;

	heap = heapCreate(MAX_ELEM);

	srand(time(NULL));

	for (int i = 1; i < MAX_ELEM; i++)
	{
		data = rand() % MAX_ELEM * 3 + 1; // 1 ~ MAX_ELEM*3 random number

		fprintf(stdout, "Inserting %d: ", data);

		// insert function call
		heapInsert( heap, data);

		heapPrint( heap);
	}

	while (heap->last > 0)
	{
		// delete function call
		heapDelete( heap, &data);

		fprintf( stdout, "Deleting %d: ", data);

		heapPrint( heap);
	}

	heapDestroy( heap);

	return 0;
}