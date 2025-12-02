#include "calculation_tree.h"
#include "differentiation.h"

node_t *Differentiation(calculation_tree *calculation_tree_differential_, node_t *tree_node, var_t ind_var)
{
    switch (tree_node->type)
    {
        case VAR_TYPE:
            if (tree_node->value.variable == ind_var)
                return IND_VAR_();
            else
                return NUM_(0);

        case NUM_TYPE:
            return NUM_(0);

        case OP_TYPE:
            switch(tree_node->value.operation)
            {
                case ADD:
                    return ADD_(dL, dR);

                case SUB:
                    return SUB_(dL, dR);

                case MUL:
                    return ADD_(MUL_(dL, cR), MUL_(cL, dR));
                
                case DIV:
                    return DIV_(SUB_(MUL_(dL, cR), MUL_(cL, dR)), DEG_(cR, NUM_(2)));

                case DEG:
                    return MUL_(ADD_(MUL_(MUL_(DIV_(NUM_(1), cL), dL), cR), MUL_(dR, lnL)), DEG_(cL, cR));

                case LOG:
                    return DIV_(SUB_(MUL_(MUL_(dR, DIV_(NUM_(1), cR)), lnL), MUL_(MUL_(dL, DIV_(NUM_(1), cL)), lnR)), DEG_(lnL, NUM_(2)));

                case SIN:
                    return MUL_(COS_(NULL, cR), dR);

                case COS:
                    return MUL_(MUL_(NUM_(-1), SIN_(NULL, cR)), dR);

                case TG:
                    return MUL_(DIV_(NUM_(1), DEG_(COS_(NULL, cR), NUM_(2))), dR);

                case CTG:
                    return MUL_(DIV_(NUM_(-1), DEG_(SIN_(NULL, cR), NUM_(2))), dR);

                default:
                    break;
            }

        default:
            break;
    }

    return NULL;
}

Calculation_Tree_Errors MakeDifferentiationTree(calculation_tree *calculation_tree_differential_, const char *logfile_name, calculation_tree *tree, var_t ind_var)
{
    calculation_tree_differential_->num_of_el = 0;
    calculation_tree_differential_->file_name = logfile_name;
    calculation_tree_differential_->root = Differentiation(calculation_tree_differential_, tree->root, ind_var);
    if (calculation_tree_differential_->root != NULL)
        calculation_tree_differential_->root->parent = NULL;

    calculation_tree_differential_->buffer_with_variables = (var_t *)calloc(NUM_OF_VARIABLES, sizeof(var_t));
    calculation_tree_differential_->buffer_with_variables[0] = nothing;

    Calculation_Tree_Errors err = NO_ERROR;

    if ((err = CalculationTreeVerify(calculation_tree_differential_)))
    {
        return err;
    }

    return err;
}

node_t *NewNodeNumInit(calculation_tree *calculation_tree_differential_, double number, node_t *node_left, node_t *node_right)
{
    node_t *node = InitNewNode(calculation_tree_differential_, node_left, node_right);

    node->type = NUM_TYPE;
    node->value.number = number;

    return node;
}

node_t *NewNodeVarInit(calculation_tree *calculation_tree_differential_, var_t variable, node_t *node_left, node_t *node_right)
{
    node_t *node = InitNewNode(calculation_tree_differential_, node_left, node_right);

    node->type = VAR_TYPE;
    node->value.variable = variable;

    return node;
}

node_t *NewNodeOpInit(calculation_tree *calculation_tree_differential_, operation_t operation, node_t *node_left, node_t *node_right)
{
    node_t *node = InitNewNode(calculation_tree_differential_, node_left, node_right);

    node->type = OP_TYPE;
    node->value.operation = operation;

    return node;
}

node_t *InitNewNode(calculation_tree *calculation_tree_differential_, node_t *node_left, node_t *node_right)
{
    node_t *node = (node_t *)calloc(1, sizeof(node_t));

    if (node == NULL)
    {
        return NULL;
    }

    calculation_tree_differential_->num_of_el++;
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

node_t *SubtreeInit(calculation_tree *calculation_tree_differential_, node_t *tree_node)
{
    if (tree_node == NULL)
        return NULL;

    node_t *node = (node_t *)calloc(1, sizeof(node_t));

    if (node == NULL)
    {
        return NULL;
    }

    calculation_tree_differential_->num_of_el++;

    node->type = tree_node->type;
    node->value = tree_node->value;

    if (tree_node->left != NULL)
    {
        node->left = SubtreeInit(calculation_tree_differential_, tree_node->left);
        node->left->parent = node;
    }
    else 
    {
        node->left = NULL;
    }

    if (tree_node->right != NULL)
    {
        node->right = SubtreeInit(calculation_tree_differential_, tree_node->right);
        node->right->parent = node;
    }
    else
    {
        node->right = NULL;
    }

    return node;
}