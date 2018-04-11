#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#define QUEUE_SIZE 4

void *reader(void *);
void *writer(void *);

sem_t mutex;				// to change read count
sem_t w_mutex;				// to alter the queue
sem_t[QUEUE_SIZE] rw_mutex;	// to alter/read a particular index of queue
int[QUEUE_SIZE] read_count; 


//queue
int front;
int rear;
int size;
int max_size;
int[QUEUE_SIZE] arr;



void *reader(void *args){
	int id = (int)args;

	int index = rand()%QUEUE_SIZE;	//choose a random index to read
	printf("Reader%d attempting to read index %d\n", id, index);

	if(index < 0 || index >= QUEUE_SIZE)	{
		fprintf(stderr, "Reader%d could not read: index %d out of range\n", id, index);
		return -1;
	}
	if(size > index){	// a valid index to read
		sem_wait(&mutex);
		int ind = (front+index)%max_size;	// actual index in arr
		read_count[ind]++;	
		if(read_count == 1){
			sem_wait(&rw_mutex[ind]);
		}
		sem_post(&mutex);

		printf("Reader%d read '%d' from index %d\n", arr[ind], id, index);

		sem_wait(&mutex);
		read_count[ind]--;
		if(read_count[ind] == 0){
			sem_post(&rw_mutex[ind]);
		}
		sem_post(&mutex);

	}else{
		fprintf(stderr, "Reader%d could not read: index %d empty\n", id, index);
		return -1;
	}
}


void *writer(void *args){
	int id = (int)args;

	sem_wait(&w_mutex);			// now no one else can write

	int whatToDo = rand()%2;	// choose randomly what to do

	if(whatToDo == 0){			// '0' so attempt enqueue

		msg = rand()%10000 + 10;				//write random msg (int from 10 to 10009)
		printf("Writer%d attempting to enqueue %d\n", id, msg);

		if(size == max_size){
			fprintf(stderr, "Writer%d queue is full, not enqueued: %d\n", id, msg);
		}else{
			sem_wait(&rw_mutex[(rear+1)%max_size]);	//lock this index
			
			rear = (rear+1)%max_size;
			arr[rear] = msg;
			size++;
			printf("Writer%d enqueued to queue: %d\n", id, msg);

			sem_post(&rw_mutex[rear]);
		}
	}else{						//attempt dequeue

		printf("Writer%d attempting to dequeue \n", id);

		if(size == 0){
				fprintf(stderr, "Writer%d queue is empty, not dequeued: %s\n", id, msg);
		}else{
			sem_wait(&rw_mutex[front]);	//lock this index
			
			int tmp = front;
			front = (front+1)%max_size;
			size--;
			printf("Writer%d dequeued from queue: %d\n", id, arr[tmp]);

			sem_post(&rw_mutex[tmp]);
		}
	}
	sem_post(&w_mutex);
}


int main(){

	//init mutexes
	sem_init(&mutex, 0, 1);
	sem_init(&w_mutex, 0, 1);
	for(int i = 0; i < QUEUE_SIZE; i++){
		sem_init(&rw_mutex[i], 0, 1);
	}

	for(int i = 0; i < QUEUE_SIZE; i++){
		read_count[i] = 0;
	}

	//init queue
	max_size = QUEUE_SIZE;
	front = 0;
	rear = 0;
	size = 0;
	for(int i = 0; i < QUEUE_SIZE; i++){
		arr[i] = 0;
	}

	//init random generator
	time_t t;
	srand((unsigned) time(&t));

	int num_threads;
	scanf("enter number of threads to spawn \n", &num_threads);

	pthread_t threads[num_threads];


	for(int i = 0; i<num_threads; i++){
		int whoAmI = rand()%8;	//generate reader/writer randomly

		//spawn writer: reader = 3:5 randomly
		if(whoAmI < 3){			//spawn a writer if whoAmI<2
			printf("Spawned Writer%d\n", i);
			pthread_create(&thread_id[i], NULL, writer, (void *)(i));

		}else{					//spawn a reader if whoAmI>=2
			printf("Spawned Reader%d\n", i);
			pthread_create(&thread_id[i], NULL, reader, (void *)(i));
		}

	}
	for(int i = 0; i<num_thread; i++){
		pthread_join(thread_id[i], NULL);
	}


	//destroy mutexes
	sem_destroy(&mutex);
	sem_destroy(&w_mutex);
	for(int i = 0; i < QUEUE_SIZE; i++){
		sem_destroy(&rw_mutex[i]);
	}
}