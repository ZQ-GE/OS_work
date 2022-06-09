#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include"parse.h"


extern void exec_back(tree_t *tree);
extern void exec_basic(tree_t *tree);
extern void exec_redirict(tree_t *tree);
extern void exec_pipe(tree_t *tree);
extern void execute(tree_t *tree);
extern char ** get_argv(tree_t *tree);
