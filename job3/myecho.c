#include<stdio.h>
#include<unistd.h>


int main(int argc, char *argv[])
{
	int error;
	/*printf("%d\n",argc);
	for(int i = 0; i <= argc; i++)
	{
		if(argv[i] == NULL)
			printf("NULL\n");
		else 
			printf("%s, %d\n",argv[i],i);
	}*/
	
	error = execvp("echo", argv);
	if(error < 0)
		printf("error\n");
	return 0;
}
