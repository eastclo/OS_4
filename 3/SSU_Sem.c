#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include "SSU_Sem.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void SSU_Sem_init(SSU_Sem *s, int value) {
	s->S = value;  
	s->front = 0;
	s->back = 1;
	memset(s->queue, 0, sizeof(int)*BUFLEN);
}

void SSU_Sem_down(SSU_Sem *s) {
	int cur;

	pthread_mutex_lock(&mutex);
	cur = (s->back) % BUFLEN;
	s->queue[s->back % BUFLEN] = s->back;
	s->back++;
	s->S--;

	//queue 넣기
//	if(s->S < 0) {
		pthread_mutex_unlock(&mutex);
		for(;;) { 
			pthread_mutex_lock(&mutex);
			int front = s->queue[s->front % BUFLEN];
			if(cur == front) {
				pthread_mutex_unlock(&mutex);
				return;
			}
			pthread_mutex_unlock(&mutex);
		}
//	}
//	pthread_mutex_unlock(&mutex);
}

void SSU_Sem_up(SSU_Sem *s) {
	pthread_mutex_lock(&mutex);
	s->S++;

	//queue 빼기
//	if(s->S <= 0)
		s->front++;

	pthread_mutex_unlock(&mutex);
}
