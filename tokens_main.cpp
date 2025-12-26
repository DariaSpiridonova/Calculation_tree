#include "tokens.h"
#include "root_functions.h"

int main()
{
    program_tree program_tree_ = {0};

    Program_Errors err = NO_ERROR;

    if ((err = MakeTreeFromProgram(&program_tree_, "logfile_for_tree.htm", "program.txt")))
    {
        PrintError(err);
        return 3;
    }

    CALCULATION_TREE_DUMP(&program_tree_);

    if ((err = SaveTreeToFile(&program_tree_, "file_program.txt")))
    {
        PrintError(err);
        return 4;
    }
    
    program_tree program_tree_1 = {0};
    
    if ((err = MakeTreeFromFile(&program_tree_1, "logfile_for_tree_from_second_file.htm", "file_program.txt")))
    {
        PrintError(err);
        return 4;
    }
    
    CALCULATION_TREE_DUMP(&program_tree_1);

    // if ((err = OptimizationFunction(&program_tree_)))
    // {
    //     PrintError(err);
    //     return 5;
    // }

    // bool is_var = false;

    // if (program_tree_.variables_size != 0)
    // {
    //     printf("Enter the differentiation variable: ");
    //     char *var = {};
    //     size_t len_var = 0;
    //     getline(&var, &len_var, stdin);
    //     var[strlen(var) - 1] = '\0';

    //     for (ssize_t i = 0; i < program_tree_.variables_size; i++)
    //     {
    //         if (!strcmp(program_tree_.variables[i].name, var))
    //             is_var = true;
    //     }

    //     if (is_var)
    //     {
    //         if ((err = MakeDifferentiationTree(&program_tree_differential_, "logfile_for_tree_differential.htm", &program_tree_, var)))
    //         {
    //             PrintError(err);
    //             return 7;
    //         }

    //         // if ((err = OptimizationFunction(&program_tree_differential_)))
    //         // {
    //         //     PrintError(err);
    //         //     return 5;
    //         // }
    //     }

    //     else 
    //     {
    //         printf("The variable %s does not exist\n", var);
    //     }

    //     free(var);
    // }

    // CALCULATION_TREE_DUMP(&program_tree_);

    // if (is_var)
    // {
    //     CALCULATION_TREE_DUMP(&program_tree_differential_);
    // }
    
    if ((err = CalculationTreeDestroy(&program_tree_)))
    {
        PrintError(err);
        return 8;
    }

    if ((err = CalculationTreeDestroy(&program_tree_1)))
    {
        PrintError(err);
        return 9;
    }

    // if (is_var)
    // {
    //     if ((err = CalculationTreeDestroy(&program_tree_differential_)))
    //     {
    //         PrintError(err);
    //         return 9;
    //     }
    //     printf("WAS\n\n\n\n\n");
    // }

    return 0;
}