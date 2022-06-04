#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/stat.h>
#include<pthread.h>

#define WORKER_NUM  4
#define MAX 1024

typedef struct task{
	int is_end;
	char path[128];
	char string[128];
} Task;

typedef struct{
	Task T;
	//struct Tasknode *pre;
	struct Tasknode *next;
	int status;
}Tasknode;

typedef struct{
	Tasknode *head;
	Tasknode *pCurnode;
	Tasknode *tail;
	int length;
}TaskQueue;

///////////////////////
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
////////////////////

sema_t smutex;
sema_t empty_smutex;
sema_t full_smutex;

TaskQueue* tq;

Tasknode *init_tnode( char *path, char *string, int is_end)
{
	Tasknode *node = (Tasknode *)malloc(sizeof(Tasknode));
	//node->pre = NULL;
	node->next = NULL;
	node->status = 1;
	node->T.is_end = is_end;
	strcpy(node->T.path,path);
	strcpy(node->T.string,string);
	return node;
}

int init_q(TaskQueue *Q)
{
	Q->head = (Tasknode *)malloc(sizeof(Tasknode));
	//Q->head->pre = NULL;
	Q->head->next = NULL;
	Q->head->status = -1;
	Q->pCurnode = Q->head;
	Q->tail = NULL;
	Q->length = 0;

	return 0;
}

int add_q(TaskQueue *Q, Tasknode *node)
{
	sema_wait(&empty_smutex);
	sema_wait(&smutex);

	Q->pCurnode->next = node;
	//node->pre = Q->pCurnode;
	Q->pCurnode = node;
	Q->length += 1;
	Q->tail = Q->pCurnode;
	
	sema_signal(&full_smutex);
	sema_signal(&smutex);
	return 0;
}

int get_q(TaskQueue *Q, Tasknode *node)
{
	sema_wait(&full_smutex);
	sema_wait(&smutex);

	node = Q->head->next;
	Q->head->next = node->next;
	//node->next->pre = Q->head;
	Q->length = 1;
	if(Q->length == 0)
		Q->pCurnode = Q->head;
	Q->tail = Q->pCurnode;
	
	sema_signal(&empty_smutex);
	sema_signal(&smutex);
	return 1;
}

int j_empty(TaskQueue *Q)
{
	if(Q->length == 0)
		return 1;
	else
		return 0;
}

////

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
			Tasknode *node_new = init_tnode(path,target,0);
			add_q(tq,node_new);
		}
	}
	closedir(dir);
}



void* worker_entry(void* arg)
{
	while (1) {
		struct task task;
		//从任务队列中获取一个任务 task;
		Tasknode *node;
		get_q(tq,node);
		task.is_end = node->T.is_end;
		strcpy(task.path, node->T.path);
		strcpy(task.string, node->T.string);
		if (task.is_end)
			break;
		//执行该任务;
		find_file(task.path, task.string);
		free(node);
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

	struct stat info;
	stat(path, &info);
	if (!S_ISDIR(info.st_mode))
	{
		find_file(path, string);
		return 0;
	}
	init_q(tq);	
	sema_init(&smutex,1);
	sema_init(&empty_smutex, MAX);
	sema_init(&full_smutex, 0);
	/*1. 创建一个任务队列;
	- 初始化时，任务队列为空
	2. 创建 WORKER_NUMBER 个子线程;
	3. 对目录 path 进行递归遍历:
	- 遇见叶子节点时
	- 把叶子节点的路径加入到任务队列中
	4. 创建 WORER_NUMBER 个特殊任务
	- 特殊任务的 is_end 为真
	* 子线程读取到特殊任务时
	* 表示主线程已经完成递归遍历，不会再向任务队列中放置任务
	* 此时，子线程可以退出
	- 把这些特殊任务加入到任务队列中
	5. 等待所有的子线程结束*/
	pthread_t workers_tid[WORKER_NUM];
	
	for(int i=0; i<WORKER_NUM; i++)
	{
		pthread_create(workers_tid+i, NULL, worker_entry, NULL);
	}
	find_dir(path, string);
	for(int i = 0; i < WORKER_NUM;i++)
	{
		Tasknode * node_end = init_tnode("end","end",1);               
		add_q(tq,node_end);	
	}

	for(int i=0; i<WORKER_NUM; i++)
	{
		pthread_join(workers_tid[i], NULL);
	}

	return 0;
}
