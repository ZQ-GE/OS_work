#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include"parse.h"

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

