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

HEAP *heapCreate( int capacity);
void heapDestroy( HEAP *heap);
int heapInsert( HEAP *heap, int data);
static void _reheapUp( HEAP *heap, int index);
int heapDelete( HEAP *heap, int* data);
static void _reheapDown( HEAP *heap, int index);
void heapPrint( HEAP *heap);

int main(void)
{
	HEAP *heap;
	int data;
	
	heap = heapCreate(MAX_ELEM);
	
	srand( time(NULL));
	
	for (int i = 1; i <= MAX_ELEM; i++)
	{
		data = rand() % MAX_ELEM * 3 + 1; // 1 ~ MAX_ELEM*3 random number
				
		fprintf( stdout, "Inserting %d: ", data);
		
		// insert function call
		heapInsert( heap, data);
		
		heapPrint( heap);
 	}

	while (heap->last >= 0)
	{
		// delete function call
		heapDelete( heap, &data);

		fprintf( stdout, "Deleting %d: ", data);

		heapPrint( heap);
 	}
	
	heapDestroy( heap);
	
	return 0;
}

void heapDestroy( HEAP *heap)
{
	free(heap->heapArr);
	free(heap);
}

HEAP *heapCreate( int capacity)
{
	HEAP *heap;
	
	heap = (HEAP *)malloc( sizeof(HEAP));
	if (!heap) return NULL;

	heap->last = -1;
	heap->capacity = capacity;
	heap->heapArr = (int *)malloc( sizeof(int) * capacity);
	if (heap->heapArr == NULL)
	{
		fprintf( stderr, "Error : not enough memory!\n");
		free( heap);
		return NULL;
	}
	return heap;
}

void heapPrint( HEAP *heap)
{
	int i;
	int *p = heap->heapArr;
	int last = heap->last;
	
	for( i = 0; i <= last; i++, p++)
	{
		printf("%6d", *p);
		if ((i+1) % 20 == 0) printf( "\n");
	}
	printf( "\n");
}

void _reheapUp( HEAP *heap, int index)
{
	int *arr = heap->heapArr;
	int parent;
	
	while(1)
	{
		if (index == 0) return; // root node
		
		parent = (index - 1) / 2;
		
		if (arr[index] > arr[parent]) // exchange (for maxheap)
		{
			int temp = arr[index];
			arr[index] = arr[parent];
			arr[parent] = temp;
			
			index = parent;
		}
		else return;
	}
}

int heapInsert( HEAP *heap, int data)
{
	if (heap->last == heap->capacity - 1)
		return 0;
	
	(heap->last)++;
	heap->heapArr[heap->last] = data;
	
	_reheapUp( heap, heap->last);
	
	return 1;
}

void _reheapDown( HEAP *heap, int index)
{
	int *arr = heap->heapArr;
	int leftData;
	int rightData;
	int noright = 0;
	int largeindex; // index of left or right child with large key
	
	while(1)
	{
		if ((index * 2 + 1) > heap->last) return; // leaf node (there is no left subtree)
		
		leftData = arr[index * 2 + 1];
		if (index * 2 + 2 <= heap->last) rightData = arr[index * 2 + 2];
		else noright = 1;
		
		if (noright || leftData > rightData) largeindex = index * 2 + 1; // left child
		else largeindex = index * 2 + 2; // right child
		
		if (arr[index] < arr[largeindex]) // exchange (for maxheap)
		{
			int temp = arr[index];
			arr[index] = arr[largeindex];
			arr[largeindex] = temp;
			
			index = largeindex;
			
			noright = 0;
		}
		else return;
	}
}

int heapDelete( HEAP *heap, int* data)
{
	if (heap->last == -1) return 0; // empty heap
	
	*data = heap->heapArr[0];
	heap->heapArr[0] = heap->heapArr[heap->last];
	
	(heap->last)--;
	
	_reheapDown( heap, 0);
	
	return 1;
}
