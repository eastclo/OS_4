#include <pthread.h>

#define BUFLEN 1024

typedef struct SSU_Sem {
	int S;
	int front, back;
	int queue[BUFLEN];
} SSU_Sem;

void SSU_Sem_init(SSU_Sem *, int);
void SSU_Sem_up(SSU_Sem *);
void SSU_Sem_down(SSU_Sem *);
