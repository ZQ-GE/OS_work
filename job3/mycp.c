#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<stdlib.h>

int main(int argc, char *argv[])
{
	/*
	int error;
	error = execvp("cp",argv);
	if(error < 0)
		printf("error\n");
	*/
	//
	int fopen = open(argv[1],O_RDONLY);
	if( fopen < 0)
	{
		puts("can't open the file");
		exit(100);
	} 
	// open file success
	//
	mode_t mode = 0777;
	int fcreat = creat(argv[2], mode);
	if( fcreat < 0)
        {
                puts("can't creat the file");
                exit(100);
        } 
        // creat file success
        //
	int fread = 0;
	int fwrite = 0;
	char buf[100];
	int offset = 50;

	//printf("%d\n", fopen); 
	fread = read(fopen, buf, offset);
	if(fread < 0)
	 {
	 	puts("can't read the file");
		exit(101);
	 }
	//printf("%d\n", fread);
	while(fread != 0)
	{
		//puts(print);
		//print[fread-1] = '\0';
		//puts(print);
		//write(1, buf, fread);
		fwrite = write(fcreat, buf, fread);
       		if(fwrite < 0)
         	{
              		puts("can't write the file");
                	exit(102);
        	}
		//printf("%d\n",fopen);
		//printf("%d\n",fread);
		lseek(fcreat, 0, SEEK_CUR);
		lseek(fopen, 0, SEEK_CUR);
		fread = read(fopen, buf, offset);
		//printf("%d\n",fopen);
		//printf("%d\n",fread);
	}
	close(fread);
	close(fwrite);
	return 0;
}
