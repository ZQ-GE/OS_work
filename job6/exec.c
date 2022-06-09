#include"exec.h"

char ** get_argv(tree_t *tree)
{
    int i;
    char **argvt = malloc(128*sizeof(char *));
    for( i = 0; i < tree->child_count; i++)
            strcpy(argvt[i], tree->child_vector[i]->token);
    argvt[i] = NULL;
    return argvt;
}

// back command
void exec_back(tree_t *tree)
{
    pid_t pid;
    pid = fork();
    if(pid <= 0)
        execute(tree->child_vector[0]);
    else
        return NULL;
}

void exec_basic(tree_t *tree)
{
    char **argv = get_argv(tree);
    // build in command exit, pwd, cd
    // exit
    if(strcmp(argv[0], "exit") == 0)
	{
		//puts("exit success");
		exit(2);
	}
	// pwd
	if(strcmp(argv[0], "pwd") == 0)
        {
                //puts("pwd success");
		char buffer[100];
 		getcwd(buffer,100);
		puts(buffer);
		return 1;
        }
	// cd
	if(strcmp(argv[0],"cd"))
	{
		chdir(argv[1]);
		return  1;
	}	
    pid_t pid;
    pid = fork();
    if(pid <= 0)
    {
        execvp(argv[0], argv);
        exit(1);
    }
    else
    {
        int status;
        wait(&status);
    }
}

void exec_redirict(tree_t *tree)
{
    pid_t pid;
    pid = fork();
    if(pid <= 0)
    {
        int fd;
        if(tree->child_vector[1]->token[0] == '<')
        {
            fd = open(tree->child_vector[2]->token, O_RDONLY, 0777);
            dup2(fd,0);
        }
        else if(tree->child_vector[1]->token[0] == '>')
        {
            fd = open(tree->child_vector[2]->token,O_CREAT|O_RDWR|O_TRUNC, 0777);
            dup2(fd,1);
        }
        else
        {
            fd = open(tree->child_vector[2]->token,O_RDWR|O_APPEND, 0777);
            dup2(fd,1);
        }
        execute(tree->child_vector[0]);
    }
    else
    {
        int status;
        wait(&status);
    }
}

void exec_pipe(tree_t *tree)
{
    int fd[2];
    pipe(fd);
    pid_t pid;
    pid = fork();
    if(pid <= 0)
    {
        dup2(fd[1],1);
        close(fd[1]);
        close(fd[0]);
        execute(tree->child_vector[0]);
    }
    int status;
    wait(&status);
    dup2(fd[0],0);
    close(fd[0]);
    close(fd[1]);
    execute(tree->child_vector[1]);
}

void execute(tree_t *tree)
{
    switch (tree->type)
    {
    case TREE_BACK:
        exec_back(tree);
        break;
    case TREE_BASIC:
        exec_basic(tree);
        break;
    case TREE_PIPE:
        exec_pipe(tree);
        break;
    case TREE_REDIRICT:
        exec_redirict(tree);
        break;
    default:
        break;
    }
}