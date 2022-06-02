#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<math.h>

#define PLUS 10000
#define N 3
typedef void * result_t;

typedef struct result_m{
	double sum;
}result;

result_t caculate_w(result_t argv)
{
	int i = (int)argv;
	int n = (int)argv;
	n += PLUS;
	result *rst = malloc(sizeof(result));
	double sum = 0;
	for(i; i < n; i += 2)
	{
		double b = ((i-1)/2)%2;
		sum += pow(-1,b)/i;
	}

	rst->sum = sum;
	//printf("%f:%f\n",rst->sum,sum);
	return rst;
}

int main()
{
	pthread_t work_tid[N];
	double sum = 0;
	double PI;
	for(int i = 0; i < N; i++)
	{
		int start = PLUS*i+1; 	
		result_t argv = (void *)start;
		pthread_create(&(work_tid[i]), NULL, caculate_w,argv);
       	}
	for(int i = 0; i < N; i++)
	{
		result *result;
		pthread_join(work_tid[i],(void **)&result);
		sum += result->sum;
		//printf("%f:%f\n",result->sum,sum);
		free(result);
	}
	PI = 4*sum;
	printf("PI:%f\n",PI);	

}
