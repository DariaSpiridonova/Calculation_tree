#include "tokens.h"
#include "root_functions.h"

Program_Errors SaveTreeToFile(program_tree *tree, const char *name_of_file)
{
    ASSERTS(tree);

    Program_Errors err = NO_ERROR;
    if ((err = CalculationTreeVerify(tree)))
    {
        return err;
    }

    FILE *file_to_read = fopen(name_of_file, "w");
    if (!OpenFileSuccess(file_to_read, name_of_file))
        return ERROR_DURING_OPENING_FILE;

    SaveTreeToFileRecursive(file_to_read, tree->root);

    if (!CloseFileSuccess(file_to_read, name_of_file))
        return ERROR_DURING_CLOSING_FILE;

    return err;
}

void SaveTreeToFileRecursive(FILE *fp, node_t *node)
{
    if (node == NULL)
    {
        fprintf(fp, "nil ");
        return;
    }
    
    fprintf(fp, " ( ");
    
    switch((int)node->type)
    {
        case (int)VAR_TYPE:
            fprintf(fp, " \"%s\" ", node->name);
            fprintf(fp, " \"V\" ");
            break;

        case (int)NUM_TYPE:
            fprintf(fp, " \"%lf\" ", node->number);
            fprintf(fp, " \"N\" ");
            break;

        case (int)PAR_TYPE:
            fprintf(fp, " \"%s\" ", node->name);
            fprintf(fp, " \"P\" ");
            break;

        case (int)COND_TYPE:
            fprintf(fp, " \"%s\" ", node->name);
            fprintf(fp, " \"C\" ");
            break;

        case (int)FUNC_TYPE:
            fprintf(fp, " \"%s\" ", node->name);
            fprintf(fp, " \"F\" ");
            break;

        case (int)SEM_POINT_TYPE:
            fprintf(fp, " \"%s\" ", node->name);
            fprintf(fp, " \"E\" ");
            break;

        case (int)ASSIGN_TYPE:
            fprintf(fp, " \"%s\" ", node->name);
            fprintf(fp, " \"A\" ");
            break;

        case (int)COMPARE_TYPE:
            fprintf(fp, " \"%s\" ", node->name);
            fprintf(fp, " \"M\" ");
            break;

        case (int)SIGN_TYPE:
            fprintf(fp, " \"%s\" ", node->name);
            fprintf(fp, " \"I\" ");
            break;

        case (int)COMM_TYPE:
            fprintf(fp, " \"%s\" ", node->name);
            fprintf(fp, " \"O\" ");
            break;

        default:
            printf("A non-existent type was found");
            printf(" \"%s\" ", node->name);
            break;
            
    }
    SaveTreeToFileRecursive(fp, node->left);
    SaveTreeToFileRecursive(fp, node->right);
    fprintf(fp, " ) ");
    return;
}

Program_Errors MakeTreeFromFile(program_tree *tree, const char *logfile_name, const char *name_of_file)
{
    Program_Errors err = NO_ERROR;
 
    char *expression = ReadExpressionFromFile(name_of_file, &err);
    printf("expression = %s\n", expression);
    if (err)
    {
        free(expression);
        return err;
    }
    
    SplitIntoParts(expression);
    char *position = expression;

    tree->num_of_el = 0;
    tree->variables_s.variables = (variable *)calloc((size_t)NUM_OF_VARIABLES, sizeof(variable));
    tree->variables_s.variables_size = 0;
    tree->variables_s.variables_capacity = NUM_OF_VARIABLES;
    tree->file_name = logfile_name;
    tree->root = NULL;

    ReadNodeFromBuffer(tree, &position, &(tree->root), NULL);
    
    if (tree->root != NULL)
        tree->root->parent = NULL;

    free(expression);

    if ((err = CalculationTreeVerify(tree)))
    {
        return err;
    }

    return err;
}

char *ReadNodeFromBuffer(program_tree *tree, char **position, node_t **node, node_t *parent)
{
    #ifdef ENABLE_THE_VERIFIER
    if ((err = CalculationTreeVerify(tree)))
    {
        return NULL;
    }
    #endif

    SkipSpaces(position);

    if (**position == '(')
    {
        (*position)++;
        SkipSpaces(position);

        if (**position == '"')
        {
            printf("Is PAR\n");
            (*position)++;
            NodeFromFileInit(tree, position, node, parent);
        }

        else return NULL;
    }

    else if (**position == 'n' && *(*position + 1) == 'i' && *(*position + 2) == 'l' && *(*position + 3) == ' ')
    {
        SkipSpaces(position);

        *position += 3;
        return *position;
    }

    *position = ReadNodeFromBuffer(tree, position, &((*node)->left), *node);
    *position = ReadNodeFromBuffer(tree, position, &((*node)->right), *node);

    SkipSpaces(position);
    if (**position == ')')
    {
        (*position)++;
        return *position;
    }

    #ifdef ENABLE_THE_VERIFIER
    if ((err = AkinatorVerify(tree)))
    {
        return *position;
    }
    #endif

    return *position;
}

Program_Errors NodeFromFileInit(program_tree *tree, char **position, node_t **node, node_t *parent)
{
    Program_Errors err = NO_ERROR;

    printf("inside\n");

    *node = (node_t *)calloc(1, sizeof(node_t));

    if (*node == NULL)
    {
        printf("Error during memory allocation\n");
    }

    (*node)->left = NULL;
    (*node)->right = NULL;
    (*node)->parent = parent;

    (*node)->name = strdup(*position);
    printf("(*node)->name = <%s>\n", *position);

    *position = strchr(*position, '\0') + 1;
    *position = strchr(*position, '"') + 1;
    printf("*position = <%s>\n", *position);

    char type = **position;
    *position = strchr(*position, '\0') + 1;

    int type_num = 0;

    for (int i = 0; i < NUM_OF_TYPES; i++)
    {
        if (type_alphas__buffer[i] == type)
        {
            type_num = i;
        }
    }

    switch(type_num)
    {
        case (int)VAR_TYPE:
            NewNodeVarInitByPos(tree, node, &err);
            break;

        case (int)NUM_TYPE:
            NewNodeNumInitByPos(node);
            break;

        default:
            NewNodeStringInitByPos(node, type_num);
    }

    if ((err = CalculationTreeVerify(tree)))
        return err;

    return err;
}

node_t *NewNodeStringInitByPos(node_t **node, int type)
{
    (*node)->type = (type_t)type;

    return *node;
}

node_t *NewNodeNumInitByPos(node_t **node)
{
    (*node)->type = NUM_TYPE;
    (*node)->number = atof((*node)->name);

    return *node;
}

node_t *NewNodeVarInitByPos(program_tree *tree, node_t **node, Program_Errors *err)
{
    bool is_var = false;
    for (ssize_t i = 0; i < tree->variables_s.variables_size; i++)
    {
        if (!strcmp(tree->variables_s.variables[i].name, (*node)->name))
        {
            free((*node)->name);
            (*node)->name = tree->variables_s.variables[i].name;
            is_var = true;
            printf("var = %s\n\n\n", tree->variables_s.variables[i].name);
            break;
        }
    }

    if (!is_var)
    {
        tree->variables_s.variables[tree->variables_s.variables_size] = {NAN, strdup((*node)->name), false};
        free((*node)->name);
        (*node)->name = tree->variables_s.variables[tree->variables_s.variables_size].name;
        printf("var = %s\n\n\n", tree->variables_s.variables[tree->variables_s.variables_size].name);
        tree->variables_s.variables_size++;
    }

    (*node)->type = VAR_TYPE;

    if (tree->variables_s.variables_size >= tree->variables_s.variables_capacity - 1)
    {
        tree->variables_s.variables_capacity *= 2;
        tree->variables_s.variables = (variable *)realloc(tree->variables_s.variables, (size_t)tree->variables_s.variables_capacity*sizeof(variable));

        if (tree->variables_s.variables == NULL)
        {
            printf("ERROR_DURING_MEMORY_ALLOCATION in NewNodeVarInit\n");
            *err = ERROR_DURING_MEMORY_ALLOCATION;
        }
    }

    return *node;
}

void SplitIntoParts(char *tree_buffer)
{
    assert(tree_buffer != NULL);
    
    char *ptr_to_quotation_mark = tree_buffer;
    for (size_t i = 0; (ptr_to_quotation_mark = strchr(ptr_to_quotation_mark, '"')) != NULL; i++)
    {
        if (i % 2) *ptr_to_quotation_mark = '\0';
        ptr_to_quotation_mark++;
    }
}