#include <stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<string.h>



int build_in(char *command) // building in
{
	// exit	
	if(strcmp(command, "exit") == 0)
	{
		//puts("exit success");
		exit(2);
	}

	// pwd
	// exec can also do it, but may have some problems
	if(strcmp(command, "pwd") == 0)
        {
                //puts("pwd success");
		char buffer[100];
 		getcwd(buffer,100);
		puts(buffer);
		return 1;
        }

	// cd
	if(command[0] == 'c' && command[1] == 'd' && command[2] == ' ')
	{
		char *path = command + 3;
		//puts("cd success");
		//puts(path);
		chdir(path);
		return  1;
	}	
	return 0;
}
void split(char *command, char *argvt[])
{
	//实现该函数，该函数执行一条命令，并等待该命令执行结束
        if(command == NULL)
        {
                puts("command error1");
                exit(1);
        }
        //存储分词后的字符串数组
        //char *argvt[10];
        char *token;
        char s[2] = " ";
        int i = 0;
        // char *copy_command = (char *)malloc(100);
        // strcpy(copy_command, command);
        
        //从标准输入读入的command不是字符常量，不需要再进行strcpy（）
        //
        // puts(copy_command);
        
        //// split
        token = strtok(command, s);
        while( token != NULL ) 
        {
                argvt[i]= token;
                // puts(token);
                i++;
                token = strtok(NULL, s);
        }
        
        argvt[i] = NULL;
}
///////////////////////////////////////////////////////////////////
void mysys(char *command)
{
    //实现该函数，该函数执行一条命令，并等待该命令执行结束
	/*if(command == NULL)
	{
		puts("command error1");
		exit(1);
	}
	//存储分词后的字符串数组
	char *argvt[10];
	char *token;
	char s[2] = " ";
	int i = 0;
	// char *copy_command = (char *)malloc(100);
	// strcpy(copy_command, command);
	
	//从标准输入读入的command不是字符常量，不需要再进行strcpy（）
	//
	// puts(copy_command);
	
	//// split
	token = strtok(command, s);
	while( token != NULL ) 
	{
      		argvt[i] = token;
		// puts(token);
		i++;
      		token = strtok(NULL, s);
   	}
	
	argvt[i] = NULL;
	*/
	char **argvt;
	split(command, argvt);
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
	int tag = 1;
	char buf[2]="> ";
	char command[50];

	while(tag)
	{
		write(1,buf,2);
		//gets(command);
		fgets(command, 50, stdin);
		char *find;
		find = strchr(command, '\n');          //查找换行符
		if(find)                            //如果find不为空指针
    			*find = '\0';
		//puts(command);
		//
		int flag = build_in(command);
		if(flag == 0)
			mysys(command);

	}
}
