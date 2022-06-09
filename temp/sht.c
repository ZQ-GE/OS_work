#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>


enum {
    TREE_BACK,
    TREE_PIPE,
    TREE_REDIRICT,
    TREE_BASIC,
    TREE_TOKEN,
};

#define MAX_CHILD 10
typedef struct tree_tl{
    int type;
    char token[20];
    int child_count;
    struct tree_tl *child_vector[MAX_CHILD];
} tree_t;

void exec_back(tree_t *tree);
int  exec_basic(tree_t *tree);
void exec_redirict(tree_t *tree);
void exec_pipe(tree_t *tree);
void execute(tree_t *tree);
void exec_wrapper(tree_t *tree);


/*void tree_init(tree_t *tree)
{
    tree->token = (char *)malloc(sizeof(char));
    //tree->child_vector = (tree_t *)malloc(sizeof(tree_t));
}*/

tree_t *tree_build(char **argvt, int start, int n)
{
    int i = start;
    int tag_back = 0;
    int tag_redirict_in = 0;
    int tag_redirict_out = 0;
    int tag_pipe = 0;

    // & command
    tree_t *node_back = (tree_t *)malloc(sizeof(tree_t));
    if(argvt[n-1][0] == '&')
    {
        node_back->type = TREE_BACK;
        n = n-1;
        node_back->child_count = 1;
        tag_back = 1;
    }
    tree_t *node_basic = (tree_t *)malloc(sizeof(tree_t));
    tree_t *node_redirict_in = (tree_t *)malloc(sizeof(tree_t));
    tree_t *node_redirict_out = (tree_t *)malloc(sizeof(tree_t));
    tree_t *node_pipe = (tree_t *)malloc(sizeof(tree_t));
    /*tree_init(node_back);
    tree_init(node_basic);
    tree_init(node_pipe);
    tree_init(node_redirict_in);
    tree_init(node_redirict_out);*/
    
    node_basic->type = TREE_BASIC;
    node_basic->child_count = 0;
    node_redirict_in->type = TREE_REDIRICT;
    node_redirict_in->child_count = 3;
    node_redirict_out->type = TREE_REDIRICT;
    node_redirict_out->child_count = 3;
    node_pipe->type = TREE_PIPE;
    node_pipe->child_count = 2;
    for(i = start; i < n; i++)
    {
        // rediritct <
        if(argvt[i][0] == '<')
        {
            tag_redirict_in = 1;
            tree_t *node_token_sign = (tree_t *)malloc(sizeof(tree_t));
            tree_t *node_token_file = (tree_t *)malloc(sizeof(tree_t));
            node_token_sign->type = TREE_TOKEN;
            strcpy(node_token_sign->token, "<");
            node_token_sign->child_count = 0;
            node_token_file->type = TREE_TOKEN;
            strcpy(node_token_file->token ,&argvt[i][1]);
            node_token_file->child_count = 0;
            node_redirict_in->child_vector[0] = node_basic;
            node_redirict_in->child_vector[1] = node_token_sign;
            node_redirict_in->child_vector[2] = node_token_file;
        }
        // redirict >,>>
        else if(argvt[i][0] == '>')
        {
            if(argvt[i][1] == '>')
            {
                tag_redirict_out = 1;
                tree_t *node_token_sign = (tree_t *)malloc(sizeof(tree_t));
                tree_t *node_token_file = (tree_t *)malloc(sizeof(tree_t));
                node_token_sign->type = TREE_TOKEN;
                strcpy(node_token_sign->token,">>");
                node_token_sign->child_count = 0;
                node_token_file->type = TREE_TOKEN;
                strcpy(node_token_file->token ,&argvt[i][2]);
                node_token_file->child_count = 0;
                node_redirict_out->child_vector[0] = node_basic;
                node_redirict_out->child_vector[1] = node_token_sign;
                node_redirict_out->child_vector[2] = node_token_file;
            }
            else
            {            
                tag_redirict_out = 1;
                tree_t *node_token_sign = (tree_t *)malloc(sizeof(tree_t));
                tree_t *node_token_file = (tree_t *)malloc(sizeof(tree_t));
                node_token_sign->type = TREE_TOKEN;
                strcpy(node_token_sign->token,">");
                node_token_sign->child_count = 0;
                node_token_file->type = TREE_TOKEN;
                strcpy(node_token_file->token ,&argvt[i][1]);
                node_token_file->child_count = 0;
                if(tag_redirict_in)
                    node_redirict_out->child_vector[0] = node_redirict_in;
                else
                    node_redirict_out->child_vector[0] = node_basic;
                node_redirict_out->child_vector[1] = node_token_sign;
                node_redirict_out->child_vector[2] = node_token_file;
            }
        }
        // pipe command |
        else if(argvt[i][0] == '|')
        {
            tag_pipe = 1;
            tree_t *node_pipe_right = tree_build(argvt, i+1, n);
            node_pipe->child_vector[1] = node_pipe_right;
            break;
        }
        else
        {
            tree_t *node_token = (tree_t *)malloc(sizeof(tree_t));
            node_token->type = TREE_TOKEN;
            node_token->child_count = 0;
            strcpy(node_token->token,argvt[i]);
            node_basic->child_vector[node_basic->child_count] = node_token;
            node_basic->child_count += 1;
        }
    }
    tree_t *temp;
    temp = node_basic;
    if(tag_redirict_out)
        temp = node_redirict_out;
    else if(tag_redirict_in)
        temp = node_redirict_in;
    
    if(tag_pipe)
    {
        node_pipe->child_vector[0] = temp;
        temp = node_pipe;
    }

    if(tag_back)
    {
        node_back->child_vector[0] = temp;
        temp = node_back;
    }
    // free
    if(!tag_back)
        free(node_back);
    if(!tag_redirict_in)
        free(node_redirict_in);
    if(!tag_redirict_out)
        free(node_redirict_out);
    if(!tag_pipe)
        free(node_pipe);
    return temp;
}

// print tree
void print_t(tree_t * tree, int level)
{
    char tab[level*4];
	memset(tab,' ', level*4);
    write(1, tab, level*4);
    switch (tree->type)
    {
    case TREE_BACK:
        write(1,"back\n",5);
        break;
    case TREE_BASIC:
        write(1,"basic\n",6);
        break;
    case TREE_REDIRICT:
        write(1,"redirict\n",9);
        break;
    case TREE_PIPE:
        write(1,"pipe\n", 5);
        break;
    case TREE_TOKEN:
        write(1,tree->token,strlen(tree->token));
        write(1,"\n",1);
        break;    
    default:
        break;
    }
    for(int i = 0; i < tree->child_count; i++)
    {
        print_t(tree->child_vector[i], level+1);
    }
}

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

int exec_basic(tree_t *tree)
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

void test()
{
    char command[50] = "cat <a | wc -l | grep int &";
    while(1)
	{
		write(1,"> ",2);
		//gets(command);
		char *find;
		find = strchr(command, '\n');          //查找换行符
		if(find)                            //如果find不为空指针
    			*find = '\0';
		//split command
		char **argvt = (char**)malloc(128*sizeof(char*));;
	    int n = split(command, argvt);
        tree_t *tree = tree_build(argvt, 0, n);
        print_t(tree, 0);
        break;
    }
}

int main()
{
	int tag = 1;
	char buf[2]="> ";
	char command[50];
    /*if(1)
        test();*/
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
		char **argvt = (char**)malloc(128*sizeof(char*));
	    int n = split(command, argvt);
        tree_t *tree = tree_build(argvt, 0, n);
        print_t(tree, 0);
        
        exec_wrapper(tree);

	}
}