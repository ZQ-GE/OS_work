#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/stat.h>
#include<pthread.h>

#define WORKER_NUM	4

typedef struct {
	int is_end;
	char path[128];
	char string[128];
} Task;

#define MAX_TASK 1024

typedef struct{
	struct TaskNode* nxt;
	Task* task;
} TaskNode;

typedef struct {
	TaskNode* front;
	TaskNode* back;
	int count;
} TaskQueue;

void init_queue(TaskQueue* tq)
{
	tq->front = NULL;
	tq->back = NULL;
	tq->count = 0;
}

TaskNode* front(TaskQueue* tq)
{
	return tq->front;
}

void push(TaskQueue* tq, TaskNode* tn)
{
	if(!tq->front) {
		tq->front = tn;
		tq->back = tn;
	}
	else {
		tq->back->nxt = tn;
		tq->back = tn;
	}
	tq->count++;
}

void pop(TaskQueue* tq)
{
	if(!tq->count)return;
	tq->count--;
	TaskNode* tmp = tq->front;
	tq->front = tq->front->nxt;
	free(tmp->task);
	free(tmp);
}

typedef struct {
	int value;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
} sema_t;


void sema_init(sema_t* sema, int value)
{
	sema->value = value;
	pthread_mutex_init(&sema->mutex, NULL);
	pthread_cond_init(&sema->cond, NULL);
}

void sema_wait(sema_t* sema)
{
	pthread_mutex_lock(&sema->mutex);
	while (sema->value <= 0)
		pthread_cond_wait(&sema->cond, &sema->mutex);
	sema->value--;
	pthread_mutex_unlock(&sema->mutex);
}

void sema_signal(sema_t* sema)
{
	pthread_mutex_lock(&sema->mutex);
	++sema->value;
	pthread_cond_signal(&sema->cond);
	pthread_mutex_unlock(&sema->mutex);
}


sema_t mutex_sema;
sema_t empty_queue_sema;
sema_t full_queue_sema;


TaskQueue* tq;

TaskNode* make_TaskNode(char* path, char* string, int is_end)
{
	Task* task = (Task*)malloc(sizeof(Task));
	task->is_end = is_end;
	strcpy(task->path, path);
	strcpy(task->string, string);
	TaskNode* tn = (TaskNode*)malloc(sizeof(TaskNode));
	tn->nxt = NULL;
	tn->task = task;
	return tn;
}




void produce(char* path, char* string, int is_end)
{
	TaskNode* tn = make_TaskNode(path, string, is_end);

	sema_wait(&empty_queue_sema);
	sema_wait(&mutex_sema);

	push(tq, tn);
	
	sema_signal(&mutex_sema);
	sema_signal(&full_queue_sema);

}


void find_file(char* path, char* target)
{
	FILE* file = fopen(path, "r");
	if(!file)
	{
		return;
	}

	char line[256];
	while ( fgets(line, sizeof(line), file) ) {
		if( strstr(line, target) )
			printf("%s: %s", path, line);
	}

	fclose(file);
}

void find_dir(char* path, char* target)
{
	DIR* dir = opendir(path);
	struct dirent* entry;
	while ( entry = readdir(dir) ) {
		char cats[101] = {0};
		strcpy(cats, path);
		cats[strlen(path)] = '/';
		cats[strlen(path)+1] = '\0';
		strcat(cats, entry->d_name);

		if( strcmp(entry->d_name, ".") == 0)
			continue;

		if( strcmp(entry->d_name, "..") == 0)
			continue;

		if( entry->d_type == DT_DIR) {
			find_dir(cats, target);
		}
		
		if( entry->d_type == DT_REG) {
			produce(cats, target, 0);
		}
	}
	closedir(dir);
}


void sfind(char* path, char* string)
{
	struct stat info;
	stat(path, &info);
	
	if (S_ISDIR(info.st_mode) )
		find_dir(path, string);
	else 
		produce(path, string, 0);
}


void init()
{
	tq = (TaskQueue*)malloc(sizeof(TaskQueue));
	init_queue(tq);
	sema_init(&mutex_sema, 1);
	sema_init(&empty_queue_sema, MAX_TASK);
	sema_init(&full_queue_sema, 0);
}


int consume()
{
	sema_wait(&full_queue_sema);
	sema_wait(&mutex_sema);

	
	TaskNode* tn = front(tq);
	Task* task = tn->task;
	
	if(!task->is_end)
		find_file(task->path, task->string);

	sema_signal(&mutex_sema);

	if(task->is_end)
	{
		sema_signal(&full_queue_sema);
		return 1;
	}
	
	sema_signal(&empty_queue_sema);
	pop(tq);
	return 0;
}

void* worker_entry(void* arg)
{
	while(1) {
		if(consume())return NULL;
	}
}


int main(int argc, char* argv[])
{
	if(argc != 3) {
		puts("Usage: sfind file string");
		return 0;
	}

	char* path = argv[1];
	char* string = argv[2];

	init();
	pthread_t workers_tid[WORKER_NUM];
	
	for(int i=0; i<WORKER_NUM; i++)
	{
		pthread_create(workers_tid+i, NULL, worker_entry, NULL);
	}

	sfind(path, string);
	produce("end","end",1);

	for(int i=0; i<WORKER_NUM; i++)
	{
		pthread_join(workers_tid[i], NULL);
	}

	return 0;
}
