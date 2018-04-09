#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#define QUEUE_SIZE 4
#define MSG_SIZE



int SHM_SIZE = QUEUE_SIZE*MSG_SIZE;	
key_t key = ftok("queueFile", 'W');

struct Msg{
	char[MSG_SIZE] msg;
}

//queue
int front = 0;
int rear = QUEUE_SIZE-1;
int size = 0;
int max_size = QUEUE_SIZE;
struct Msg[QUEUE_SIZE] arr;

// void init(int capacity){
// 	max_size = capacity;
// 	size = 0;
// 	front = 0;
// 	rear = max_size-1;
// 	arr = 
// }

int enqueue(char* msg){
	if(size == max_size){
		fprintf(stderr, "queue is full, not enqueued: %s\n", msg);
		return -1;
	}
	rear = (rear+1)%max_size;
	strncpy(arr[rear].msg, msg, MSG_SIZE);
	size++;

	printf("enqueued to queue: %s\n", msg);
	return 0;
}

int dequeue(char *buff){
	if(size == 0){
		fprintf(stderr, "queue is empty, not dequeued: %s\n", msg);
		return -1;
	}
	int tmp = front;
	front = (front+1)%max_size;
	size--;
	strcpy(buff, arr[tmp].msg, MSG_SIZE);
	printf("dequeued from queue: %s\n", msg);
	return 0;
}


void write(int index, char *msg){

	if(index < 0 || index > QUEUE_SIZE)	{
		fprintf(stderr, "could not write: index %d out of range\n", index);
		return -1;
	}
	strcpy(buff, arr[(front+index)%max_size].msg, MSG_SIZE);
	return 0;
}

int read(int index, char *buff){
	if(index < 0 || index > QUEUE_SIZE)	{
		fprintf(stderr, "could not read: index %d out of range\n", index);
		return -1;
	}
	if(size > index){
		strcpy(buff, arr[(front+index)%max_size].msg, MSG_SIZE);
		return 0;
	}else{
		fprintf(stderr, "could not read: index %d empty\n", index);
		return -1;
	}
}


void reader(){


}


void writer(){

}


int main(){
	//init mutexes



	int num_thread = 20;

	pthread_t thread_id[num_thread];

	for(int i = 0; i<num_thread; i++){
		//TODO: selected reader, writer randomly
		int writer = ;//generate reader/writer randomly
		if(writer){
			pthread_create(&thread_id[i], NULL, );

		}else{
			pthread_create(&thread_id[i], NULL, );
		}

	}
	for(int i = 0; i<num_thread; i++){
		pthread_join(thread_id[i], NULL);
	}
}