#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define QUEUE_SIZE 4

void *reader(void *);
void *writer(void *);


sem_t rc_mutex[QUEUE_SIZE];				// to change read count
sem_t w_mutex;				// to alter the queue
sem_t rw_mutex[QUEUE_SIZE];	// to alter/read a particular index of queue
int read_count[QUEUE_SIZE]; 


//queue
int front;
int rear;
int size;
int max_size;
int arr[QUEUE_SIZE];


typedef struct reader_args{
	int id;
	int whatToDo;
	int index;
} Reader_Args;

typedef struct writer_args{
	int id;
	int msg;
} Writer_Args;


void *reader(void *args){

	Reader_Args *init = (Reader_Args *)args;

	int id = init->id;
	int whatToDo;
	if(init->whatToDo == -1){
		whatToDo = rand()%8;					// choose randomly what to do
		if(whatToDo < 6){
			whatToDo = 1;						// read random index
		}else{
			whatToDo = 0;						// dequeue
		}
	}else{
		whatToDo = init->whatToDo;
	}


	if(whatToDo == 1){							// read index from queue
		int index;
		if(init->index == -1){
			//printf("Reader%d selecting randomly\n", id);
			srand(time(0)+id);
			index = rand()%QUEUE_SIZE;	//choose a random index to read
		}else{
			index = init->index;
		}

		printf(">>>Reader%d attempting to read index %d\n", id, index);

		if(index < 0 || index >= QUEUE_SIZE)	{
			printf("Reader%d could not read: index %d out of range\n", id, index);
		}
		if(size > index){	// a valid index to read
			int ind = (front+index)%max_size;	// actual index in arr
			
			sem_wait(&rc_mutex[ind]);

			read_count[ind]++;	
			if(read_count[ind] == 1){
				sem_wait(&rw_mutex[ind]);
			}
			sem_post(&rc_mutex[ind]);

			sleep(2);
			printf("Reader%d read '%d' from index %d\n", id, arr[ind], index);

			sem_wait(&rc_mutex[ind]);
			read_count[ind]--;
			if(read_count[ind] == 0){
				sem_post(&rw_mutex[ind]);
			}
			sem_post(&rc_mutex[ind]);

		}else{
			printf("Reader%d could not read: index %d empty\n", id, index);
		}


	}else{						//attempt dequeue

		sem_wait(&w_mutex);			// now no one else can write
		printf("Reader%d got w_mutex\n", id);


		printf(">>>Reader%d attempting to dequeue \n", id);
		printf("Reader%d: queue size %d\n",id, size);
		if(size == 0){
				printf("Reader%d queue is empty, not dequeued\n", id);
		}else{

			printf("Reader%d attempt to lock rw_mutex%d \n", id, front);
			sem_wait(&rw_mutex[front]);	//lock this index
			printf("Reader%d acquired lock rw_mutex%d \n", id, front);

			int tmp = front;
			front = (front+1)%max_size;
			size--;
			sleep(3);
			printf("Reader%d dequeued from queue: %d\n", id, arr[tmp]);

			printf("Reader%d released lock rw_mutex%d \n", id, tmp);
			sem_post(&rw_mutex[tmp]);
		}

		printf("Reader%d released w_mutex\n", id);
		sem_post(&w_mutex);
	}

	
}


void *writer(void *args){

	Writer_Args *init = (Writer_Args *)args;

	int id = init->id;
	int msg;
	if(init->msg == -1){
		srand(time(0)+id);	
		msg = rand()%10000 + 10;				//write random msg (int from 10 to 10009)
	}else{
		msg = init->msg;
	}
	


	sem_wait(&w_mutex);			// now no one else can write
	printf("Writer%d got w_mutex\n", id);


	// attempt enqueue

	printf(">>>Writer%d attempting to enqueue %d\n", id, msg);

	if(size == max_size){
		printf("Writer%d queue is full, not enqueued: %d\n", id, msg);
	}else{
		printf("Writer%d attempting to lock rw_mutex%d \n", id, (rear+1)%max_size);
		sem_wait(&rw_mutex[(rear+1)%max_size]);	//lock this index
		printf("Writer%d acquired lock rw_mutex%d \n", id, (rear+1)%max_size);

		rear = (rear+1)%max_size;
		arr[rear] = msg;
		size++;
		sleep(3);
		printf("Writer%d enqueued to queue: %d\n", id, msg);

		printf("Writer%d released lock rw_mutex%d \n", id, rear);
		sem_post(&rw_mutex[rear]);
	}
	
	printf("Writer%d released w_mutex\n", id);
	sem_post(&w_mutex);
}



int main(){

	//init mutexes
	sem_init(&w_mutex, 0, 1);
	for(int i = 0; i < QUEUE_SIZE; i++){
		sem_init(&rw_mutex[i], 0, 1);
		read_count[i] = 0;
		sem_init(&rc_mutex[i], 0, 1);
	}

	//init queue
	max_size = QUEUE_SIZE;
	front = 0;
	rear = QUEUE_SIZE-1;
	size = 0;
	for(int i = 0; i < QUEUE_SIZE; i++){
		arr[i] = 0;
	}
	//init random generator
	srand(time(0));

	int num_threads;
	printf("enter number of threads to spawn \n");
	scanf("%d", &num_threads);

	pthread_t threads[num_threads];


	for(int i = 0; i<num_threads; i++){
		int whoAmI = rand()%8;	//generate reader/writer randomly

		//spawn writer: reader = 3:5 randomly
		if(whoAmI > 4){			//spawn a writer
			printf("Spawned Writer%d\n", i);
			Writer_Args *tmp = malloc(sizeof(Writer_Args));
			tmp->id = i; tmp->msg = -1;
			pthread_create(&threads[i], NULL, writer, (void *)(tmp));

		}else{					//spawn a reader
			printf("Spawned Reader%d\n", i);
			Reader_Args *tmp = malloc(sizeof(Reader_Args));
			tmp->id = i; tmp->whatToDo = -1; tmp->index = -1;
			pthread_create(&threads[i], NULL, reader, (void *)(tmp));
		}

	}
	for(int i = 0; i<num_threads; i++){
		pthread_join(threads[i], NULL);
	}

	printf("\n\nthreads joined\n");

	//destroy mutexes
	sem_destroy(&w_mutex);
	for(int i = 0; i < QUEUE_SIZE; i++){
		sem_destroy(&rw_mutex[i]);
		sem_destroy(&rc_mutex[i]);	
	}
	printf("semaphores destroyed\n");

}
