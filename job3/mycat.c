#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdlib.h>

int main(int argc, char *argv[])
{
	/*int error;
	error = execvp("cat",argv);
	if(error < 0)
		printf("error\n");*/
	int fopen = open(argv[1],O_RDONLY);
	if( fopen < 0)
	{
		puts("can't open the file");
		exit(100);
	}
	// open file success	
	 int fread = 0;
	 char print[100];
	 int offset = 50;
	 fread = read(fopen, print, offset);
	 if(fread < 0)
	 {
	 	puts("can't read the file");
		exit(101);
	 }
	 // open file success
	 
	 while(fread != 0)
	{
		//puts(print);
		//print[fread-1] = '\0';
		//puts(print);
		write(1, print, fread);	
		//printf("%d\n",fopen);
		//printf("%d\n",fread);
		//lseek(fopen, 0, SEEK_CUR);
		fread = read(fopen, print, offset);
		//printf("%d\n",fopen);
		//printf("%d\n",fread);
	}

	return 0;

}
