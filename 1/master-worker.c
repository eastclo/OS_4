#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include <unistd.h>

int item_to_produce, item_to_consum, curr_produced_idx, curr_consumed_idx;
int total_items, max_buf_size, num_workers, num_masters;
int *buffer;


pthread_mutex_t master_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t worker_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty_buf_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t full_buf_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t produce_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t consum_cond = PTHREAD_COND_INITIALIZER;

void print_produced(int num, int master) 
{
	printf("Produced %d by master %d\n", num, master);
}

void print_consumed(int num, int worker) 
{
	printf("Consumed %d by worker %d\n", num, worker);
}

//produce items and place in buffer
//modify code below to synchronize correctly
void *generate_requests_loop(void *data)
{
	int thread_id = *((int *)data);

	while(1)
	{
		pthread_mutex_lock(&master_mutex);

		pthread_cond_wait(&produce_cond, &master_mutex); //signal 전까지 대기
		//풀 버퍼인지 확인

		if(curr_produced_idx - curr_consumed_idx == max_buf_size) {
			pthread_cond_wait(&full_buf_cond, &master_mutex);
		}

		//생산 끝이면 락 해제 후 종료
		if(item_to_produce >= total_items) {
			pthread_cond_broadcast(&produce_cond); 
			pthread_mutex_unlock(&master_mutex); //unlock
			break;
		}

		buffer[(curr_produced_idx++ % max_buf_size)] = item_to_produce;
		print_produced(item_to_produce, thread_id);
		item_to_produce++;

		pthread_cond_signal(&empty_buf_cond); //빈 버퍼 아님
		pthread_cond_signal(&produce_cond); //wait에 시그널 보냄
		pthread_mutex_unlock(&master_mutex); //unlock
	}
	return 0;
}

void *generate_consum_loop(void *data)
{
	int thread_id = *((int *)data);

	while(1)
	{
		pthread_mutex_lock(&worker_mutex);
		//빈 버퍼인지 확인
		if(curr_produced_idx - curr_consumed_idx == 0 && item_to_consum < total_items) 
			pthread_cond_wait(&empty_buf_cond, &worker_mutex);

		pthread_cond_wait(&consum_cond, &worker_mutex); //signal 전까지 대기

		if(item_to_consum >= total_items) {
			pthread_cond_broadcast(&empty_buf_cond);
			pthread_cond_broadcast(&consum_cond);
			pthread_mutex_unlock(&worker_mutex);
			break;
		}

		print_consumed(buffer[(curr_consumed_idx++ % max_buf_size)], thread_id);
		item_to_consum++;

		pthread_cond_signal(&full_buf_cond); //풀 버퍼 아님
		pthread_cond_signal(&consum_cond); //wait에 시그널 보냄
		pthread_mutex_unlock(&worker_mutex); //unlock
	}
}

//write function to be run by worker threads
//ensure that the workers call the function print_consumed when they consume an item

int main(int argc, char *argv[])
{
	int *master_thread_id;
	int *worker_thread_id;
	pthread_t *master_thread;
	pthread_t *worker_thread;
	item_to_produce = 0;
	item_to_consum = 0;
	curr_produced_idx = 0;
	curr_consumed_idx = 0;

	int i;

	if (argc < 5) {
		printf("./master-worker #total_items #max_buf_size #num_workers #masters e.g. ./exe 10000 1000 4 3\n");
		exit(1);
	}
	else {
		num_masters = atoi(argv[4]); //P : 마스터 쓰레드 수
		num_workers = atoi(argv[3]); //C : 워커쓰레드 수
		total_items = atoi(argv[1]); //M : 생성할 숫자, 0 ~ M-1 생성
		max_buf_size = atoi(argv[2]); //N : 버퍼 최대 크기
	}

	buffer = (int *)malloc (sizeof(int) * max_buf_size);

	//create master producer threads
	master_thread_id = (int *)malloc(sizeof(int) * num_masters);
	master_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_masters);
	for (i = 0; i < num_masters; i++)
		master_thread_id[i] = i;

	for (i = 0; i < num_masters; i++)
		pthread_create(&master_thread[i], NULL, generate_requests_loop, (void *)&master_thread_id[i]);

	//create worker consumer threads
	worker_thread_id = (int *)malloc(sizeof(int) * num_workers);
	worker_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_workers);
	for (i = 0; i < num_workers; i++)
		worker_thread_id[i] = i;

	for (i = 0; i < num_workers; i++)
		pthread_create(&worker_thread[i], NULL, generate_consum_loop, (void *)&worker_thread_id[i]);

	//signal 
	pthread_cond_signal(&produce_cond);
	sleep(1);
	pthread_cond_signal(&consum_cond);

	//wait for all threads to complete
	for (i = 0; i < num_masters; i++)
	{
		pthread_join(master_thread[i], NULL);
		printf("master %d joined\n", i);
	}

	for (i = 0; i < num_workers; i++)
	{
		pthread_join(worker_thread[i], NULL);
		printf("worker %d joined\n", i);
	}

	pthread_mutex_destroy(&master_mutex);
	pthread_mutex_destroy(&worker_mutex);
	pthread_cond_destroy(&empty_buf_cond);
	pthread_cond_destroy(&full_buf_cond);
	pthread_cond_destroy(&produce_cond);
	pthread_cond_destroy(&consum_cond);

	/*----Deallocating Buffers---------------------*/
	free(buffer);
	free(master_thread_id);
	free(master_thread);

	return 0;
}