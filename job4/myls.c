#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<dirent.h>
#include<string.h>

//struct dirent {
//               ino_t          d_ino;       /* Inode number */
//               off_t          d_off;       /* Not an offset; see below */
//               unsigned short d_reclen;    /* Length of this record */
//               unsigned char  d_type;      /* Type of file; not supported
//                                              by all filesystem types */
//               char           d_name[256]; /* Null-terminated filename */
//           };

	 /*   DT_BLK      This is a block device.

              DT_CHR      This is a character device.

              DT_DIR      This is a directory.

              DT_FIFO     This is a named pipe (FIFO).

              DT_LNK      This is a symbolic link.

              DT_REG      This is a regular file.

              DT_SOCK     This is a UNIX domain socket.

              DT_UNKNOWN  The file type could not be determined.
*/

int main(int argc, char *argv[])
{

	char path[256];
	if(argc == 1)
		//strcpy(path, ".");//因为没有设这为环境变量，只有在此目录下才能运行，所以不带参数的运行就是相当于 ls myls所在目录下的文件
		//若考虑到全局
		getcwd(path,256);
	else
		strcpy(path, argv[1]);

	DIR *dir = opendir(path);
	int length;
	while(1)
	{
		struct dirent *read = readdir(dir);
		if(read == NULL)
			break;
		else if(strcmp(read->d_name, ".") != 0 && strcmp(read->d_name, "..") != 0)
		{
			//if(read->d_type == DT_DIR || read->d_type == DT_RGB)
			//{	
				length = strlen(read->d_name);
				write(1,read->d_name,length);
				write(1,"\n",1);
			//}
		}
		else
			continue;
			
	}
}
