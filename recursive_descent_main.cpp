#include "optimization.h"
#include "differentiation.h"

int main()
{
    calculation_tree calculation_tree_ = {0};
    calculation_tree calculation_tree_differential_ = {0};

    Calculation_Tree_Errors err = NO_ERROR;

    if ((err = MakeTreeFromExpression(&calculation_tree_, "logfile_for_tree.htm", "expression.txt")))
    {
        PrintError(err);
        return 3;
    }

    CALCULATION_TREE_DUMP(&calculation_tree_);

    if ((err = OptimizationFunction(&calculation_tree_)))
    {
        PrintError(err);
        return 5;
    }

    bool is_var = false;

    if (calculation_tree_.variables_size != 0)
    {
        printf("Enter the differentiation variable: ");
        char *var = {};
        size_t len_var = 0;
        getline(&var, &len_var, stdin);
        var[strlen(var) - 1] = '\0';

        for (ssize_t i = 0; i < calculation_tree_.variables_size; i++)
        {
            if (!strcmp(calculation_tree_.variables[i].name, var))
                is_var = true;
        }

        if (is_var)
        {
            if ((err = MakeDifferentiationTree(&calculation_tree_differential_, "logfile_for_tree_differential.htm", &calculation_tree_, var)))
            {
                PrintError(err);
                return 7;
            }

            // if ((err = OptimizationFunction(&calculation_tree_differential_)))
            // {
            //     PrintError(err);
            //     return 5;
            // }
        }

        else 
        {
            printf("The variable %s does not exist\n", var);
        }

        free(var);
    }

    CALCULATION_TREE_DUMP(&calculation_tree_);

    if (is_var)
    {
        CALCULATION_TREE_DUMP(&calculation_tree_differential_);
    }
    
    if ((err = CalculationTreeDestroy(&calculation_tree_)))
    {
        PrintError(err);
        return 8;
    }

    if (is_var)
    {
        if ((err = CalculationTreeDestroy(&calculation_tree_differential_)))
        {
            PrintError(err);
            return 9;
        }
        printf("WAS\n\n\n\n\n");
    }

    return 0;
}