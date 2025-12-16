#include "tokens.h"

static node_t *GetE(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num);
static node_t *GetT(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num);
static node_t *GetP(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num);
static node_t *GetN(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num);
static node_t *GetF(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num);
static node_t *GetV(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num);
static node_t *GetOp(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num);
static node_t *GetCond(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num);
static node_t *GetA(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num);
static node_t *GetD(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num);
static node_t *GetDeg(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num);

node_t *GetG(program_tree *tree, tokens_t tokens, Program_Errors *err)
{
    printf("case G\n");
    ssize_t token_num = 0;
    node_t *val = GetOp(tree, tokens, err, &token_num);
    if (strcmp(tokens.tokens_buffer[token_num].name, ";"))
    {
        printf("ABSENCE_SEMICOLON\n");
        *err = ABSENCE_SEMICOLON;
    }
    printf("G\n");

    return val;
}

static node_t *GetOp(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num)
{
    node_t *val = NULL;
    node_t *val2 = NULL;
    token_t sem;

    if (tokens.tokens_buffer[*token_num].type == COND_TYPE)
    {
        val = GetCond(tree, tokens, err, token_num);
    }

    else 
    {
        val = GetA(tree, tokens, err, token_num);
        printf("*token_num44 = %zd\n", *token_num);
    }

    while (tokens.tokens_buffer[*token_num].type == SEM_POINT_TYPE)
    {
        if (*token_num >= tokens.tokens_size - 1 || (*token_num < tokens.tokens_size - 1 && tokens.tokens_buffer[*token_num + 1].type == PAR_TYPE))
        {
            printf("\n***********Stop cycle******************\n");
            return val;
        }

        sem = tokens.tokens_buffer[*token_num];
        (*token_num)++;
        if (tokens.tokens_buffer[*token_num].type == COND_TYPE)
        {
            val2 = GetCond(tree, tokens, err, token_num);
        }

        else 
        {
            val2 = GetA(tree, tokens, err, token_num);
        }

        val = NewNodeStringInit(tree, sem, val, val2, err);
    }

    return val;
}

static node_t *GetCond(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num)
{
    node_t *val1 = NULL;
    node_t *val2 = NULL;
    node_t *val = NULL;

    token_t condition = tokens.tokens_buffer[*token_num];
    (*token_num)++;
    if (tokens.tokens_buffer[*token_num].type == PAR_TYPE && !strcmp(tokens.tokens_buffer[*token_num].name, "("))
    {
        (*token_num)++;
        val1 = GetD(tree, tokens, err, token_num);
        if (tokens.tokens_buffer[*token_num].type == PAR_TYPE && !strcmp(tokens.tokens_buffer[*token_num].name, ")"))
        {
            (*token_num)++;
            if (tokens.tokens_buffer[*token_num].type == PAR_TYPE && !strcmp(tokens.tokens_buffer[*token_num].name, "{"))
            {
                (*token_num)++;
                printf("*token_num90 = %zd\n", *token_num);
                val2 = GetOp(tree, tokens, err, token_num);

                if (strcmp(tokens.tokens_buffer[*token_num].name, ";"))
                {
                    printf("ABSENCE_SEMICOLON\n");
                    *err = ABSENCE_SEMICOLON;
                }
                else 
                {
                    (*token_num)++;
                }

                printf("*token_num92 = %zd\n", *token_num);
                if (tokens.tokens_buffer[*token_num].type == PAR_TYPE && !strcmp(tokens.tokens_buffer[*token_num].name, "}"))
                {
                    (*token_num)++;
                    val = NewNodeStringInit(tree, condition, val1, val2, err);
                }
                else
                {
                    printf("ABSENCE_PAR_CLOSE 94\n");
                    *err = ABSENCE_PAR_CLOSE;
                }
            }
            else 
            {
                printf("ABSENCE_PAR_OPEN\n");
                *err = ABSENCE_PAR_OPEN;
            }
        }
        else 
        {
            printf("ABSENCE_PAR_CLOSE 106\n");
            *err = ABSENCE_PAR_CLOSE;
        }
    }
    else 
    {
        printf("ABSENCE_PAR_OPEN\n");
        *err = ABSENCE_PAR_OPEN;
    }
    
    return val;
}

static node_t *GetA(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num)
{
    node_t *val2 = NULL;
    node_t *val = NULL;

    val = GetV(tree, tokens, err, token_num);
    if (tokens.tokens_buffer[*token_num].type == ASSIGN_TYPE)
    {
        token_t assign = tokens.tokens_buffer[*token_num];
        (*token_num)++;
        val2 = GetD(tree, tokens, err, token_num);
        printf("*token_num136 = %zd\n", *token_num);
        val = NewNodeStringInit(tree, assign, val, val2, err);
    }

    else 
    {
        printf("ABSENCE_ASSIGN\n");
        *err = ABSENCE_ASSIGN;
    }

    return val;
}

static node_t *GetD(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num)
{
    node_t * val2 = NULL;
    node_t * val = NULL;

    val = GetE(tree, tokens, err, token_num);
    if (tokens.tokens_buffer[*token_num].type == COMPARE_TYPE)
    {
        token_t symbol = tokens.tokens_buffer[*token_num];
        (*token_num)++;
        val2 = GetE(tree, tokens, err, token_num);
        val = NewNodeStringInit(tree, symbol, val, val2, err);
        printf("\n<<<<<<<<<<type_compare = %s>>>>>>>>>>>>\n", type_buffer[(int)val->type]);
    }

    return val;
}

static node_t *GetE(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num)
{
    node_t *val = GetT(tree, tokens, err, token_num);
    while (tokens.tokens_buffer[*token_num].type == SIGN_TYPE && (!strcmp(tokens.tokens_buffer[*token_num].name, "+") || !strcmp(tokens.tokens_buffer[*token_num].name, "-")))
    {
        token_t sign = tokens.tokens_buffer[*token_num];
        (*token_num)++;
        node_t *val2 = GetT(tree, tokens, err, token_num);
        val = NewNodeStringInit(tree, sign, val, val2, err);
    }

    return val;
}

static node_t *GetT(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num)
{
    node_t *val = GetDeg(tree, tokens, err, token_num);
    while (tokens.tokens_buffer[*token_num].type == SIGN_TYPE && (!strcmp(tokens.tokens_buffer[*token_num].name, "*") || !strcmp(tokens.tokens_buffer[*token_num].name, "/")))
    {
        token_t sign = tokens.tokens_buffer[*token_num];
        (*token_num)++;
        node_t *val2 = GetDeg(tree, tokens, err, token_num);
        val = NewNodeStringInit(tree, sign, val, val2, err);
    }

    return val;
}

static node_t *GetDeg(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num)
{
    node_t *val = GetP(tree, tokens, err, token_num);
    while (tokens.tokens_buffer[*token_num].type == SIGN_TYPE && !strcmp(tokens.tokens_buffer[*token_num].name, "^"))
    {
        token_t sign = tokens.tokens_buffer[*token_num];
        (*token_num)++;
        node_t *val2 = GetP(tree, tokens, err, token_num);
        val = NewNodeStringInit(tree, sign, val, val2, err);
    }

    return val;
}

static node_t *GetP(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num)
{
    if (tokens.tokens_buffer[*token_num].type == PAR_TYPE && !strcmp(tokens.tokens_buffer[*token_num].name, "("))
    {
        (*token_num)++;
        node_t *val = GetE(tree, tokens, err, token_num);
        if (strcmp(tokens.tokens_buffer[*token_num].name, ")")) 
        {
            printf("ABSENCE_PAR_CLOSE\n");
            *err = ABSENCE_PAR_CLOSE;
        }
        else 
            (*token_num)++;

        return val;
    }

    if (tokens.tokens_buffer[*token_num].type == NUM_TYPE)
    { 
        return GetN(tree, tokens, err, token_num);
    }

    else if (tokens.tokens_buffer[*token_num].type == FUNC_TYPE)
    {
        return GetF(tree, tokens, err, token_num);
    }

    else 
        return GetV(tree, tokens, err, token_num);
}

static node_t *GetN(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num)
{
    token_t val = tokens.tokens_buffer[*token_num];
    (*token_num)++;
    printf("*token_num242 = %zd\n", *token_num);

    printf("val = %lf\n", val.number);

    return NewNodeNumInit(tree, val, NULL, NULL, err);
}

static node_t *GetF(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num)
{
    node_t *val = NULL;
    token_t func = tokens.tokens_buffer[*token_num];
    (*token_num)++;

    if (tokens.tokens_buffer[*token_num].type == PAR_TYPE && !strcmp(tokens.tokens_buffer[*token_num].name, "("))
    {
        (*token_num)++;
        val = GetE(tree, tokens, err, token_num);
        if (tokens.tokens_buffer[*token_num].type == PAR_TYPE && !strcmp(tokens.tokens_buffer[*token_num].name, ")"))
        {
            (*token_num)++;
        }
        else 
        {
            printf("ABSENCE_PAR_CLOSE\n");
            *err = ABSENCE_PAR_CLOSE;
        }
    }
    else 
    {
        printf("ABSENCE_PAR_OPEN\n");
        *err = ABSENCE_PAR_OPEN;
    }

    if (val != NULL)
        return NewNodeStringInit(tree, func, NULL, val, err);

    else    
    {
        printf("The %s function has an empty argument.\n", tokens.tokens_buffer[*token_num].name);
        *err = EMPTY_FUNC_ARG;
    }

    return val;
}

static node_t *GetV(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num)
{
    token_t var = tokens.tokens_buffer[*token_num];
    (*token_num)++;

    return NewNodeVarInit(tree, var, NULL, NULL, err);
}

node_t *NewNodeStringInit(program_tree *tree, token_t token, node_t *node_left, node_t *node_right, Program_Errors *err)
{
    node_t *node = InitNewNode(tree, node_left, node_right, err);

    node->type = token.type;
    node->name = strdup(token.name);

    return node;
}

node_t *NewNodeNumInit(program_tree *tree, token_t token, node_t *node_left, node_t *node_right, Program_Errors *err)
{
    node_t *node = InitNewNode(tree, node_left, node_right, err);

    node->type = token.type;
    node->name = strdup(token.name);
    node->number = token.number;

    return node;
}

node_t *InitNewNode(program_tree *tree, node_t *node_left, node_t *node_right, Program_Errors *err)
{
    node_t *node = (node_t *)calloc(1, sizeof(node_t));

    if (node == NULL)
    {
        printf("ERROR_DURING_MEMORY_ALLOCATION in InitNewNode\n");
        *err = ERROR_DURING_MEMORY_ALLOCATION;
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

node_t *NewNodeVarInit(program_tree *tree, token_t token, node_t *node_left, node_t *node_right, Program_Errors *err)
{
    node_t *node = InitNewNode(tree, node_left, node_right, err);

    bool is_var = false;
    for (ssize_t i = 0; i < tree->variables_s.variables_size; i++)
    {
        if (!strcmp(tree->variables_s.variables[i].name, token.name))
        {
            node->name = tree->variables_s.variables[i].name;
            is_var = true;
            printf("var = %s\n\n\n", tree->variables_s.variables[i].name);
            break;
        }
    }

    if (!is_var)
    {
        tree->variables_s.variables[tree->variables_s.variables_size] = {NAN, strdup(token.name), false};
        node->name = tree->variables_s.variables[tree->variables_s.variables_size].name;
        printf("var = %s\n\n\n", tree->variables_s.variables[tree->variables_s.variables_size].name);
        tree->variables_s.variables_size++;
    }

    node->type = token.type;


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

    return node;
}

// node_t *SubtreeInit(program_tree *tree, node_t *tree_node)
// {
//     if (tree_node == NULL)
//         return NULL;

//     node_t *node = (node_t *)calloc(1, sizeof(node_t));

//     if (node == NULL)
//     {
//         return NULL;
//     }

//     tree->num_of_el++;

//     node->type = tree_node->type;
//     node->value = tree_node->value;

//     if (tree_node->left != NULL)
//     {
//         node->left = SubtreeInit(tree, tree_node->left);
//         node->left->parent = node;
//     }
//     else 
//     {
//         node->left = NULL;
//     }

//     if (tree_node->right != NULL)
//     {
//         node->right = SubtreeInit(tree, tree_node->right);
//         node->right->parent = node;
//     }
//     else
//     {
//         node->right = NULL;
//     }

//     return node;
// }
