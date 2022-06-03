#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

#define N 4
#define COUNT 8
int buffer1[N];
int buffer2[N];

int in1,out1,in2,out2;

pthread_mutex_t mutex1;
pthread_mutex_t mutex2;
pthread_cond_t wait_empty_buffer1;
pthread_cond_t wait_empty_buffer2;
pthread_cond_t wait_full_buffer1;
pthread_cond_t wait_full_buffer2;


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
		pthread_mutex_lock(&mutex1);
		while(buffer1_is_full())
			pthread_cond_wait(&wait_empty_buffer1,&mutex1);

		iteam = 'a'+i;
		put_iteam1(iteam);
		printf("%c\n",iteam);
		pthread_cond_signal(&wait_full_buffer1);
		pthread_mutex_unlock(&mutex1);
	
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
		pthread_mutex_lock(&mutex1);
		while(buffer1_is_empty())
			pthread_cond_wait(&wait_full_buffer1,&mutex1);

		low_iteam = get_iteam1();
		upper_iteam = low_iteam + 'A' - 'a';
		printf("%c:%c\n",low_iteam, upper_iteam);
		pthread_cond_signal(&wait_empty_buffer1);
		pthread_mutex_unlock(&mutex1);

		pthread_mutex_lock(&mutex2);
		while(buffer2_is_full())
			pthread_cond_wait(&wait_empty_buffer2,&mutex2);
		put_iteam2(upper_iteam);
		pthread_cond_signal(&wait_full_buffer2);
		pthread_mutex_unlock(&mutex2);
	}
	return NULL;
}

void *consumer(void *arg)
{
	int i;
	int iteam;
	for(i = 0; i < COUNT; i++)
	{
		pthread_mutex_lock(&mutex2);
		while(buffer2_is_empty())
			pthread_cond_wait(&wait_full_buffer2,&mutex2);

		iteam = get_iteam2();
		printf("%c\n",iteam);
		pthread_cond_signal(&wait_empty_buffer2);
		pthread_mutex_unlock(&mutex2);
	}
	return NULL;
}


int main()
{
	pthread_t prod_tid, cacu_tid,cons_tid;
	pthread_mutex_init(&mutex1,NULL);
	pthread_mutex_init(&mutex2,NULL);
	pthread_cond_init(&wait_empty_buffer1,NULL);
	pthread_cond_init(&wait_empty_buffer2,NULL);
	pthread_cond_init(&wait_full_buffer1,NULL);
	pthread_cond_init(&wait_full_buffer1,NULL);
	
	pthread_create(&prod_tid,NULL,producer,NULL);
	pthread_create(&cacu_tid,NULL,caculater,NULL);
	pthread_create(&cons_tid,NULL,consumer,NULL);

	pthread_join(prod_tid,NULL);
	pthread_join(cacu_tid,NULL);
	pthread_join(cons_tid,NULL);
	return 0;
}
