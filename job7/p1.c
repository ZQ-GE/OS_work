#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<math.h>

#define PLUS 1024
typedef void * result_t;

typedef struct result{
	float sum;
}

result_t caclulate_w(result_t argv)
{
	int i = argv;
	int n = argv;
	n += PLUS;
	result *result = (result *)malloc(sizeof(result));
	float sum = 0;
	for(i; i < n; i += 2)
		sum += pow(-1,((i-1)/2)%2)/i;

	result->sum = sum;
	return result;
}

float caclulate_m(int argv)
{       
        int i = argv;
        int n = argv;
        n += PLUS;
        float sum = 0;
        for(i; i < n; i += 2)
                sum += pow(-1,((i-1)/2)%2)/i;

        return sum;
}


int main()
{
	pthread_t work_tid;
	int start = 1;
	float worker, master;
	result * result;
	result argv = (void *) start;
	pthread_creat(work_tid, NULL, caculate_w,argv);
       	master = caculate_m(i+PLUS);
	pthread_join(work_tid,(void **)&result);
	worker = result->sum;
	free(result);
	float PI = 4*(master + worker);
	printf("PI:%f",PI);	

}
