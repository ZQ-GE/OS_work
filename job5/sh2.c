#include <stdio.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<fcntl.h>



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

//split()
int split(char *command, char *argvt[])
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
	return i;
}

///////////////////////////////////////////////////////////////////
void mysys(char *command)
{
	char **argvt = (char**)malloc(128*sizeof(char*));;
	int n = split(command, argvt);
	char **argvt_new= (char**)malloc(128*sizeof(char*));
	char *filename1 = (char *)malloc(20*sizeof(char));
	char *filename2 = (char *)malloc(20*sizeof(char));
	char *filename3 = (char *)malloc(20*sizeof(char));
	int count = 0;
	int tag1, tag2, tag3;
	tag1 = tag2 = tag3 = 0;
        int i = 0;	
	for(i = 0; i <= n-1; i++)  //  split-new
	{
		if(argvt[i][0] == '<')
		{
			int length = strlen(argvt[i]);
			for(int j = 0; j < length-1; j++)
			{
				filename1[j] = argvt[i][j+1];
			}
			filename1[length-1] = '\0';
			tag1 = 1;
		}
		else if(argvt[i][0] == '>')
		{
			if(argvt[i][1] == '>')
			{
				int length = strlen(argvt[i]);
	                        for(int j = 0; j < length-1; j++)
        	                {
                	                filename3[j] = argvt[i][j+2];
                        	}
                        	filename3[length-2] = '\0';
                        	tag3 = 1;
			}
			else
			{
				int length = strlen(argvt[i]);
                        	for(int j = 0; j < length-1; j++)
                        	{
                                	filename2[j] = argvt[i][j+1];
                        	}
                        	filename2[length-1] = '\0';
				tag2 = 1;
			}
		}
		else
		{
			argvt_new[count] = argvt[i];
			count++;
		}
	}
	argvt_new[count] = NULL;
	pid_t pid;
	pid = fork();
	if(pid == 0)
	{
		
		int fd1, fd2;
		if(tag1)
		{
			/*puts("tag1");
			puts(filename1);*/
			fd1 = open(filename1, O_RDONLY, 0777);
			dup2(fd1, 0);
			close(fd1);
		}
		if(tag2)
		{
			/*puts("tag2");
			puts(filename2);*/
			fd2 = open(filename2,O_CREAT|O_RDWR|O_TRUNC, 0777);
			dup2(fd2, 1);
			close(fd2);
		}
		else if(tag3)
		{
			/*puts("tag3");
			puts(filename3);*/
			fd2 = open(filename3,O_RDWR|O_APPEND, 0777);
			dup2(fd2, 1);
			close(fd2);
		}	
		
		int error = execvp(argvt_new[0], argvt_new);
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
