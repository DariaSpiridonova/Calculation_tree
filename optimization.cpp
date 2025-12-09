#include "optimization.h"

static bool IsNumSons(calculation_tree *tree, node_t **node);
static bool IsNumOnlySon(calculation_tree *tree, node_t **node);

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

