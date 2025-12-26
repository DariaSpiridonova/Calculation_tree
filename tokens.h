#ifndef TOKENS
#define TOKENS

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

const ssize_t SIZE_OF_NAME = 200;
const ssize_t NUM_OF_VARIABLES = 10;
const ssize_t NUM_OF_TYPES = 12;
const ssize_t NUM_OF_SIGNES = 10;
const ssize_t NUM_OF_OPERATIONS = 6;
const ssize_t NUM_OF_SYMBOLS = 6;
const ssize_t NUM_OF_TOKENS = 30;
const char link_to_graphviz_file[] = "../Graphviz/program_tree_";

const char * const type_buffer[NUM_OF_TYPES] = {"VAR_TYPE", "NUM_TYPE", "PAR_TYPE", "COND_TYPE", "FUNC_TYPE", "FUNCTION_TYPE", "COMMA_TYPE", "SEM_POINT_TYPE", "ASSIGN_TYPE", "COMPARE_TYPE", "SIGN_TYPE", "COMM_TYPE"};
const char type_alphas__buffer[NUM_OF_TYPES] = {'V', 'N', 'P', 'C', 'F', 'U', 'J', 'E', 'A', 'M', 'I', 'O'};

enum type_t 
{
    VAR_TYPE,
    NUM_TYPE,
    PAR_TYPE,
    COND_TYPE,
    FUNC_TYPE,
    FUNCTION_TYPE,
    COMMA_TYPE,
    SEM_POINT_TYPE,
    ASSIGN_TYPE,
    COMPARE_TYPE,
    SIGN_TYPE,
    COMM_TYPE
};

const char * const signes_buffer[NUM_OF_SIGNES] = {"\0", "+", "-", "*", "/", "^"};
const char * const operations_buffer[NUM_OF_OPERATIONS] = {"\0", "log", "sin", "cos", "tg", "ctg"};
const char * const compare_symbols[NUM_OF_SYMBOLS] = {"\0", "==", ">", "<", ">=", "<="};
const char * const symbols_in_words[NUM_OF_SYMBOLS] = {"\0", "==", "&jt;", "&lt;", "&ge;", "&le;"};
const char * const io_buffer[NUM_OF_SYMBOLS] = {"\0", "getnum", "getstr", "print"};
const char * const conditions_buffer[NUM_OF_SYMBOLS] = {"\0", "if", "while"};

enum input_output_t 
{
    NO_COM,
    GET,
    RETURN
};

enum condition_t
{
    NO_COND,
    IF,
    WHILE
};

enum sign_t 
{
    NO_SIGN,
    PLUS,
    MINUS,
    MUL,
    DIV,
    DEG
};

enum operation_t 
{
    NO_OP,
    LOG,
    SIN,
    COS,
    TG,
    CTG
};

enum symbol_t 
{
    NO_SYM,
    EQ,
    ABOVE,
    BELOW,
    AE,
    BE
};

typedef struct 
{
    double value;
    char *name;
    bool is_value;
} variable;

union value_type
{
    double number;
    variable var;
    operation_t operation;
};

typedef struct
{
    type_t type;
    char *name;
    double number;
} token_t;

typedef struct 
{
    token_t *tokens_buffer;
    ssize_t tokens_size;
    ssize_t tokens_capacity;
} tokens_t;

typedef struct 
{
    variable *variables;
    ssize_t variables_size;
    ssize_t variables_capacity;
} variables_t;

typedef struct node_t
{
    type_t type;
    char *name;
    variable var;
    double number;
    struct node_t *left;
    struct node_t *right;
    struct node_t *parent;
} node_t;

typedef struct
{
    ssize_t num_of_el;
    variables_t variables_s;
    node_t *root;
    const char *file_name;
} program_tree;

enum Program_Errors
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
    DIVISION_BY_ZERO_IS_UNACCEPTABLE,
    ABSENCE_SEMICOLON,
    ABSENCE_PAR_OPEN,
    ABSENCE_PAR_CLOSE,
    EMPTY_FUNC_ARG,
    ABSENCE_ASSIGN,
    ABSENCE_PARAMS
};

Program_Errors MakeTreeFromProgram(program_tree *tree, const char *logfile_name, const char *name_of_file_with_tokens);
Program_Errors MakeTokensBuffer(tokens_t *tokens, char **expression);
void DestroyTokensBuffer(tokens_t *tokens);
void NeccesaryExpansion(tokens_t *tokens);
bool IsChrInside(char **expression, const char *string);
void MakeParToken(tokens_t *tokens, char **expression);
void MakeFunctionToken(tokens_t *tokens, char **expression);
void MakeFuncToken(tokens_t *tokens, char **expression);
void MakeCompToken(tokens_t *tokens, char **expression);
void MakeCompOrAssignToken(tokens_t *tokens, char **expression);
void MakeOpToken(tokens_t *tokens, char **expression, const char *function);
void MakeCondToken(tokens_t *tokens, char **expression, const char *condition);
void MakeCommandToken(tokens_t *tokens, char **expression, const char *condition);
void MakeSignToken(tokens_t *tokens, char **expression);
void MakeNumToken(tokens_t *tokens, char **expression);
void MakeSemToken(tokens_t *tokens, char **expression);
void MakeCommaToken(tokens_t *tokens, char **expression);
void MakeVarToken(tokens_t *tokens, char **expression);

Program_Errors SaveTreeToFile(program_tree *tree, const char *name_of_file);
void SaveTreeToFileRecursive(FILE *fp, node_t *node);

Program_Errors MakeTreeFromFile(program_tree *tree, const char *logfile_name, const char *name_of_file);
void SplitIntoParts(char *tree_buffer);

char *ReadNodeFromBuffer(program_tree *tree, char **position, node_t **node, node_t *parent);
Program_Errors NodeFromFileInit(program_tree *tree, char **position, node_t **node, node_t *parent);
node_t *NewNodeStringInitByPos(node_t **node, int type);
node_t *NewNodeNumInitByPos(node_t **node);
node_t *NewNodeVarInitByPos(program_tree *tree, node_t **node, Program_Errors *err);

node_t *GetG(program_tree *tree, tokens_t tokens, Program_Errors *err);

node_t *InitNewNode(program_tree *tree, node_t *node_left, node_t *node_right, Program_Errors *err);
node_t *NewNodeVarInit(program_tree *tree, token_t token, node_t *node_left, node_t *node_right, Program_Errors *err);
node_t *NewNodeStringInit(program_tree *tree, token_t token, node_t *node_left, node_t *node_right, Program_Errors *err);
node_t *NewNodeNumInit(program_tree *tree, token_t token, node_t *node_left, node_t *node_right, Program_Errors *err);

char *ReadExpressionFromFile(const char *name_of_file, Program_Errors *err);
size_t return_num_of_bytes_in_file(int fd1);
node_t *BuildingATree(program_tree *tree, tokens_t tokens, Program_Errors *err);

#endif