#include "calculation_tree.h"
#include "differentiation.h"

int main()
{
    calculation_tree calculation_tree_ = {0};
    calculation_tree calculation_tree_differential_ = {0};

    Calculation_Tree_Errors err = NO_ERROR;

    if ((err = CalculationTreeInit(&calculation_tree_, "logfile_for_tree1.htm")))
    {
        PrintError(err);
        return 1;
    }

    // if ((err = MakeTreeFromExpression(&calculation_tree_, "logfile_for_tree1.htm", "expression.txt")))
    // {
    //     PrintError(err);
    //     return 3;
    // }

    if ((err = ReadTreeFromFile(&calculation_tree_, "calculation_tree.txt")))
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

    if ((err = SaveTreeToFile(&calculation_tree_, "calculation_tree_expression.txt")))
    {
        PrintError(err);
        return 6;
    }

    if ((err = MakeDifferentiationTree(&calculation_tree_differential_, "logfile_for_tree_differential.htm", &calculation_tree_, X)))
    {
        PrintError(err);
        return 7;
    }

    CALCULATION_TREE_DUMP(&calculation_tree_differential_);

    if ((err = OptimizationFunction(&calculation_tree_differential_)))
    {
        PrintError(err);
        return 5;
    }

    CALCULATION_TREE_DUMP(&calculation_tree_differential_);

    if ((err = CalculationTreeDestroy(&calculation_tree_)))
    {
        PrintError(err);
        return 8;
    }

    if ((err = CalculationTreeDestroy(&calculation_tree_differential_)))
    {
        PrintError(err);
        return 9;
    }

    return 0;
}