#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<math.h>

#define PLUS 10000
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
	return rst;
}

double caculate_m(int argv)
{       
        int i = argv;
        int n = argv;
        n += PLUS;
        double sum = 0;
        for(i; i < n; i += 2)
        {               
                double b = ((i-1)/2)%2;
                sum += pow(-1,b)/i;
        }

        return sum;
}


int main()
{
	pthread_t work_tid;
	int start = 1;
	double worker, master;
	result * result;
	result_t argv = (void *)start;
	pthread_create(&work_tid, NULL, caculate_w,argv);
       	master = caculate_m(start+PLUS);
	pthread_join(work_tid,(void **)&result);
	worker = result->sum;
	free(result);
	//printf("%f:%f",master,worker);
	double PI = 4*(master + worker);
	printf("PI:%f\n",PI);	

}
