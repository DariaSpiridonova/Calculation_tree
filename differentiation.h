#ifndef DIFFERENTIATION_TREE
#define DIFFERENTIATION_TREE

#include "calculation_tree.h"

#define dL Differentiation(calculation_tree_differential_, tree_node->left, ind_var)
#define dR Differentiation(calculation_tree_differential_, tree_node->right, ind_var)
#define cL SubtreeInit(calculation_tree_differential_, tree_node->left)
#define cR SubtreeInit(calculation_tree_differential_, tree_node->right)

#define deg2 NewNodeNumInit(calculation_tree_differential_, 2, NULL, NULL)
#define e    NewNodeNumInit(calculation_tree_differential_, e_num, NULL, NULL)

#define IND_VAR_()     NewNodeNumInit(calculation_tree_differential_, 1, NULL, NULL)
#define NOT_IND_VAR_() NewNodeVarInit(calculation_tree_differential_, tree_node->value.variable, NULL, NULL)
#define NUM_()         NewNodeNumInit(calculation_tree_differential_, 0, NULL, NULL)

#define ADD_(left, right) NewNodeOpInit(calculation_tree_differential_, ADD, left, right)
#define SUB_(left, right) NewNodeOpInit(calculation_tree_differential_, SUB, left, right)
#define MUL_(left, right) NewNodeOpInit(calculation_tree_differential_, MUL, left, right)
#define DIV_(left, right) NewNodeOpInit(calculation_tree_differential_, DIV, left, right)
#define DEG_(left, right) NewNodeOpInit(calculation_tree_differential_, DEG, left, right)
#define LOG_(left, right) NewNodeOpInit(calculation_tree_differential_, LOG, left, right)

const double e_num = 2.718281;

Calculation_Tree_Errors MakeDifferentiationTree(calculation_tree *calculation_tree_differential_, const char *logfile_name, calculation_tree *tree, var_t ind_var);
node_t *Differentiation(calculation_tree *calculation_tree_differential_, node_t *tree_node, var_t ind_var);
node_t *NewNodeNumInit(calculation_tree *calculation_tree_differential_, double number, node_t *diff_node_left, node_t *diff_node_right);
node_t *NewNodeVarInit(calculation_tree *calculation_tree_differential_, var_t variable, node_t *diff_node_left, node_t *diff_node_right);
node_t *NewNodeOpInit(calculation_tree *calculation_tree_differential_, operation_t operation, node_t *diff_node_left, node_t *diff_node_right);
node_t *SubtreeInit(calculation_tree *calculation_tree_differential_, node_t *tree_node);

#endif
