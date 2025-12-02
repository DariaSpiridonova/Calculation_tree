#ifndef CALCULATION_TREE
#define CALCULATION_TREE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>
#include <time.h>
#include <sys/stat.h>

#define ASSERTS(tree)\
        assert(tree != NULL);\
        assert(tree->num_of_el >= 0);\
        assert(tree->root != NULL);\
        assert(tree->root->parent == NULL);

#define CALCULATION_TREE_DUMP(tree)\
        CalculationTreeDump(tree,__FILE__, __LINE__)

#define BEAUTIFUL_DUMP 
#define ENABLE_THE_VERIFIER 

typedef int used_type;

const used_type POIZON = 765418;
const ssize_t SIZE_OF_NAME = 200;
const size_t STR_SIZE = 50;
const size_t ANSWER_SIZE = 5;
const int NUM_OF_VARIABLES = 6;
const int NUM_OF_TYPES = 3;
const int NUM_OF_OPERATIONS = 11;
const char link_to_graphviz_file[] = "../Graphviz/calculation_tree_";

const char * const type_buffer[NUM_OF_TYPES] = {"OP_TYPE", "VAR_TYPE", "NUM_TYPE"};
const char * const operations_buffer[NUM_OF_OPERATIONS] = {"\0", "+", "-", "*", "/", "^", "log", "sin", "cos", "tg", "ctg"};
const char * const variables_buffer[NUM_OF_VARIABLES] = {"\0", "x", "y", "z", "t", "u"};

enum type_t 
{
    OP_TYPE,
    VAR_TYPE,
    NUM_TYPE
};

enum var_t
{
    nothing,
    X,
    Y,
    Z,
    T,
    U
};

enum operation_t 
{
    NO_OP,
    ADD,
    SUB,
    MUL,
    DIV,
    DEG,
    LOG,
    SIN,
    COS,
    TG,
    CTG
};

union value_type
{
    double number;
    var_t variable;
    operation_t operation;
};

struct NODE_T
{
    type_t type;
    value_type value;
    struct NODE_T *left;
    struct NODE_T *right;
    struct NODE_T *parent;
};

typedef struct NODE_T node_t;

typedef struct
{
    ssize_t num_of_el;
    var_t *buffer_with_variables;
    node_t *root;
    const char *file_name;
} calculation_tree;

enum Calculation_Tree_Errors
{
    NO_ERROR,
    NULL_POINTER_ON_TREE,
    TREE_IS_EMPTY,
    ELEMENT_NOT_FOUND,
    ERROR_DURING_THE_CREATION_OF_THE_TREE,
    NEGATIVE_NUM_OF_ELEMENTS,
    NULL_POINTER_ON_ROOT,
    ROOT_HAVE_PARENT,
    SON_HAS_WRONG_PARENT,
    ERROR_DURING_OPENING_FILE,
    ERROR_DURING_CLOSING_FILE,
    ERROR_DURING_MEMORY_ALLOCATION,
    ERROR_DURING_READ_FILE,
    INCORRECT_DATA_IN_FILE,
    COLORS_ARE_NOT_SUPPORTED,
    NON_EXISTENT_VALUE,
    DIVISION_BY_ZERO_IS_UNACCEPTABLE
};

bool is_number(const char *node_value);

Calculation_Tree_Errors CalculationTreeInit(calculation_tree *tree, const char *logfile_name);

Calculation_Tree_Errors CalculationTreeVerify(calculation_tree *tree);
bool CheckSonsAndParents(node_t *node, node_t **son, node_t **parent);
void CheckSonsAndParentsRecursive(node_t *node, bool *flag, node_t **son, node_t **parent);

Calculation_Tree_Errors CalculationTreeDestroy(calculation_tree *tree);
Calculation_Tree_Errors CalculationTreeDestroyRecursive(calculation_tree *tree, node_t **node);

void CalculationTreeDump(calculation_tree *tree, const char *file, int line);
void DumpToConsole(const calculation_tree *tree, const char *file, int line, ssize_t *rank);
void DumpToLogfile(const calculation_tree *tree, const char *logfile_name, const char *gvfile_name, ssize_t *rank);
void DumpToTexFile(const calculation_tree *tree, const char *texfile_name);
char *DumpToTexFileRecursive(node_t *node, FILE *tex);
void Dump(FILE *fp, const calculation_tree *tree, ssize_t *rank);
void ShowTree(FILE *fp, node_t *node, ssize_t *rank, ssize_t *cur_rank);
void PrintEdges(FILE *fp, node_t *node);
void LinkEdges(FILE *fp, node_t *node);
void CreateGraph(const calculation_tree *tree, const char *gvfile_name);

// Optimization
Calculation_Tree_Errors OptimizationFunction(calculation_tree *tree);
Calculation_Tree_Errors ConstantsConvolution(calculation_tree *tree);
void ConstantsConvolutionRecursive(calculation_tree *tree, node_t **node);
Calculation_Tree_Errors RemovingNeutralElements(calculation_tree *tree);
node_t *RemovingNeutralElementsRecursive(calculation_tree *tree, node_t *node, Calculation_Tree_Errors *err);
bool is_a_specific_number(double value, int number);

Calculation_Tree_Errors SaveTreeToFile(calculation_tree *tree, const char *name_of_file);
void SaveTreeToFileRecursive(FILE *fp, node_t *node);

Calculation_Tree_Errors MakeTreeFromExpression(calculation_tree *calculation_tree, const char *logfile_name, const char *name_of_file_with_expression);
node_t *BuildingATree(calculation_tree *calculation_tree, char **expression, Calculation_Tree_Errors *err);
char *ReadExpressionFromFile(const char *name_of_file, Calculation_Tree_Errors *err);

Calculation_Tree_Errors ReadTreeFromFile(calculation_tree *tree, const char *name_of_file);
void SplitIntoParts(char *tree_buffer);
char *ReadNodeFromBuffer(calculation_tree *tree, char **position, node_t **node, node_t *parent);
Calculation_Tree_Errors NodeFromFileInit(calculation_tree *tree, char **position, node_t **node, node_t *parent);
size_t return_num_of_bytes_in_file(int fd1);
void SkipSpaces(char **position);

node_t *NewNodeNumInit (calculation_tree *calculation_tree_differential_, double number, node_t *node_left, node_t *node_right);
node_t *NewNodeVarInit (calculation_tree *calculation_tree_differential_, var_t variable, node_t *node_left, node_t *node_right);
node_t *NewNodeOpInit  (calculation_tree *calculation_tree_differential_, operation_t operation, node_t *node_left, node_t *node_right);
node_t *InitNewNode    (calculation_tree *calculation_tree_differential_, node_t *node_left, node_t *node_right);
node_t *SubtreeInit    (calculation_tree *calculation_tree_differential_, node_t *tree_node);

bool OpenFileSuccess(FILE *fp, const char * file_name);
bool CloseFileSuccess(FILE *fp, const char * file_name);

bool PrintError(Calculation_Tree_Errors err);

#endif