#include "recursive_descent.h"

static node_t *GetG(calculation_tree *calculation_tree, char **expression, Calculation_Tree_Errors *err);
static node_t *GetE(calculation_tree *calculation_tree, char **expression, Calculation_Tree_Errors *err);
static node_t *GetT(calculation_tree *calculation_tree, char **expression, Calculation_Tree_Errors *err);
static node_t *GetP(calculation_tree *calculation_tree, char **expression, Calculation_Tree_Errors *err);
static node_t *GetN(calculation_tree *calculation_tree, char **expression);
static node_t *GetF(calculation_tree *tree, char **expression, Calculation_Tree_Errors *err);
static node_t *GetV(calculation_tree *tree, char *var);

node_t *GetG(calculation_tree *tree, char **expression, Calculation_Tree_Errors *err)
{
    printf("case G\n");
    node_t *val = GetE(tree, expression, err);
    if (**expression != '$') 
        *err = ERROR_DURING_READ_FILE;
    printf("G\n");
    (*expression)++;
    return val;
}
// token **tokens - structs buffer
node_t *GetE(calculation_tree *tree, char **expression, Calculation_Tree_Errors *err)
{
    node_t *val = GetT(tree, expression, err);
    while (**expression == '+' || **expression == '-')
    {
        int op = **expression;
        (*expression)++;
        node_t *val2 = GetT(tree, expression, err);
        if (op == '+')
            val = NewNodeOpInit(tree, ADD, val, val2);
        else    
            val = NewNodeOpInit(tree, SUB, val, val2);
    }

    return val;
}

node_t *GetT(calculation_tree *tree, char **expression, Calculation_Tree_Errors *err)
{
    node_t *val = GetP(tree, expression, err);

    printf("*expression_in_T = %c\n", **expression);
    while (**expression == '*' || **expression == '/')
    {
        char op = **expression;
        (*expression)++;
        node_t *val2 = GetP(tree, expression, err);
        if (op == '*')
            val = NewNodeOpInit(tree, MUL, val, val2);
        else    
            val = NewNodeOpInit(tree, DIV, val, val2);
    }

    return val;
}

node_t *GetP(calculation_tree *tree, char **expression, Calculation_Tree_Errors *err)
{
    if (**expression == '(')
    {
        (*expression)++;
        node_t *val = GetE(tree, expression, err);
        if (**expression != ')') 
            *err = ERROR_DURING_READ_FILE;
        else (*expression)++;
        return val;
    }

    if ('0' <= **expression && **expression <= '9')
    { 
        return GetN(tree, expression);
    }

    else 
        return GetF(tree, expression, err);
}

node_t *GetN(calculation_tree *tree, char **expression)
{
    double val = 0;
    while ('0' <= **expression && **expression <= '9')
    {
        val = val * 10 + (double)(**expression - '0');
        printf("val_intermediate = %lf\n", val);
        (*expression)++;
    }

    printf("val = %lf\n", val);
    printf("*expression = %c\n", **expression);

    return NewNodeNumInit(tree, val, NULL, NULL);
}

node_t *GetF(calculation_tree *tree, char **expression, Calculation_Tree_Errors *err)
{
    size_t n = 0;
    bool was_shift = false;
    bool is_operation = false;
    char *expression_start = *expression;
    while (('a' <= **expression && **expression <= 'z') || ('A' <= **expression && **expression <= 'Z') || **expression == '_' || ('0' <= **expression && **expression <= '9'))
    {
        (*expression)++;
        n++;
        was_shift = true;
    }
    
    char *function = (char *)calloc(n + 1, sizeof(char));
    strncpy(function, expression_start, n);
    function[n] = '\0';

    if (!was_shift) *err = ERROR_DURING_READ_FILE;

    int i = 6;
    for (;i < NUM_OF_OPERATIONS; i++)
    {
        if (!strcmp(function, operations_buffer[i]))
        {
            is_operation = true;
            break;
        }
    }

    node_t *val = NULL;
    if (is_operation)
    {
        if (**expression == '(')
        {
            (*expression)++;
            val = GetE(tree, expression, err);
            if (**expression != ')') 
            {
                printf("Compilation error: expected ')'" "\n");
                *err = ERROR_DURING_READ_FILE;
            }
            else (*expression)++;
        }
    }

    if (is_operation && val != NULL)
        return NewNodeOpInit(tree, (operation_t)i, NULL, val);

    else    
        return GetV(tree, function);
}

node_t *GetV(calculation_tree *tree, char *var)
{
    assert(var != NULL);

    return NewNodeVarInit(tree, var, NULL, NULL);
}

Calculation_Tree_Errors MakeTreeFromExpression(calculation_tree *tree, const char *logfile_name, const char *name_of_file_with_expression)
{
    Calculation_Tree_Errors err = NO_ERROR;
    
    tree->num_of_el = 0;
    tree->variables = (variable *)calloc((size_t)NUM_OF_VARIABLES, sizeof(variable));
    tree->variables_size = 0;
    tree->variables_capacity = NUM_OF_VARIABLES;
    tree->file_name = logfile_name;
    
    char *expression = ReadExpressionFromFile(name_of_file_with_expression, &err);
    char *expression_beginning = expression;

    printf("expression = %s\n", expression);

    if (err)
    {
        free(expression);
        return err;
    }

    tree->root = BuildingATree(tree, &expression, &err);

    free(expression_beginning);
    
    if (err)
    {
        return err;
    }

    if (tree->root != NULL)
        tree->root->parent = NULL;

    if ((err = CalculationTreeVerify(tree)))
    {
        return err;
    }

    return err;
}

node_t *BuildingATree(calculation_tree *tree, char **expression, Calculation_Tree_Errors *err)
{
    return GetG(tree, expression, err);
}

char *ReadExpressionFromFile(const char *name_of_file, Calculation_Tree_Errors *err)
{
    FILE *file_to_read = fopen(name_of_file, "r");
    if (!OpenFileSuccess(file_to_read, name_of_file))
    {
        *err = ERROR_DURING_OPENING_FILE;
        return NULL;
    }

    fseek(file_to_read, SEEK_SET, 0);
    size_t num_of_bytes_in_file = return_num_of_bytes_in_file(fileno(file_to_read));

    char *expression = (char *)calloc(num_of_bytes_in_file + 2, sizeof(char));
    if (expression == NULL)
    {
        *err = ERROR_DURING_MEMORY_ALLOCATION;
        return NULL;
    }

    size_t num_success_read_symbols = fread(expression, sizeof(char), num_of_bytes_in_file, file_to_read);
    if (num_success_read_symbols < num_of_bytes_in_file)
    {
        free(expression);
        printf("123\n");
        *err = ERROR_DURING_READ_FILE;
        return NULL;
    }

    expression[num_success_read_symbols] = '$';
    expression[num_success_read_symbols + 1] = '\0';
   
    if (!CloseFileSuccess(file_to_read, name_of_file))
    {
        free(expression);
        *err = ERROR_DURING_CLOSING_FILE;
        return NULL;
    }

    return expression;
}

size_t return_num_of_bytes_in_file(int fd1)
{
    struct stat st1 = {};
    fstat(fd1, &st1);

    return (size_t)st1.st_size;
}

node_t *NewNodeNumInit(calculation_tree *tree, double number, node_t *node_left, node_t *node_right)
{
    node_t *node = InitNewNode(tree, node_left, node_right);

    node->type = NUM_TYPE;
    node->value.number = number;

    return node;
}

node_t *NewNodeVarInit(calculation_tree *tree, char *var, node_t *node_left, node_t *node_right)
{
    node_t *node = InitNewNode(tree, node_left, node_right);

    variable var_str = {NAN, var, false};
    tree->variables[tree->variables_size] = var_str;
    tree->variables_size++;

    if (tree->variables_size >= tree->variables_capacity - 1)
    {
        tree->variables = (variable *)calloc((size_t)tree->variables_capacity*2, sizeof(variable));
        tree->variables_capacity *= 2;
    }

    node->type = VAR_TYPE;
    node->value.var = var_str;

    return node;
}

node_t *NewNodeOpInit(calculation_tree *tree, operation_t operation, node_t *node_left, node_t *node_right)
{
    node_t *node = InitNewNode(tree, node_left, node_right);

    node->type = OP_TYPE;
    node->value.operation = operation;

    return node;
}

node_t *InitNewNode(calculation_tree *tree, node_t *node_left, node_t *node_right)
{
    node_t *node = (node_t *)calloc(1, sizeof(node_t));

    if (node == NULL)
    {
        return NULL;
    }

    tree->num_of_el++;
    printf("sfw\n");

    node->left = node_left;
    node->right = node_right;

    if (node_left != NULL)
    {
        node->left->parent = node;
    }

    if (node_right != NULL)
    {
        node->right->parent = node;
    }

    return node;
}

node_t *SubtreeInit(calculation_tree *tree, node_t *tree_node)
{
    if (tree_node == NULL)
        return NULL;

    node_t *node = (node_t *)calloc(1, sizeof(node_t));

    if (node == NULL)
    {
        return NULL;
    }

    tree->num_of_el++;

    node->type = tree_node->type;
    node->value = tree_node->value;

    if (tree_node->left != NULL)
    {
        node->left = SubtreeInit(tree, tree_node->left);
        node->left->parent = node;
    }
    else 
    {
        node->left = NULL;
    }

    if (tree_node->right != NULL)
    {
        node->right = SubtreeInit(tree, tree_node->right);
        node->right->parent = node;
    }
    else
    {
        node->right = NULL;
    }

    return node;
}

Calculation_Tree_Errors CalculationTreeVerify(calculation_tree *tree)
{
    if (tree == NULL) return NULL_POINTER_ON_TREE;
    else if (tree->num_of_el < 0) return NEGATIVE_NUM_OF_ELEMENTS;
    else if (tree->root == NULL) return NULL_POINTER_ON_ROOT; 
    else if (tree->root->parent != NULL) return ROOT_HAVE_PARENT;  
    
    node_t *parent = NULL;
    node_t *son = NULL;

    if (!CheckSonsAndParents(tree->root, &son, &parent))
    {
        printf("son %p has a parent %p, but not %p", son, son->parent, parent);
        return SON_HAS_WRONG_PARENT;
    } 

    return NO_ERROR;
}

bool CheckSonsAndParents(node_t *node, node_t **son, node_t **parent)
{
    bool flag = true;
    CheckSonsAndParentsRecursive(node, &flag, son, parent);

    return flag;
}

void CheckSonsAndParentsRecursive(node_t *node, bool *flag, node_t **son, node_t **parent)
{
    if (node == NULL)
        return;

    CheckSonsAndParentsRecursive(node->left, flag, son, parent);

    if (node->left != NULL) 
    {
        if (node->left->parent != node) 
        {
            *flag = false;
            *son = node->left->parent;
            *parent = node;
        }
    }
    if (node->right != NULL) 
    {
        if (node->right->parent != node) 
        {
            *flag = false;
            *son = node->right->parent;
            *parent = node;
        }
    }

    CheckSonsAndParentsRecursive(node->right, flag, son, parent);
    
    return;
}

Calculation_Tree_Errors CalculationTreeDestroy(calculation_tree *tree)
{
    ASSERTS(tree);

    Calculation_Tree_Errors err = NO_ERROR;
    err = CalculationTreeDestroyRecursive(tree, &(tree->root));

    for (ssize_t i = 0; i < tree->variables_size; i++)
    {
        free(tree->variables[i].name);
    }
    free(tree->variables);

    return err;
}

Calculation_Tree_Errors CalculationTreeDestroyRecursive(calculation_tree *tree, node_t **node)
{
    assert(tree != NULL);

    Calculation_Tree_Errors err = NO_ERROR;

    #ifdef ENABLE_THE_VERIFIER
    if ((err = CalculationTreeVerify(tree)))
    {
        return err;
    }
    #endif

    if (*node == NULL)
    {
        return NO_ERROR;
    }
    CalculationTreeDestroyRecursive(tree, &((*node)->left));
    CalculationTreeDestroyRecursive(tree, &((*node)->right));

    // if ((*node)->type == VAR_TYPE)//тут надо понять встречается ли в дереве эта переменная ещё или нет, если да, то буфер не надо очищать, если нет, то нужно как то указать, что этой переменной в дереве нет 
    // {
    //     free((*node)->value.var.name);
    // }

    free(*node);
    tree->num_of_el--;
    *node = NULL;

    return err;
}

void CalculationTreeDump(calculation_tree *tree, const char *file, int line)
{
    ASSERTS(tree);

    ssize_t rank = 0;
    DumpToConsole(tree, file, line, &rank);

    time_t rawtime;      
    struct tm *timeinfo; 
    char buffer[80];

    // Get the current calendar time
    time(&rawtime);

    // Convert the calendar time to local time
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%Y_%m_%d_%H_%M_%S", timeinfo);

    // Print the formatted time string
    struct timespec tstart={0,0};
    clock_gettime(CLOCK_MONOTONIC, &tstart);

    char gvfile_name[SIZE_OF_NAME] = {0};
    sprintf(gvfile_name, "%s%s%s%ld.gv", link_to_graphviz_file, tree->file_name, buffer, tstart.tv_nsec);

    DumpToLogfile(tree, tree->file_name, gvfile_name, &rank);

    char texfile_name[SIZE_OF_NAME] = {0};
    sprintf(texfile_name, "%s.tex", tree->file_name);

    DumpToTexFile(tree, texfile_name);

    CreateGraph(tree, gvfile_name);
}

void DumpToConsole(const calculation_tree *tree, const char *file, int line, ssize_t *rank)
{
    printf("ListDump called from %s :%d\n", file, line);
    Dump(stdout, tree, rank);
}

void DumpToTexFile(const calculation_tree *tree, const char *texfile_name)
{
    FILE *tex = fopen(texfile_name, "a");

    if (!OpenFileSuccess(tex, texfile_name))
    {
        return;
    }

    char *expression = NULL;

    fprintf(tex, "\\documentclass{article}\n");
    fprintf(tex, "\\begin{document}\n");
    
    expression = DumpToTexFileRecursive(tree->root, tex);
    printf("expression = %s\n", expression);
    free(expression);

    fprintf(tex, "\\end{document}\n\n\n\n");

    char *command = (char *)calloc(strlen(texfile_name) + strlen("pdflatex -quiet ") + 1, sizeof(char));
    
    // Компилируем в PDF
    snprintf(command, strlen(texfile_name) + strlen("pdflatex ") + 1, "pdflatex %s", texfile_name);
    system(command);

    free(command);

    if (!CloseFileSuccess(tex, texfile_name))
    {
        return;
    }
}

char *DumpToTexFileRecursive(node_t *node, FILE *tex)
{
    if (node == NULL) 
        return NULL;

    char *s1 = NULL, *s2 = NULL, *s3 = NULL;
    
    s1 = DumpToTexFileRecursive(node->left, tex);
    if (s1 != NULL) printf("s1_tex = %s\n", s1);
    s2 = DumpToTexFileRecursive(node->right, tex);
    if (s2 != NULL) printf("s2_tex = %s\n", s2);

    if (s1 == NULL && s2 == NULL)
    {
        if (node->type == NUM_TYPE)
        {
            printf("number = %lf\n", node->value.number);

            char number[100] = {0};
            sprintf(number, "%lf", node->value.number);
            s3 = (char *)calloc(strlen(number) + 1, sizeof(char));
            sprintf(s3, "%lf", node->value.number);
            return s3;
        }

        else if (node->type == VAR_TYPE) 
        {
            printf("variable = %s\n", node->value.var.name);

            s3 = (char *)calloc(strlen(node->value.var.name) + 1, sizeof(char));
            sprintf(s3, "%s", node->value.var.name);
            return s3;
        }   
    }

    else
    {
        printf("operation = %s\n", operations_buffer[node->value.operation]);

        if      (s1 == NULL)
            s3 = (char *)calloc(strlen(s2) + 12, sizeof(char));
        else if (s2 == NULL)
            s3 = (char *)calloc(strlen(s1) + 12, sizeof(char));
        else
            s3 = (char *)calloc(strlen(s1) + strlen(s2) + 12, sizeof(char));

        switch (node->value.operation)
        {
            case ADD:
            case SUB: 
                sprintf(s3, "(%s %s %s)", s1, operations_buffer[node->value.operation], s2);
                printf("s3 = %s\n", s3);
                fprintf(tex, "\\[\n\\ %.*s\n\\]\n", (int)strlen(s3) - 2, s3 + 1);
                break;

            case MUL:
                if (node->parent != NULL && (node->parent->value.operation == ADD || node->parent->value.operation == SUB || node->parent->value.operation == DIV))
                    sprintf(s3, " %s \\cdot %s ", s1, s2);
                else   
                    sprintf(s3, " (%s \\cdot %s) ", s1, s2);
                fprintf(tex, "\\[\n\\ %s\n\\]\n", s3);
                break;

            case DIV:
                sprintf(s3, "\\frac{%s}{%s}", s1, s2);
                fprintf(tex, "\\[\n\\ %s\n\\]\n", s3);
                break;

            case DEG:
                sprintf(s3, "(%s)^{%s}", s1, s2);
                fprintf(tex, "\\[\n\\ %s\n\\]\n", s3);
                break;

            case LOG:
                sprintf(s3, "log_{%s}(%s)", s1, s2);
                fprintf(tex, "\\[\n\\ %s\n\\]\n", s3);
                break;

            case SIN:
                sprintf(s3, "sin(%s)", s2);
                fprintf(tex, "\\[\n\\ %s\n\\]\n", s3);
                break;

            case COS:
                sprintf(s3, "cos(%s)", s2);
                fprintf(tex, "\\[\n\\ %s\n\\]\n", s3);
                break;

            case TG:
                sprintf(s3, "tg(%s)", s2);
                fprintf(tex, "\\[\n\\ %s\n\\]\n", s3);
                break;

            case CTG:
                sprintf(s3, "ctg(%s)", s2);
                fprintf(tex, "\\[\n\\ %s\n\\]\n", s3);
                break;

            default:
                break;
        }

        if (s1 != NULL)
        {   free(s1);
            s1 = NULL;
        }

        if (s2 != NULL)
        {
            free(s2);
            s2 = NULL;
        }
    }
    
    printf("s3 = %s\n", s3);
    return s3;
}

void Dump(FILE *fp, const calculation_tree *tree, ssize_t *rank)
{
    fprintf(fp, "TREE[%p]\n", tree);
    fprintf(fp, "  |  \n");
    fprintf(fp, "ROOT[%p]\n", tree->root);
    fprintf(fp, "BUFFER_WITH_VARIABLES[%p]\n", tree->variables);
    fprintf(fp, "{\n");
    fprintf(fp, "    num_of_el = %zd\n", tree->num_of_el);
    fprintf(fp, "    Buffer with variables content:\n");
    for (ssize_t i = 0; i < tree->variables_size; i++)
    {
        printf("        [%zd]  %s\n", i, tree->variables[i].name);
    }
    fprintf(fp, "    Tree content:\n");

    ssize_t cur_rank = 0;
    ShowTree(fp, tree->root, rank, &cur_rank);

    fprintf(fp, "    rank = %zd\n", *rank);
    fprintf(fp, "\n}");
    fprintf(fp, "\n");
}

void DumpToLogfile(const calculation_tree *tree, const char *logfile_name, const char *gvfile_name, ssize_t *rank)
{
    FILE *fp = fopen(logfile_name, "a");

    if (!OpenFileSuccess(fp, logfile_name))
    {
        return;
    }

    fprintf(fp, "<pre>\n");
    Dump(fp, tree, rank);

    fprintf(fp, "<img src=\"%*s.png\" alt=\"Graphviz image\" width=\"1000\">", (int)strlen(gvfile_name) - 3, gvfile_name);

    if (!CloseFileSuccess(fp, logfile_name))
    {
        return;
    }
}

void CreateGraph(const calculation_tree *tree, const char *gvfile_name)
{
    FILE *fp = fopen(gvfile_name, "w");

    if (!OpenFileSuccess(fp, gvfile_name))
    {
        return;
    }

    fprintf(fp, "digraph CalculationTree_game {\n");
    fprintf(fp, "   rankdir=TB;\n");
    fprintf(fp, "   node[shape=\"record\", style=\"filled\", fillcolor=\"Pink\", fontsize=20, pin = true];\n");
    fprintf(fp, "   splines = ortho;\n");
    fprintf(fp, "   rank = same;\n");
    fprintf(fp, "   nodesep = 0.5;\n");
    fprintf(fp, "   edge[arrowsize = 0.5];\n");
    fprintf(fp, "   bgcolor=\"LightBlue\";\n");

    PrintEdges(fp, tree->root);
    LinkEdges(fp, tree->root);

    fprintf(fp, "\n}");
    if (!CloseFileSuccess(fp, gvfile_name))
    {
        return;
    }

    char command[SIZE_OF_NAME * 2] = {0};
    sprintf(command, "dot %s -Tpng -o %*s.png", gvfile_name, (int)strlen(gvfile_name) - 3, gvfile_name);
    
    int error = system(command);
    if (error)
    {
        printf("%d", error);
        //assert(0);
    }
}

void PrintEdges(FILE *fp, node_t *node)
{
    if (node == NULL)
        return;

    PrintEdges(fp, node->left);

    #ifdef BEAUTIFUL_DUMP
    if (node->type == VAR_TYPE)
        fprintf(fp, "\"node_%p\" [fillcolor = \"Pink\", label = \"{<type> type = %s | <val> val = %s}\"];\n", node, type_buffer[node->type], node->value.var.name);
    else if (node->type == NUM_TYPE)
        fprintf(fp, "\"node_%p\" [fillcolor = \"deeppink\", label = \"{<type> type = %s | <val> val = %lf}\"];\n", node, type_buffer[node->type], node->value.number);
    if (node->type == OP_TYPE)
        fprintf(fp, "\"node_%p\" [fillcolor = \"violet\", label = \"{<type> type = %s | <val> val = %s | {<left_operand>  op_1| <right_operand> op_2}}\"];\n", node, type_buffer[node->type], operations_buffer[node->value.operation]);

    #else
    if (node->left == NULL)
    fprintf(fp, "\"node_%p\" [fillcolor = \"Pink\", label = \"{<parent> %p | <data> %s | {<left> %p |<right> %p}}\"];\n", node, node->parent, node->data, node->left, node->right);
    else fprintf(fp, "\"node_%p\" [fillcolor = \"Pink\", label = \"{<parent> %p | <data> %s | {<left> %p |<right> %p} | {<left_answer> yes |<right_answer> no}}\"];\n", node, node->parent, node->data, node->left, node->right);
    #endif

    PrintEdges(fp, node->right);

    return;
}

void LinkEdges(FILE *fp, node_t *node)
{
    if (node == NULL)
        return;

    LinkEdges(fp, node->left);
    
    if (node->left != NULL) {
        fprintf(fp, "\"node_%p\":left_operand -> \"node_%p\";\n", node, node->left);}

    if (node->right != NULL)
        fprintf(fp, "\"node_%p\":right_operand -> \"node_%p\";\n", node, node->right);
    
    LinkEdges(fp, node->right);

    return;
}

void ShowTree(FILE *fp, node_t *node, ssize_t *rank, ssize_t *cur_rank)
{
    if (node == NULL)
        return;
    fprintf(fp, "(");
    (*cur_rank)++;
    ShowTree(fp, node->left, rank, cur_rank);
    if (*cur_rank > *rank) *rank = *cur_rank;
    fprintf(fp, "\n        node.parent = %p\n        node.left = %p\n        node.right = %p\n        NODE.type_t = %s\n        ", node->parent, node->left, node->right, type_buffer[node->type]);
    switch (node->type)
    {
        case VAR_TYPE:
            fprintf(fp, "node.value = %s\n\n", node->value.var.name);
            break;
        case OP_TYPE:
            fprintf(fp, "node.value = %s\n\n", operations_buffer[node->value.operation]);
            break;
        case NUM_TYPE:
            fprintf(fp, "node.value = %lf\n\n", node->value.number);
            break;
        default:
            break;
    }
    ShowTree(fp, node->right, rank, cur_rank);
    fprintf(fp, ")");
    (*cur_rank)--;
    return;
}

bool OpenFileSuccess(FILE *fp, const char * file_name)
{
    if (fp == NULL)
    {
        printf("An error occurred when opening the file %s", file_name);
        return false;
    }

    return true;
}

bool CloseFileSuccess(FILE *fp, const char * file_name)
{
    if (fclose(fp))
    {
        printf("An error occurred when closing the file %s", file_name);
        return false;
    }
    
    return true;
}

bool PrintError(Calculation_Tree_Errors err)
{
    switch (err)
    {
        case NO_ERROR:
            return true;

        case NULL_POINTER_ON_TREE:
            printf("The pointer on the TREE is empty\n");
            return true;

        case TREE_IS_EMPTY:
            printf("The TREE is empty\n");
            return true;

        case ELEMENT_NOT_FOUND:
            printf("The item was not found in the tree\n");
            return true;

        case ERROR_DURING_THE_CREATION_OF_THE_TREE:
            printf("An error occurred when was the tree created\n");
            return true;

        case NEGATIVE_NUM_OF_ELEMENTS:
            printf("The number of elements takes a negative value\n");
            return true;

        case NULL_POINTER_ON_ROOT:
            printf("The pointer on the ROOT is empty\n");
            return true;

        case ROOT_HAVE_PARENT:
            printf("The ROOT have a parent\n");
            return true;

        case SON_HAS_WRONG_PARENT:
            printf("A son has a wrong parent\n");
            return true;

        case ERROR_DURING_OPENING_FILE:
            printf("An error occurred while opening the file\n");
            return true;

        case ERROR_DURING_CLOSING_FILE:
            printf("An error occurred while closing the file\n");
            return true;

        case ERROR_DURING_MEMORY_ALLOCATION:
            printf("Couldn't allocate dynamic memory\n");
            return true;

        case ERROR_DURING_READ_FILE:
            printf("The number of successfully read characters from the file is less than its content\n");
            return true;

        case INCORRECT_DATA_IN_FILE:
            printf("There are incorrect data in the file with tree nodes\n");
            return true;

        case COLORS_ARE_NOT_SUPPORTED:
            printf("Your terminal does not support color\n");
            return true;

        case NON_EXISTENT_VALUE:
            printf("Non existent value found in the file\n");
            return true;

        default:
            return false;
    }

    return false;
}