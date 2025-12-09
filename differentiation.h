#ifndef DIFFERENTIATION_TREE
#define DIFFERENTIATION_TREE

#include "recursive_descent.h"

#define dL Differentiation(calculation_tree_differential_, tree_node->left, ind_var)
#define dR Differentiation(calculation_tree_differential_, tree_node->right, ind_var)
#define cL SubtreeInit(calculation_tree_differential_, tree_node->left)
#define cR SubtreeInit(calculation_tree_differential_, tree_node->right)

#define lnL NewNodeOpInit(calculation_tree_differential_, LOG, NUM_(e_num), cL)
#define lnR NewNodeOpInit(calculation_tree_differential_, LOG, NUM_(e_num), cR)

#define IND_VAR_()           NewNodeNumInit(calculation_tree_differential_, 1, NULL, NULL)
#define NOT_IND_VAR_()       NewNodeVarInit(calculation_tree_differential_, tree_node->value.variable, NULL, NULL)
#define NUM_(number)         NewNodeNumInit(calculation_tree_differential_, number, NULL, NULL)

#define ADD_(left, right) NewNodeOpInit(calculation_tree_differential_, ADD, left, right)
#define SUB_(left, right) NewNodeOpInit(calculation_tree_differential_, SUB, left, right)
#define MUL_(left, right) NewNodeOpInit(calculation_tree_differential_, MUL, left, right)
#define DIV_(left, right) NewNodeOpInit(calculation_tree_differential_, DIV, left, right)
#define DEG_(left, right) NewNodeOpInit(calculation_tree_differential_, DEG, left, right)
#define LOG_(left, right) NewNodeOpInit(calculation_tree_differential_, LOG, left, right)
#define SIN_(left, right) NewNodeOpInit(calculation_tree_differential_, SIN, left, right)
#define COS_(left, right) NewNodeOpInit(calculation_tree_differential_, COS, left, right)

const double e_num = 2.718281;

Calculation_Tree_Errors MakeDifferentiationTree(calculation_tree *calculation_tree_differential_, const char *logfile_name, calculation_tree *tree, char *ind_var);

node_t *Differentiation(calculation_tree *calculation_tree_differential_, node_t *tree_node, char *ind_var);

#endif
