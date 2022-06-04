#include <stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<string.h>


void mysys(char *command)
{
    //实现该函数，该函数执行一条命令，并等待该命令执行结束
	if(command == NULL)
	{
		puts("command error1");
		exit(1);
	}
	char *argvt[10];
	char *token;	
	char s[2] = " ";
	int i = 0;
	//main（）样例中，直接将字符常量传入函数，会导致command是一个字符常量，需要调用strcpy（）
	char *copy_command = (char *)malloc(100);
	strcpy(copy_command, command);
	// puts(copy_command);
	token = strtok(copy_command, s);
	
	while( token != NULL ) 
	{
      		argvt[i] = token;
		// puts(token);
		i++;
      		token = strtok(NULL, s);
   	}
	
	argvt[i] = NULL;
	pid_t pid;
	pid = fork();
	
	if(pid == 0)
	{
		int error = execvp(argvt[0], argvt);
		// if(error < 0)
		//	puts("-------1");
		// else
		//	puts("33333333333333");
		//execlp("sh", "-c", command, NULL);
			//if(tag < 0)
			//	puts("command error");
		
		exit(1);
	}
	else if(pid > 0)
	{
		int status;
		wait(&status);
		if(WIFEXITED(status))
			status = -1;
			//exit(0);
		else
		{
			puts("command error2");
			//exit(1);
		}
	}
}

int main()
{
	printf("--------------------------------------------------\n");
	mysys("echo HELLO WORLD");
	printf("--------------------------------------------------\n");
	mysys("ls /");
	printf("--------------------------------------------------\n");
   	return 0;
}
