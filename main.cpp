#include "calculation_tree.h"

int main()
{
    calculation_tree akinator_tree;

    Calculation_Tree_Errors err = NO_ERROR;

    if ((err = CalculationTreeInit(&akinator_tree, "logfile_for_tree1.htm")))
    {
        PrintError(err);
        return 2;
    }

    if ((err = ReadTreeFromFile(&akinator_tree, "calculation_tree.txt")))
    {
        PrintError(err);
        return 3;
    }

    CALCULATION_TREE_DUMP(&akinator_tree);

    if ((err = RemovingNeutralElements(&akinator_tree)))
    {
        PrintError(err);
        return 5;
    }

    if ((err = ConstantsConvolution(&akinator_tree)))
    {
        PrintError(err);
        return 4;
    }    

    if ((err = SaveTreeToFile(&akinator_tree, "calculation_tree_expression.txt")))
    {
        PrintError(err);
        return 6;
    }

    if ((err = CalculationTreeDestroy(&akinator_tree)))
    {
        PrintError(err);
        return 7;
    }

    return 0;
}