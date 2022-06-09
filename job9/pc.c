#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

#define N 4
#define COUNT 8

int buffer1[N];
int buffer2[N];

int in1,out1,in2,out2;

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
///////////////
//
sema_t smutex1;
sema_t smutex2;
sema_t empty_smutex1;
sema_t empty_smutex2;
sema_t full_smutex1;
sema_t full_smutex2;


int buffer1_is_empty()
{
	if(in1 == out1)
		return 1;
	else
		return 0;
}

int buffer1_is_full()
{
	if((in1+1)%N == out1)
		return 1;
	else
		return 0;
}

int buffer2_is_empty()
{
        if(in2 == out2)
                return 1;
        else
                return 0;
}

int buffer2_is_full()
{
        if((in2+1)%N == out2)
                return 1;
        else
                return 0;
}

int get_iteam1()
{
	int iteam;
	iteam = buffer1[out1];
	out1 = (out1+1)%N;
	return iteam;
}

int get_iteam2()
{
	int iteam;
	iteam = buffer2[out2];
	out2 = (out2+1)%N;
	return iteam;
}

int put_iteam1(int iteam)
{
	buffer1[in1] = iteam;
	in1 = (in1+1)%N;
}

int put_iteam2(int iteam)
{
	buffer2[in2] = iteam;
	in2 = (in2+1)%N;
}

void *producer(void *arg)
{
	int i;
	int iteam;
	for(i = 0; i < COUNT; i++)
	{
		sema_wait(&empty_smutex1);
		sema_wait(&smutex1);
		
		iteam = 'a'+i;
		put_iteam1(iteam);
		printf("%c\n",iteam);
		
		sema_signal(&smutex1);
		sema_signal(&full_smutex1);
		
	
	}
	return NULL;
}

void *caculater(void *arg)
{
	int i;
	int low_iteam;
	int upper_iteam;
	for(i = 0; i < COUNT; i++)
	{
		sema_wait(&full_smutex1);
		sema_wait(&smutex1);
		
		low_iteam = get_iteam1();
		upper_iteam = low_iteam + 'A' - 'a';
		printf("%c:%c\n",low_iteam, upper_iteam);
		
		sema_signal(&smutex1);
		sema_signal(&empty_smutex1);
		
		//////////////	
		sema_wait(&empty_smutex2);
		sema_wait(&smutex2);
		
		put_iteam2(upper_iteam);
		
		sema_signal(&smutex2);
		sema_signal(&full_smutex2);
		

	}
	return NULL;
}

void *consumer(void *arg)
{
	int i;
	int iteam;
	for(i = 0; i < COUNT; i++)
	{
		
		sema_wait(&full_smutex2);
		sema_wait(&smutex2);

		iteam = get_iteam2();
		printf("%c\n",iteam);
		
		sema_signal(&smutex2);
		sema_signal(&empty_smutex2);
		
	}
	return NULL;
}


int main()
{
	pthread_t prod_tid, cacu_tid,cons_tid;

	in1 = 0;
	in2 = 0;
	out1 = 0;
	out2 = 0;
	sema_init(&smutex1,1);
	sema_init(&smutex2,1);
	sema_init(&empty_smutex1,N-1);
	sema_init(&empty_smutex2,N-1);
	sema_init(&full_smutex1,0);
	sema_init(&full_smutex2,0);
	
	pthread_create(&prod_tid,NULL,producer,NULL);
	pthread_create(&cacu_tid,NULL,caculater,NULL);
	pthread_create(&cons_tid,NULL,consumer,NULL);

	pthread_join(prod_tid,NULL);
	pthread_join(cacu_tid,NULL);
	pthread_join(cons_tid,NULL);
	return 0;
}
