#include "rw_lock.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void init_rwlock(struct rw_lock * rw)
{
	//	Write the code for initializing your read-write lock.
	rw->wlocked = 0;
	rw->rlocked = 0;
	rw->rRequest = 0;
	rw->wRequest = 0;
}

void r_lock(struct rw_lock * rw)
{
	//	Write the code for aquiring read-write lock by the reader.
	int req_check = 0;
	for(;;) {
		pthread_mutex_lock(&mutex);
		if(rw->wlocked == 0) { //write unlocked상태
			if(req_check == 1)
				rw->rRequest--;
			rw->rlocked++; //read lock 횟수 증가
			pthread_mutex_unlock(&mutex);
			break;
		}
		else if(req_check == 0)
			rw->rRequest++, req_check = 1; //write lock일시 request 증가
		pthread_mutex_unlock(&mutex);
	}
}

void r_unlock(struct rw_lock * rw)
{
	//	Write the code for releasing read-write lock by the reader.
	pthread_mutex_lock(&mutex);
	rw->rlocked--; //read lock 횟수 감소
	pthread_mutex_unlock(&mutex);
}

void w_lock(struct rw_lock * rw)
{
	//	Write the code for aquiring read-write lock by the writer.
	int req_check = 0;
	for(;;) {
		pthread_mutex_lock(&mutex);
		if(rw->wlocked == 0 && rw->rlocked == 0 && rw->rRequest == 0) { //unlocked상태, 읽기 요청 없을 시 lock
			if(req_check == 1)
				rw->wRequest--;
			rw->wlocked = 1;
			pthread_mutex_unlock(&mutex);
			break;
		}
		else if(req_check == 0)
			rw->wRequest++, req_check = 1;
		pthread_mutex_unlock(&mutex);
	}
}

void w_unlock(struct rw_lock * rw)
{
	//	Write the code for releasing read-write lock by the writer.
	pthread_mutex_lock(&mutex);
	rw->wlocked = 0;
	pthread_mutex_unlock(&mutex);
}
