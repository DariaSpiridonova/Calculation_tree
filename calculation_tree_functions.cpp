#include "calculation_tree.h"
#include "differentiation.h"

// static node_t *GetG(calculation_tree *calculation_tree, char **expression, Calculation_Tree_Errors *err);
// static node_t *GetE(calculation_tree *calculation_tree, char **expression, Calculation_Tree_Errors *err);
// static node_t *GetT(calculation_tree *calculation_tree, char **expression, Calculation_Tree_Errors *err);
// static node_t *GetP(calculation_tree *calculation_tree, char **expression, Calculation_Tree_Errors *err);
// static node_t *GetN(calculation_tree *calculation_tree, char **expression, Calculation_Tree_Errors *err);

static bool RecognizingStartPosition(calculation_tree *tree, char **position, node_t **node, node_t *parent);
static bool RecognizingEndPosition(char **position);
static void InitNumNode(node_t **node, bool *value_found, char *node_value);
static void InitNonNumNode(node_t **node, bool *value_found, char *node_value);
static bool IsNumSons(calculation_tree *tree, node_t **node);
static bool IsNumOnlySon(calculation_tree *tree, node_t **node);

static node_t *SimplifyCaseZero(calculation_tree *tree, node_t * node, node_t *zero_node, node_t *sub_node, Calculation_Tree_Errors *err);
static node_t *SimplifyCaseOne(calculation_tree *tree, node_t * node, node_t *one_node, node_t *sub_node);
static node_t *Simplify(calculation_tree *tree, node_t *node, Calculation_Tree_Errors *err);

#undef ENABLE_THE_VERIFIER
Calculation_Tree_Errors CalculationTreeInit(calculation_tree *tree, const char *logfile_name)
{
    assert(tree != NULL);
    assert(logfile_name != NULL);
    
    tree->num_of_el = 1;
    tree->file_name = logfile_name;
    tree->root = (node_t *)calloc(1, sizeof(node_t));
    if (tree->root == NULL)
    {
        return ERROR_DURING_THE_CREATION_OF_THE_TREE;
    }

    //tree->buffer_with_variables = (var_t *)calloc(NUM_OF_VARIABLES, sizeof(var_t));
    //tree->buffer_with_variables[0] = nothing;
    tree->root->left = NULL;
    tree->root->right = NULL;
    tree->root->parent = NULL;
    tree->root->type = OP_TYPE;
    tree->root->value.operation = NO_OP;

    Calculation_Tree_Errors err = NO_ERROR;
    if ((err = CalculationTreeVerify(tree)))
        return err;

    return err;
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

    //free(tree->buffer_with_variables);
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
    if (s1 != NULL) printf("s1 = %s\n", s1);
    s2 = DumpToTexFileRecursive(node->right, tex);
    if (s2 != NULL) printf("s2 = %s\n", s2);

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
            printf("variable = %s\n", variables_buffer[node->value.variable]);

            s3 = (char *)calloc(2, sizeof(char));
            sprintf(s3, "%s", variables_buffer[node->value.variable]);
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
    fprintf(fp, "BUFFER_WITH_VARIABLES[%p]\n", tree->buffer_with_variables);
    fprintf(fp, "{\n");
    fprintf(fp, "    num_of_el = %zd\n", tree->num_of_el);
    fprintf(fp, "    Buffer with variables content:\n");
    // for (size_t i = 0; tree->buffer_with_variables[i] != nothing; i++)
    // {
    //     printf("        [%zu]  %s\n", i, variables_buffer[tree->buffer_with_variables[i]]);
    // }
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
        fprintf(fp, "\"node_%p\" [fillcolor = \"Pink\", label = \"{<type> type = %s | <val> val = %s}\"];\n", node, type_buffer[node->type], variables_buffer[node->value.variable]);
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
            fprintf(fp, "node.value = %s\n\n", variables_buffer[node->value.variable]);
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

Calculation_Tree_Errors SaveTreeToFile(calculation_tree *tree, const char *name_of_file)
{
    ASSERTS(tree);

    Calculation_Tree_Errors err = NO_ERROR;
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
        return;
    }

    if (node->parent != NULL && node->parent->left == node)
        fprintf(fp, "( ");
    
    SaveTreeToFileRecursive(fp, node->left);
    
    switch (node->type)
    {
        case VAR_TYPE:
            fprintf(fp, "%s ", variables_buffer[node->value.variable]);
            break;

        case OP_TYPE:
            fprintf(fp, "%s ", operations_buffer[node->value.operation]);
            break;

        case NUM_TYPE:
            fprintf(fp, "%lf ", node->value.number);
            break;

        default:
            break;
    }

    SaveTreeToFileRecursive(fp, node->right);
    if (node->parent != NULL  && node->parent->right == node)
        fprintf(fp, ") ");
    return;
}

Calculation_Tree_Errors ReadTreeFromFile(calculation_tree *tree, const char *name_of_file)
{
    ASSERTS(tree);

    Calculation_Tree_Errors err = NO_ERROR;
    if ((err = CalculationTreeVerify(tree)))
    {
        return err;
    }

    FILE *file_to_read = fopen(name_of_file, "r");
    if (!OpenFileSuccess(file_to_read, name_of_file))
        return ERROR_DURING_OPENING_FILE;

    fseek(file_to_read, SEEK_SET, 0);
    size_t num_of_bytes_in_file = return_num_of_bytes_in_file(fileno(file_to_read));

    char *tree_buffer = (char *)calloc(num_of_bytes_in_file + 1, sizeof(char));
    if (tree_buffer == NULL)
        return ERROR_DURING_MEMORY_ALLOCATION;

    size_t num_success_read_symbols = fread(tree_buffer, sizeof(char), num_of_bytes_in_file, file_to_read);
    if (num_success_read_symbols < num_of_bytes_in_file)
    {
        free(tree_buffer);
        return ERROR_DURING_READ_FILE;
    }

    tree_buffer[num_success_read_symbols] = '\0';
   
    if (!CloseFileSuccess(file_to_read, name_of_file))
    {
        free(tree_buffer);
        return ERROR_DURING_CLOSING_FILE;
    }

    SplitIntoParts(tree_buffer);

    char *position = tree_buffer;

    ReadNodeFromBuffer(tree, &position, &(tree->root), NULL);

    free(tree_buffer);

    return err;
}

void SplitIntoParts(char *tree_buffer)
{
    assert(tree_buffer != NULL);
    
    char *ptr_to_quotation_mark = tree_buffer;
    ptr_to_quotation_mark = strchr(ptr_to_quotation_mark, '(');
    for (size_t i = 0; (ptr_to_quotation_mark = strchr(ptr_to_quotation_mark, ' ')) != NULL; i++)
    {
        if (*(ptr_to_quotation_mark - 1) == '(');
        else if (*(ptr_to_quotation_mark - 1) == ')');
        else if (*(ptr_to_quotation_mark - 1) != 'l' || *(ptr_to_quotation_mark - 2) != 'i') *ptr_to_quotation_mark = '\0'; 
        ptr_to_quotation_mark++;
    }
}

#define ENABLE_THE_VERIFIER
char *ReadNodeFromBuffer(calculation_tree *tree, char **position, node_t **node, node_t *parent)
{
    ASSERTS(tree);

    Calculation_Tree_Errors err = NO_ERROR;

    #ifdef ENABLE_THE_VERIFIER
    if ((err = CalculationTreeVerify(tree)))
    {
        return NULL;
    }
    #endif

    SkipSpaces(position);

    if (RecognizingStartPosition(tree, position, node, parent)) 
        return *position;

    *position = ReadNodeFromBuffer(tree, position, &((*node)->left), *node);
    *position = ReadNodeFromBuffer(tree, position, &((*node)->right), *node);

    SkipSpaces(position);

    if (RecognizingEndPosition(position)) 
        return *position;

    #ifdef ENABLE_THE_VERIFIER
    if ((err = CalculationTreeVerify(tree)))
    {
        return *position;
    }
    #endif

    return *position;
}

static bool RecognizingStartPosition(calculation_tree *tree, char **position, node_t **node, node_t *parent)
{
    if (**position == '(')
    {
        (*position)++;
        SkipSpaces(position);

        NodeFromFileInit(tree, position, node, parent);

        *position = strchr(*position, '\0') + 1;
    }

    else if (**position == 'n' && *(*position + 1) == 'i' && *(*position + 2) == 'l' && *(*position + 3) == ' ')
    {
        SkipSpaces(position);

        *position += 3;
        return true;
    }

    return false;
}

static bool RecognizingEndPosition(char **position)
{
    if (**position == ')')
    {
        (*position)++;
        return true;
    }

    return false;
}

Calculation_Tree_Errors NodeFromFileInit(calculation_tree *tree, char **position, node_t **node, node_t *parent)
{
    ASSERTS(tree);
    
    Calculation_Tree_Errors err = NO_ERROR;
    if ((err = CalculationTreeVerify(tree)))
        return err;

    if (*node != tree->root)
    {
        *node = (node_t *)calloc(1, sizeof(node_t));
        tree->num_of_el++;
    }

    char *node_value = NULL;
    node_value = strdup(*position);
    bool value_found = false;

    if (is_number(node_value)) 
    {
        InitNumNode(node, &value_found, node_value);
    }

    else
    {
        InitNonNumNode(node, &value_found, node_value);
    }

    free(node_value);

    if (!value_found) 
    {
        return NON_EXISTENT_VALUE;
    }

    (*node)->parent = parent; 
    (*node)->left = NULL;
    (*node)->right = NULL;

    if ((err = CalculationTreeVerify(tree)))
        return err;

    return err;
}

static void InitNumNode(node_t **node, bool *value_found, char *node_value)
{
    (*node)->type = NUM_TYPE;
    (*node)->value.number = atof(node_value);
    *value_found = true;
}

static void InitNonNumNode(node_t **node, bool *value_found, char *node_value)
{
    for (int i = 0; i < NUM_OF_VARIABLES; i++)
    {
        if (!strcmp(node_value, variables_buffer[i]))
        {
            (*node)->type = VAR_TYPE;
            (*node)->value.variable = (var_t)i;
            *value_found = true;
            break;
        }
    }

    for (int i = 0; i < NUM_OF_OPERATIONS; i++)
    {
        if (!strcmp(node_value, operations_buffer[i]))
        {
            (*node)->type = OP_TYPE;
            (*node)->value.operation = (operation_t)i;
            *value_found = true;
            break;
        }
    }
}

Calculation_Tree_Errors OptimizationFunction(calculation_tree *tree)
{
    ASSERTS(tree);

    Calculation_Tree_Errors err = NO_ERROR;
    if ((err = CalculationTreeVerify(tree)))
        return err;

    ssize_t number_of_elements = tree->num_of_el;

    do
    {
        number_of_elements = tree->num_of_el;
        err = ConstantsConvolution(tree);
        err = RemovingNeutralElements(tree);
        if (err) return err;
        CALCULATION_TREE_DUMP(tree);
    }
    while (number_of_elements != tree->num_of_el);

    if ((err = CalculationTreeVerify(tree)))
        return err;

    return err;
}

Calculation_Tree_Errors ConstantsConvolution(calculation_tree *tree)
{
    ASSERTS(tree);

    Calculation_Tree_Errors err = NO_ERROR;
    if ((err = CalculationTreeVerify(tree)))
        return err;

    ssize_t number_of_elements = tree->num_of_el;

    do
    {
        number_of_elements = tree->num_of_el;
        ConstantsConvolutionRecursive(tree, &(tree->root));
        CALCULATION_TREE_DUMP(tree);
    }
    while (number_of_elements != tree->num_of_el);

    if ((err = CalculationTreeVerify(tree)))
        return err;

    return err;
}

void ConstantsConvolutionRecursive(calculation_tree *tree, node_t **node)
{
    if (*node == NULL)
        return;

    if (IsNumSons(tree, node))
        return;

    if (IsNumOnlySon(tree, node))
        return;

    ConstantsConvolutionRecursive(tree, &(*node)->left);
    ConstantsConvolutionRecursive(tree, &(*node)->right);

    return;
}

static bool IsNumSons(calculation_tree *tree, node_t **node)
{
    if ((*node)->type != OP_TYPE || (*node)->left == NULL || (*node)->right == NULL || (*node)->left->type != NUM_TYPE || (*node)->right->type != NUM_TYPE)
        return false;

    (*node)->type = NUM_TYPE;
    switch ((*node)->value.operation)
    {
        case NO_OP:
            break;

        case ADD:
            (*node)->value.number = (*node)->left->value.number + (*node)->right->value.number;
            break;

        case SUB:
            (*node)->value.number = (*node)->left->value.number - (*node)->right->value.number;
            break;

        case MUL:
            (*node)->value.number = (*node)->left->value.number * (*node)->right->value.number;
            break;

        case DIV:
            (*node)->value.number = (*node)->left->value.number / (*node)->right->value.number;
            break;

        case DEG:
            (*node)->value.number = pow((*node)->left->value.number, (*node)->right->value.number);
            break;

        case LOG:
            (*node)->value.number = log((*node)->right->value.number) / log((*node)->left->value.number);
            break;
            
        default:
            break;
    }

    free((*node)->left); 
    (*node)->left = NULL;
    free((*node)->right); 
    (*node)->right = NULL;

    tree->num_of_el -= 2;

    return true;
}

static bool IsNumOnlySon(calculation_tree *tree, node_t **node)
{
    if ((*node)->type != OP_TYPE || (*node)->right == NULL || (*node)->left != NULL || (*node)->right->type != NUM_TYPE)
        return false;

    (*node)->type = NUM_TYPE;

    node_t *node_num = (*node)->left;

    if ((*node)->left == NULL) 
        node_num = (*node)->right;

    switch ((*node)->value.operation)
    {
        case SIN:
            (*node)->value.number = sin(node_num->value.number);
            break;

        case COS:
            (*node)->value.number = cos(node_num->value.number);
            break;

        case TG:
            (*node)->value.number = tan(node_num->value.number);
            break;

        case CTG:
            (*node)->value.number = 1 / tan(node_num->value.number);
            break;
            
        default:
            void((*node)->value.operation);
            break;
    }

    if ((*node)->left != NULL) 
    {
        free((*node)->left); 
        (*node)->left = NULL;
    }
    else if ((*node)->right != NULL) 
    {
        free((*node)->right); 
        (*node)->right = NULL;
    }
    tree->num_of_el--;

    return true;
}

Calculation_Tree_Errors RemovingNeutralElements(calculation_tree *tree)
{
    ASSERTS(tree);

    Calculation_Tree_Errors err = NO_ERROR;
    if ((err = CalculationTreeVerify(tree)))
        return err;

    ssize_t number_of_elements = tree->num_of_el;

    do
    {
        number_of_elements = tree->num_of_el;
        RemovingNeutralElementsRecursive(tree, tree->root, &err);
        CALCULATION_TREE_DUMP(tree);
    }
    while (number_of_elements != tree->num_of_el);

    if ((err = CalculationTreeVerify(tree)))
        return err;

    return err;
}

node_t *SimplifyCaseZero(calculation_tree *tree, node_t * node, node_t *zero_node, node_t *sub_node, Calculation_Tree_Errors *err)
{
    assert(zero_node);

    switch(node->value.operation)
    {

        case SUB:
            if (zero_node == node->left) 
            {
                node->left->value.number = -1;
                node->value.operation = MUL;
                return node;
            }
            [[fallthrough]];

        case ADD:
            free(zero_node);
            zero_node = NULL;

            if (node == tree->root)
            {
                tree->root = sub_node;
                sub_node->parent = NULL;
            }

            else if (node->parent->left == node)
            {
                sub_node->parent = node->parent;
                node->parent->left = sub_node;
            }

            else if (node->parent->right == node)
            {
                sub_node->parent = node->parent;
                node->parent->right = sub_node;
            }

            free(node);
            node = NULL;
            
            tree->num_of_el -= 2;
            return sub_node;

        case DIV:
            if (zero_node == node->right) 
            {
                *err = DIVISION_BY_ZERO_IS_UNACCEPTABLE;
                return sub_node;
            }
            [[fallthrough]];

        case MUL:
            node->type = NUM_TYPE;
            node->value.number = 0;
            free(zero_node);
            zero_node = NULL;

            CalculationTreeDestroyRecursive(tree, &(sub_node));
            node->left = NULL;
            node->right = NULL;
            tree->num_of_el--;
            return sub_node;

        default:
            break;
    }

    return sub_node;
}

node_t *SimplifyCaseOne(calculation_tree *tree, node_t * node, node_t *one_node, node_t *sub_node)
{
    assert(one_node);
   
    switch(node->value.operation)
    {
        
        case DIV:
            if (one_node == node->left) 
            {
                break;
            }
            [[fallthrough]];

        case MUL:
            free(one_node);
            one_node = NULL;

            if (node == tree->root)
            {
                tree->root = sub_node;
                sub_node->parent = NULL;
            }

            else if (node->parent->left == node)
            {
                sub_node->parent = node->parent;
                node->parent->left = sub_node;
            }

            else if (node->parent->right == node)
            {
                sub_node->parent = node->parent;
                node->parent->right = sub_node;
            }

            free(node);
            node = NULL;
            
            tree->num_of_el -= 2;
            return sub_node;

        default:
            break;
    }
    return node;
}

node_t *Simplify(calculation_tree *tree, node_t *node, Calculation_Tree_Errors *err)
{
    if (node == NULL)
        return NULL;

    if (node->type != OP_TYPE || 
        ((node->left == NULL || node->left->type != NUM_TYPE) && (node->right == NULL || node->right->type != NUM_TYPE))) {
        return node;
    }

    printf("OP_TYPE = %s\n", operations_buffer[node->value.operation]);

    printf("(*node)->left ->type = %s\n", type_buffer[node->left->type]);
    printf("(*node)->right->type = %s\n", type_buffer[node->right->type]);

    if (node->right->type == NUM_TYPE && is_a_specific_number(node->right->value.number, 0))
    {
        return SimplifyCaseZero(tree, node, node->right, node->left, err);
    } 
    if (node->left->type == NUM_TYPE && is_a_specific_number(node->left->value.number, 0))
    {
        return SimplifyCaseZero(tree, node, node->left, node->right, err);
    }

    if (node->right->type == NUM_TYPE && is_a_specific_number(node->right->value.number, 1))
    {
        return SimplifyCaseOne(tree, node, node->right, node->left);
    }   
    if (node->left->type == NUM_TYPE && is_a_specific_number(node->left->value.number, 1))
    {
        return SimplifyCaseOne(tree, node, node->left, node->right);
    }

    return node;
}

node_t *RemovingNeutralElementsRecursive(calculation_tree *tree, node_t *node, Calculation_Tree_Errors *err)
{
    assert(tree);
    
    if (node == NULL)
        return NULL;

    if (*err) 
        return node;

    if (node != tree->root) {
        printf("node != tree->root: node->parent-> left = %p"
                                   "node->parent->right = %p" "\n", 
                                   node->parent->left,
                                   node->parent->right);
    }

    if (node != NULL)
    {
        RemovingNeutralElementsRecursive(tree, node->left, err);
        RemovingNeutralElementsRecursive(tree, node->right, err);
    }

    node = Simplify(tree, node, err);
    CALCULATION_TREE_DUMP(tree);

    return node;
}

bool is_number(const char *node_value)
{    
    char *endptr;
    strtod(node_value, &endptr);
    
    return (*endptr == '\0' && endptr != node_value);
}

bool is_a_specific_number(double value, int number)
{
    double e = 1e-50;
    
    if (value <= e + (double)number &&  value >= -e + (double)number)
        return true;
    return false;
}

size_t return_num_of_bytes_in_file(int fd1)
{
    struct stat st1 = {};
    fstat(fd1, &st1);

    return (size_t)st1.st_size;
}

void SkipSpaces(char **position)
{
    while (**position == ' ') 
    {
        (*position)++;
        continue;
    };
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