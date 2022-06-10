#include"exec.h"

int get_argv(tree_t *tree, char *argvt[])
{
    int i = 0;
    //char **argvt = malloc(128*sizeof(char *));
    while( i < tree->child_count)
    {
        /*char * tmp = malloc(15*sizeof(char));
        strcpy(tmp, tree->child_vector[i]->token);*/
        //puts(tree->child_vector[i]->token);
        argvt[i]= tree->child_vector[i]->token;
        //puts(argvt[i]);
        i++;
    }
    argvt[i] = NULL;
    return i;
}

int exec_build_in(tree_t *tree)
{
    if(strcmp(tree->child_vector[0]->token, "exit") == 0)
	{
		//puts("exit success");
		exit(2);
	}
	// pwd
	if(strcmp(tree->child_vector[0]->token, "pwd") == 0)
        {
                //puts("pwd success");
		char buffer[100];
 		getcwd(buffer,100);
		puts(buffer);
		return 1;
        }
	// cd
	if(strcmp(tree->child_vector[0]->token,"cd") == 0)
	{
		chdir(tree->child_vector[1]->token);
		return  1;
	}	
    return 0;
}

// back command
void exec_back(tree_t *tree)
{
    exec_wrapper(tree->child_vector[0]);
}

void exec_basic(tree_t *tree)
{
    char **argv = (char **)malloc(128*sizeof(char *));
    get_argv(tree, argv);
    execvp(argv[0], argv);
}

void exec_redirict(tree_t *tree)
{
    int fd;
    //puts(tree->child_vector[1]->token);
    if(tree->child_vector[1]->token[0] == '<')
    {
        fd = open(tree->child_vector[2]->token, O_RDONLY, 0777);
        dup2(fd,0);
        close(fd);
    }
    else if(strlen(tree->child_vector[1]->token) == 1)
    {
        fd = open(tree->child_vector[2]->token,O_CREAT|O_RDWR|O_TRUNC, 0777);
        dup2(fd,1);
        close(fd);
    }
    else
    {
        fd = open(tree->child_vector[2]->token,O_RDWR|O_APPEND, 0777);
        dup2(fd,1);
        close(fd);
    }
    execute(tree->child_vector[0]);
}

void exec_pipe(tree_t *tree)
{
    int fd[2];
    pipe(fd);
    pid_t pid;
    pid = fork();
    if(pid == 0)
    {
        dup2(fd[1],1);
        close(fd[1]);
        close(fd[0]);
        execute(tree->child_vector[0]);
        exit(1);
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

void exec_wrapper(tree_t *tree)
{
    int tag = -1;
    if(tree->type == TREE_BASIC)
        tag = exec_build_in(tree);
    if(tag == 1)
        return;
    pid_t pid;
    pid = fork();
    if(pid <= 0)
    {
        execute(tree);
        exit(1);

    }   
    else
    {
        int status;
        if(tree->type == TREE_BACK)
            return;
        else    
            wait(&status);
    } 
}


