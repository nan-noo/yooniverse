#include <unistd.h>
#include <stdio.h>

#define my_queue_enqueue 335
#define my_queue_dequeue 336


int main(){

	int a = 0;

	//enqueue
	a = syscall(my_queue_enqueue, 1);
	printf("Enqueue: ");
	printf("%d\n", a);

	a = syscall(my_queue_enqueue, 2);
	printf("Enqueue: ");
	printf("%d\n", a);

	a = syscall(my_queue_enqueue, 3);
	printf("Enqueue: ");
	printf("%d\n", a);

	a = syscall(my_queue_enqueue, 3);
	printf("Enqueue: ");
	printf("%d\n", a);

	//dequeue
	a = syscall(my_queue_dequeue);
	printf("Dequeue: ");
	printf("%d\n", a);

	a = syscall(my_queue_dequeue);
	printf("Dequeue: ");
	printf("%d\n", a);

	a = syscall(my_queue_dequeue);
	printf("Dequeue: ");
	printf("%d\n", a);	

	return 0;
}
