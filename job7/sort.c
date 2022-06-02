#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<math.h>


int array[10] = {5, 7, 6, 9, 12, 3, 13, 21, 75, 1};

typedef void * result_t;

typedef struct param{
	int start;
	int end;
}params;

result_t select_sort(result_t argv)
{
	params *pr;
	params *result;
	result = malloc(sizeof(params));
	pr = (params *)argv;
	result->start = pr->start;
	result->end = pr->end;
	//printf("%d %d\n",pr->start, pr->end);
	int i, j, max, k;
	for(i = pr->end; i >= pr->start;i--)
	{
		max = array[pr->start];
		k = pr->start;
		for(j = pr->start; j < i; j++)
		{
			if(array[j+1] > array[j])
			{
				max = array[j+1];
				k = j+1;
			}
		}
		array[k] = array[i];
		array[i] = max;

	}
	return result;
}

int main()
{
	int n = 10;

	pthread_t work_tid[2];
	//printf("111\n");
	params *param[2];
	for(int i = 0; i < 2; i++)
	{
		param[i] = malloc(sizeof(params));
		if(i == 0)
		{
			param[i]->start = 0;
			param[i]->end = n/2-1;
			//printf("%d:%d\n",param[i]->start,param[i]->end);
		}
		else
		{
			param[i]->start = n/2;
			param[i]->end = n-1;
			//printf("%d:%d\n",param[i]->start,param[i]->end);
		}	
		result_t argv = (void *)param[i];
		pthread_create(&(work_tid[i]), NULL, select_sort,argv);
		//printf("ttt\n");
       	}
	params *result[2];
	for(int i =0; i < 2; i++)
	{	
		pthread_join(work_tid[i],(void **)&result[i]);
	}
	int t_array[n];
	int count = 0;
	int l1 = result[0]->start;
	int r1 = result[0]->end;
	int l2 = result[1]->start;
	int r2 = result[1]->end;
	//printf("222\n");
	free(result[0]);
	free(result[1]);
	free(param[0]);
	free(param[1]);
	//printf("%d %d %d %d\n",l1, r1, l2, r2);
	while(count<= n-1)
	{
		while(l1 <= r1 && l2 <= r2)
		{
			if(array[l1] < array[l2])
			{
				t_array[count] = array[l1];
				count++;
				l1++;
				//printf("%d %d %d\n",l1, l2, count);
			}
			else
			{
				t_array[count] = array[l2];
				count++;
				l2++;
				//printf("%d %d %d\n",l1, l2, count);
			}
		}
		while(l1 <= r1)
		{
			t_array[count] = array[l1];
			count++;
			l1++;
			//printf("%d %d %d\n",l1, l2, count);
		}
		while(l2 <= r2)
		{
			t_array[count] = array[l2];
			count++;
			l2++;
			//printf("%d %d %d\n",l1, l2, count);
		}
	}
	printf("all %d\n",n);
	for(int m = 0; m < n; m++)
	{
		array[m] = t_array[m];
		printf("%d ",array[m]);
	}
}
