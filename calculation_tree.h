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
    NON_EXISTENT_VALUE
};

bool is_number(char *node_value);

Calculation_Tree_Errors CalculationTreeInit(calculation_tree *tree, const char *logfile_name);

Calculation_Tree_Errors CalculationTreeVerify(calculation_tree *tree);
bool CheckSonsAndParents(node_t *node, node_t **son, node_t **parent);
void CheckSonsAndParentsRecursive(node_t *node, bool *flag, node_t **son, node_t **parent);

Calculation_Tree_Errors CalculationTreeDestroy(calculation_tree *tree);
Calculation_Tree_Errors CalculationTreeDestroyRecursive(calculation_tree *tree, node_t **node);

void CalculationTreeDump(calculation_tree *tree, const char *file, int line);
void DumpToConsole(const calculation_tree *tree, const char *file, int line, ssize_t *rank);
void DumpToLogfile(const calculation_tree *tree, const char *logfile_name, const char *gvfile_name, ssize_t *rank);
void Dump(FILE *fp, const calculation_tree *tree, ssize_t *rank);
void ShowTree(FILE *fp, node_t *node, ssize_t *rank, ssize_t *cur_rank);
void PrintEdges(FILE *fp, node_t *node);
void LinkEdges(FILE *fp, node_t *node);
void CreateGraph(const calculation_tree *tree, const char *gvfile_name);

Calculation_Tree_Errors TreeOptimization(calculation_tree *tree);
void TreeOptimizationRecursive(calculation_tree *tree, node_t **node);

// Calculation_Tree_Errors CalculationTreeGame(calculation_tree *tree);
// void GetARequestNumber(ssize_t *num);
// Calculation_Tree_Errors PlayGame(calculation_tree *tree);
// void AskQuestion(node_t **node);
// Calculation_Tree_Errors Victory(calculation_tree *tree);
// Calculation_Tree_Errors AddingNode(calculation_tree *tree, node_t *node);
// Calculation_Tree_Errors NodeInit(calculation_tree *tree, node_t **node, node_t *parent, char **string);
// bool GetAnswer();
// void GetObject(char **ptr);
// void GetFeature(char **ptr, char *object, node_t *node);
// bool GetDifference(char **ptr, char *object, node_t *node);
// void DiscardSuperfluous();

// Calculation_Tree_Errors FindObject(calculation_tree *tree);
// bool FindObjectRecursive(calculation_tree *tree, node_t *node, char *object, node_t **ptr_object, bool *object_was_found, node_t **ptrs_on_nodes, size_t *index);
// void GetDescription(calculation_tree *tree, node_t * ptr_object, node_t **ptrs_on_nodes, size_t i);

// Calculation_Tree_Errors CompareObjects(calculation_tree *tree);
// void compare_found_items(calculation_tree *tree, bool *not_root_element, node_t **ptrs1_on_nodes, node_t **ptrs2_on_nodes, node_t *ptr_object1, node_t *ptr_object2);
// void print_general_signs(bool not_root_element, node_t **ptrs1_on_nodes, node_t **ptrs2_on_nodes, size_t *i1, size_t *i2);
// void print_differences(calculation_tree *tree, node_t **ptrs1_on_nodes, node_t **ptrs2_on_nodes, node_t *ptr_object1, node_t *ptr_object2, size_t *i1, size_t *i2);
// void free_objects(char *object1, char *object2, node_t **ptrs1_on_nodes, node_t **ptrs2_on_nodes);

Calculation_Tree_Errors SaveTreeToFile(calculation_tree *tree, const char *name_of_file);
void SaveTreeToFileRecursive(FILE *fp, node_t *node);

Calculation_Tree_Errors ReadTreeFromFile(calculation_tree *tree, const char *name_of_file);
void SplitIntoParts(char *tree_buffer);
char *ReadNodeFromBuffer(calculation_tree *tree, char **position, node_t **node, node_t *parent);
Calculation_Tree_Errors NodeFromFileInit(calculation_tree *tree, char **position, node_t **node, node_t *parent);
size_t return_num_of_bytes_in_file(int fd1);
void SkipSpaces(char **position);

bool OpenFileSuccess(FILE *fp, const char * file_name);
bool CloseFileSuccess(FILE *fp, const char * file_name);

bool PrintError(Calculation_Tree_Errors err);

#endif