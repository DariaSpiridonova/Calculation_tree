#include "tokens.h"
#include "root_functions.h"
#include "recursive_descent_for_tokens.h"

Program_Errors MakeTreeFromProgram(program_tree *tree, const char *logfile_name, const char *name_of_file_with_expression)
{
    Program_Errors err = NO_ERROR;
    
    char *expression = ReadExpressionFromFile(name_of_file_with_expression, &err);
    char *expression_beginning = expression;

    printf("expression = %s\n", expression);

    if (err)
    {
        free(expression);
        return err;
    }

    tokens_t tokens = {0};

    if ((err = MakeTokensBuffer(&tokens, &expression)))
    {
        return err;
    }

    printf("tokens_size = %zd\n", tokens.tokens_size);

    for (ssize_t i = 0; i < tokens.tokens_size; i++)
    {
        printf("%zd: name = %s\n", i, tokens.tokens_buffer[i].name);
        printf("     type = %s\n", type_buffer[(int)tokens.tokens_buffer[i].type]);
    }
    
    free(expression_beginning);
    
    if (err)
    {
        return err;
    }
    
    tree->num_of_el = 0;
    tree->variables_s.variables = (variable *)calloc((size_t)NUM_OF_VARIABLES, sizeof(variable));
    tree->variables_s.variables_size = 0;
    tree->variables_s.variables_capacity = NUM_OF_VARIABLES;
    tree->functions_s.functions = (function *)calloc((size_t)NUM_OF_FUNCTIONS, sizeof(function));
    tree->functions_s.functions_size = 0;
    tree->functions_s.functions_capacity = NUM_OF_FUNCTIONS;
    tree->file_name = logfile_name;
    
    tree->root = BuildingATree(tree, tokens, &err);
    
    if (tree->root != NULL)
        tree->root->parent = NULL;

    DestroyTokensBuffer(&tokens);

    if ((err = CalculationTreeVerify(tree)))
    {
        return err;
    }

    return err;
}

Program_Errors MakeTokensBuffer(tokens_t *tokens, char **expression)
{
    Program_Errors err = NO_ERROR;
    
    assert(expression != NULL);

    tokens->tokens_buffer = (token_t *)calloc(NUM_OF_TOKENS, sizeof(token_t));

    if (tokens->tokens_buffer == NULL)
    {
        printf("ERROR_DURING_MEMORY_ALLOCATION during creating a tokens_buffer");
        err = ERROR_DURING_MEMORY_ALLOCATION;
    }

    tokens->tokens_capacity = NUM_OF_TOKENS;
    tokens->tokens_size = 0;

    while (**expression != '\0')
    {
        NeccesaryExpansion(tokens);

        if (IsChrInside(expression, "(){}"))
        {
            MakeParToken(tokens, expression);
            SkipSpaces(expression);
            
            continue;
        }

        if (IsChrInside(expression, "+-*/^"))
        {
            MakeSignToken(tokens, expression);
            SkipSpaces(expression);
            continue;
        }

        if (IsChrInside(expression, "<>"))
        {
            MakeCompToken(tokens, expression);
            SkipSpaces(expression);

            continue;
        }

        if (IsChrInside(expression, "="))
        {
            MakeCompOrAssignToken(tokens, expression);
            SkipSpaces(expression);

            continue;
        }

        if (IsChrInside(expression, "0123456789"))
        {
            MakeNumToken(tokens, expression);
            SkipSpaces(expression);

            continue;
        }

        if (!strncmp(*expression, "print", strlen("print")))
        {
            MakeCommandToken(tokens, expression, "print");
            continue;
        }

        if (!strncmp(*expression, "getnum", strlen("getnum")))
        {
            MakeCommandToken(tokens, expression, "getnum");
            continue;
        }

        if (!strncmp(*expression, "getstr", strlen("getstr")))
        {
            MakeCommandToken(tokens, expression, "getstr");
            continue;
        }

        if (!strncmp(*expression, "func_def", strlen("func_def")))
        {
            printf("func_def\n");
            MakeFunctionToken(tokens, expression, "func_def");
            continue;
        }

        if (!strncmp(*expression, "func_call", strlen("func_call")))
        {
            printf("func_call\n");
            MakeFunctionToken(tokens, expression, "func_call");
            continue;
        }

        if (strlen(*expression) >= strlen("while"))
        {
            if (!strncmp(*expression, "while", strlen("while")))
            {
                MakeCondToken(tokens, expression, "while");
                continue;
            }
        }

        if (strlen(*expression) > strlen("if"))
        {
            if (!strncmp(*expression, "if", strlen("if")))
            {
                MakeCondToken(tokens, expression, "if");
                continue;
            }

            for (ssize_t i = 1; i < NUM_OF_OPERATIONS; i++)
            {
                if (!strncmp(*expression, operations_buffer[i], strlen(operations_buffer[i])))
                {
                    MakeOpToken(tokens, expression, operations_buffer[i]);
                    continue;
                }
            }
        }

        if (IsChrInside(expression, ";"))
        {
            MakeSemToken(tokens, expression);
            SkipSpaces(expression);
            
            continue;
        }

        if (IsChrInside(expression, ","))
        {
            MakeCommaToken(tokens, expression);
            SkipSpaces(expression);

            continue;
        }

        if (isalnum(**expression))
        {
            MakeVarToken(tokens, expression);
            SkipSpaces(expression);

            continue;
        }
    }

    return err;
}

void DestroyTokensBuffer(tokens_t *tokens)
{
    for (ssize_t i = 0; i < tokens->tokens_size; i++)
    {
        free(tokens->tokens_buffer[i].name);
    }

    free(tokens->tokens_buffer);
    tokens->tokens_size = 0;
    tokens->tokens_capacity = 0;

    tokens->tokens_buffer = NULL;
}

void NeccesaryExpansion(tokens_t *tokens)
{
    if (tokens->tokens_size >= tokens->tokens_capacity)
    {
        tokens->tokens_capacity *= 2;
        tokens->tokens_buffer = (token_t *)realloc(tokens->tokens_buffer, (size_t)tokens->tokens_capacity * sizeof(token_t));
    }
}

bool IsChrInside(char **expression, const char *string)
{
    return strchr(string, **expression) != NULL;
}

void MakeParToken(tokens_t *tokens, char **expression)
{
    tokens->tokens_buffer[tokens->tokens_size].type = PAR_TYPE;
    tokens->tokens_buffer[tokens->tokens_size].name = strndup(*expression, 1);

    tokens->tokens_size++;
    (*expression)++;
}

void MakeFunctionToken(tokens_t *tokens, char **expression, const char *func_act)
{
    (*expression) += strlen(func_act);
    SkipSpaces(expression);

    if (isalnum(**expression))
    {
        MakeFuncToken(tokens, expression, !strcmp(func_act, "func_def") ? 1 : 0);
        SkipSpaces(expression);
    }
}

void MakeFuncToken(tokens_t *tokens, char **expression, const bool is_func_def)
{
    char *func = *expression;
    size_t len = 0;
    while (isalnum(**expression))
    {
        (*expression)++;
        len++;
    }

    is_func_def ? 
        tokens->tokens_buffer[tokens->tokens_size].type = FUNC_DEF_TYPE 
    : 
        tokens->tokens_buffer[tokens->tokens_size].type = FUNC_CALL_TYPE;

    tokens->tokens_buffer[tokens->tokens_size].name = strndup(func, len);

    tokens->tokens_size++;
}

void MakeCompToken(tokens_t *tokens, char **expression)
{
    size_t n = 1;
    if (*(*expression+1) == '=')
    {
        n++;
    }

    tokens->tokens_buffer[tokens->tokens_size].type = COMPARE_TYPE;
    tokens->tokens_buffer[tokens->tokens_size].name = strndup(*expression, n);

    tokens->tokens_size++;
    (*expression) += n;
}

void MakeCompOrAssignToken(tokens_t *tokens, char **expression)
{
    size_t n = 1;
    if (*(*expression+1) == '=')
    {
        n++;
        tokens->tokens_buffer[tokens->tokens_size].type = COMPARE_TYPE;
    }
    
    else
    {
        tokens->tokens_buffer[tokens->tokens_size].type = ASSIGN_TYPE;
    }
    
    tokens->tokens_buffer[tokens->tokens_size].name = strndup(*expression, n);
    tokens->tokens_size++;
    (*expression) += n;
}

void MakeCondToken(tokens_t *tokens, char **expression, const char *condition)
{
    tokens->tokens_buffer[tokens->tokens_size].type = COND_TYPE;
    tokens->tokens_buffer[tokens->tokens_size].name = strndup(*expression, strlen(condition));
    *expression += strlen(condition);

    tokens->tokens_size++;
    SkipSpaces(expression);
}

void MakeCommandToken(tokens_t *tokens, char **expression, const char *condition)
{
    tokens->tokens_buffer[tokens->tokens_size].type = COMM_TYPE;
    tokens->tokens_buffer[tokens->tokens_size].name = strndup(*expression, strlen(condition));
    *expression += strlen(condition);

    tokens->tokens_size++;
    SkipSpaces(expression);
}

void MakeOpToken(tokens_t *tokens, char **expression, const char *func)
{
    tokens->tokens_buffer[tokens->tokens_size].type = FUNC_TYPE;
    tokens->tokens_buffer[tokens->tokens_size].name = strndup(*expression, strlen(func));
    *expression += strlen(func);

    tokens->tokens_size++;
    SkipSpaces(expression);
}

void MakeNumToken(tokens_t *tokens, char **expression)
{
    tokens->tokens_buffer[tokens->tokens_size].type = NUM_TYPE;
    char *value = *expression;
    size_t n = 0;
    while (IsChrInside(expression, "0123456789.")) 
    {
        (*expression)++;
        n++;
    }

    tokens->tokens_buffer[tokens->tokens_size].name = strndup(value, n);
    tokens->tokens_buffer[tokens->tokens_size].number = atof(value);

    tokens->tokens_size++;
}

void MakeSemToken(tokens_t *tokens, char **expression)
{
    tokens->tokens_buffer[tokens->tokens_size].type = SEM_POINT_TYPE;
    tokens->tokens_buffer[tokens->tokens_size].name = strndup(*expression, 1);
    (*expression)++;

    tokens->tokens_size++;
    SkipSpaces(expression);
}

void MakeCommaToken(tokens_t *tokens, char **expression)
{
    tokens->tokens_buffer[tokens->tokens_size].type = COMMA_TYPE;
    tokens->tokens_buffer[tokens->tokens_size].name = strndup(*expression, 1);
    (*expression)++;

    tokens->tokens_size++;
    SkipSpaces(expression);
}

void MakeSignToken(tokens_t *tokens, char **expression)
{
    tokens->tokens_buffer[tokens->tokens_size].type = SIGN_TYPE;
    tokens->tokens_buffer[tokens->tokens_size].name = strndup(*expression, 1);
    (*expression)++;

    tokens->tokens_size++;
    SkipSpaces(expression);
}

void MakeVarToken(tokens_t *tokens, char **expression)
{
    char *var = *expression;
    size_t len = 0;
    while (isalnum(**expression))
    {
        (*expression)++;
        len++;
    }

    tokens->tokens_buffer[tokens->tokens_size].type = VAR_TYPE;
    tokens->tokens_buffer[tokens->tokens_size].name = strndup(var, len);

    tokens->tokens_size++;
}

char *ReadExpressionFromFile(const char *name_of_file, Program_Errors *err)
{
    FILE *file_to_read = fopen(name_of_file, "r");
    if (!OpenFileSuccess(file_to_read, name_of_file))
    {
        *err = ERROR_DURING_OPENING_FILE;
        return NULL;
    }

    fseek(file_to_read, 0, SEEK_SET);
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

    expression[num_success_read_symbols] = '\0';
    
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

node_t *BuildingATree(program_tree *tree, tokens_t tokens, Program_Errors *err)
{
    return GetG(tree, tokens, err);
}