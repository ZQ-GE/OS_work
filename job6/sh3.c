#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include"parse.h"
#include"exec.h"


int split(char *command, char *argvt[])
{
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
		//split command
		char **argvt = (char**)malloc(128*sizeof(char*));;
	    int n = split(command, argvt);
        tree_t *tree = tree_build(argvt, 0, n);
        print_t(tree, 0);
		/*int flag = build_in(command);
		if(flag == 0)
			mysys(command);*/
	}
}