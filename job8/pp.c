#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

#define N 4
#define COUNT 20

int tag=0;

pthread_mutex_t mutex;
pthread_cond_t wait_ping;
pthread_cond_t wait_pong;



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
		pthread_mutex_lock(&mutex);
		while(pong_empty())
			pthread_cond_wait(&wait_ping,&mutex);

		ping();
		pthread_cond_signal(&wait_pong);
		pthread_mutex_unlock(&mutex);
	
	}
	return NULL;
}

void *p_pong(void *arg)
{
	int i;
	for(i = 0; i < COUNT; i++)
	{
		pthread_mutex_lock(&mutex);
		while(ping_empty())
			pthread_cond_wait(&wait_pong, &mutex);

		pong();
		pthread_cond_signal(&wait_ping);
		pthread_mutex_unlock(&mutex);

	}
	return NULL;
}

int main()
{
	pthread_t ping_tid, pong_tid;
	pthread_mutex_init(&mutex,NULL);
	pthread_cond_init(&wait_ping,NULL);
	pthread_cond_init(&wait_pong,NULL);
	
	pthread_create(&ping_tid,NULL,p_ping,NULL);
	pthread_create(&pong_tid,NULL,p_pong,NULL);

	pthread_join(ping_tid,NULL);
	pthread_join(pong_tid,NULL);
	return 0;
}
