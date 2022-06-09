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
    char *token;
    int child_count;
    struct tree_tl *child_vector[MAX_CHILD];
} tree_t;


extern tree_t *tree_build(char **argvt, int start, int n);
extern void print_t(tree_t * tree, int level);