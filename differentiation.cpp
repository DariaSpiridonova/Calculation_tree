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

node_t *NewNodeNumInit(calculation_tree *calculation_tree_differential_, double number, node_t *diff_node_left, node_t *diff_node_right)
{
    node_t *diff_node = InitNewNode(calculation_tree_differential_, diff_node_left, diff_node_right);

    diff_node->type = NUM_TYPE;
    diff_node->value.number = number;

    return diff_node;
}

node_t *NewNodeVarInit(calculation_tree *calculation_tree_differential_, var_t variable, node_t *diff_node_left, node_t *diff_node_right)
{
    node_t *diff_node = InitNewNode(calculation_tree_differential_, diff_node_left, diff_node_right);

    diff_node->type = VAR_TYPE;
    diff_node->value.variable = variable;

    return diff_node;
}

node_t *NewNodeOpInit(calculation_tree *calculation_tree_differential_, operation_t operation, node_t *diff_node_left, node_t *diff_node_right)
{
    node_t *diff_node = InitNewNode(calculation_tree_differential_, diff_node_left, diff_node_right);

    diff_node->type = OP_TYPE;
    diff_node->value.operation = operation;

    return diff_node;
}

node_t *InitNewNode(calculation_tree *calculation_tree_differential_, node_t *diff_node_left, node_t *diff_node_right)
{
    node_t *diff_node = (node_t *)calloc(1, sizeof(node_t));

    if (diff_node == NULL)
    {
        return NULL;
    }

    calculation_tree_differential_->num_of_el++;

    diff_node->left = diff_node_left;
    diff_node->right = diff_node_right;

    if (diff_node_left != NULL)
    {
        diff_node->left->parent = diff_node;
    }

    if (diff_node_right != NULL)
    {
        diff_node->right->parent = diff_node;
    }

    return diff_node;
}

node_t *SubtreeInit(calculation_tree *calculation_tree_differential_, node_t *tree_node)
{
    if (tree_node == NULL)
        return NULL;

    node_t *diff_node = (node_t *)calloc(1, sizeof(node_t));

    if (diff_node == NULL)
    {
        return NULL;
    }

    calculation_tree_differential_->num_of_el++;

    diff_node->type = tree_node->type;
    diff_node->value = tree_node->value;

    if (tree_node->left != NULL)
    {
        diff_node->left = SubtreeInit(calculation_tree_differential_, tree_node->left);
        diff_node->left->parent = diff_node;
    }
    else 
    {
        diff_node->left = NULL;
    }

    if (tree_node->right != NULL)
    {
        diff_node->right = SubtreeInit(calculation_tree_differential_, tree_node->right);
        diff_node->right->parent = diff_node;
    }
    else
    {
        diff_node->right = NULL;
    }

    return diff_node;
}