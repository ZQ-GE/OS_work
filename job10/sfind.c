#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/stat.h>


void find_file(char* path, char* target)
{
	FILE* file = fopen(path, "r");

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
			find_file(cats, target );
		}
	}
	closedir(dir);
}


int main(int argc, char* argv[])
{
	
	if(argc != 3 ) { //在vscode调试时进行了更改，在linux下，改为3
		puts("Usage: sfind file string");
		return 0;
	}

	char* path = argv[1]; //改为1
	char* string = argv[2];//改为2
	
	        struct stat info;
        stat(path, &info);

        if (S_ISDIR(info.st_mode) )
                find_dir(path, string);
        else
                find_file(path, string);

	return 0;
}
