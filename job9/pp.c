#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

#define N 4
#define COUNT 20

int tag=0;

typedef struct{
        int value;
        pthread_mutex_t mutex;
        pthread_cond_t  cond;
}sema_t;


///////////////////////////
void sema_init(sema_t *sema, int value)
{
        sema->value = value;
        pthread_mutex_init(&sema->mutex,NULL);
        pthread_cond_init(&sema->cond,NULL);
}

void sema_wait(sema_t *sema)
{
        pthread_mutex_lock(&sema->mutex);
        while(sema->value <= 0)
                pthread_cond_wait(&sema->cond,&sema->mutex);

        sema->value -= 1;
        pthread_mutex_unlock(&sema->mutex);
}

void sema_signal(sema_t *sema)
{
        pthread_mutex_lock(&sema->mutex);
        sema->value += 1;
        pthread_cond_signal(&sema->cond);
        pthread_mutex_unlock(&sema->mutex);
}
////////////////////////

sema_t ping_mutex;
sema_t pong_mutex;


int ping_empty()
{
	if(tag == 0)
		return 1;
	else
		return 0;
}

int pong_empty()
{
	if(tag == 1)
		return 1;
	else
		return 0;
}

int ping()
{
        printf("ping\n");
	tag = 1;
	return 0;
}

int pong()
{
        printf("pong\n");
	tag = 0;
	return 0;
}

void *p_ping(void *arg)
{
	int i;
	for(i = 0; i < COUNT; i++)
	{
		sema_wait(&ping_mutex);
		ping();
		sema_signal(&pong_mutex);
	}
	return NULL;
}

void *p_pong(void *arg)
{
	int i;
	for(i = 0; i < COUNT; i++)
	{
		sema_wait(&pong_mutex);
		pong();
		sema_signal(&ping_mutex);

	}
	return NULL;
}

int main()
{
	pthread_t ping_tid, pong_tid;
	sema_init(&ping_mutex,1);
	sema_init(&pong_mutex,0);
	
	pthread_create(&ping_tid,NULL,p_ping,NULL);
	pthread_create(&pong_tid,NULL,p_pong,NULL);

	pthread_join(ping_tid,NULL);
	pthread_join(pong_tid,NULL);
	return 0;
}
