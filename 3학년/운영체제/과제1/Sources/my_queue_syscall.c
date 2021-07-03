#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/linkage.h>

#define MAXSIZE 500

int queue[MAXSIZE];
int front = 0;
int rear = 0;
int i, res = 0;

SYSCALL_DEFINE1(oslab_enqueue, int, a){ //int sys_oslab_enqueue(int)
	if(rear >= MAXSIZE - 1){ //if queue is full
		printk(KERN_INFO "[Error] - QUEUE IS FULL-------\n");
		return -2;
	}
	
	for(i = front;i < rear;i++){ //check whether a is in queue
		if(queue[i] == a){ //if exists, return
		    	printk(KERN_INFO "[Error] - Already existing value-------\n");
		    	return a;
		}
	}

	//enqueue
	queue[rear] = a; 
   	rear++;	
	
	//kernel print
	printk(KERN_INFO "[System call] oslab_enqueue(); ----\n");
	printk("Queue Front-----------------------------------\n");
	for(i = front;i < rear;i++){ //print element of queue
		printk("%d\n", queue[i]);
	}
	printk("Queue Rear------------------------------------\n");
	
	return a;
}

SYSCALL_DEFINE0(oslab_dequeue){ //int sys_oslab_dequeue(void)
	if(rear == front){ //if queue is empty
		printk(KERN_INFO "[Error] - EMPTY QUEUE--------\n");
		return -2;
	}
	
	//dequeue
	res = queue[front];
	front++; 

	//kernel print
	printk(KERN_INFO "[System call] oslab_dequeue(); ----\n");
	printk("Queue Front-----------------------------------\n");
	for(i = front;i < rear;i++){ //print element of queue
		printk("%d\n", queue[i]);
	}
	printk("Queue Rear------------------------------------\n");

	return res;
}
