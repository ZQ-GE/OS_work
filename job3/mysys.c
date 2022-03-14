#include <stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>


void mysys(char *command)
{
    //实现该函数，该函数执行一条命令，并等待该命令执行结束
	if(command == NULL)
	{
		puts("command error");
		exit(1);
	}
	pid_t pid;
	pid = fork();
	if(pid == 0)
	{
		execlp("sh", "sh", "-c", command, NULL);
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
			puts("command error");
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
