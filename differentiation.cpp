#include "differentiation.h"

node_t *Differentiation(calculation_tree *calculation_tree_differential_, node_t *tree_node, char *ind_var)
{
    switch (tree_node->type)
    {
        case VAR_TYPE:
            if (!strcmp(tree_node->value.var.name, ind_var))
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

Calculation_Tree_Errors MakeDifferentiationTree(calculation_tree *calculation_tree_differential_, const char *logfile_name, calculation_tree *tree, char *ind_var)
{
    Calculation_Tree_Errors err = NO_ERROR;
    
    calculation_tree_differential_->variables = (variable *)calloc((size_t)tree->variables_capacity, sizeof(variable));
    calculation_tree_differential_->variables_size = 0;
     
    calculation_tree_differential_->num_of_el = 0;
    calculation_tree_differential_->file_name = logfile_name;
    calculation_tree_differential_->root = Differentiation(calculation_tree_differential_, tree->root, ind_var);
    if (calculation_tree_differential_->root != NULL)
        calculation_tree_differential_->root->parent = NULL;

    

    if ((err = CalculationTreeVerify(calculation_tree_differential_)))
    {
        return err;
    }

    return err;
}