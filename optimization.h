#ifndef OPTIMIZATION
#define OPTIMIZATION

#include "recursive_descent.h"

Calculation_Tree_Errors OptimizationFunction(calculation_tree *tree);
Calculation_Tree_Errors ConstantsConvolution(calculation_tree *tree);
void ConstantsConvolutionRecursive(calculation_tree *tree, node_t **node);
Calculation_Tree_Errors RemovingNeutralElements(calculation_tree *tree);
node_t *RemovingNeutralElementsRecursive(calculation_tree *tree, node_t *node, Calculation_Tree_Errors *err);
bool is_a_specific_number(double value, int number);

node_t *SimplifyCaseZero(calculation_tree *tree, node_t * node, node_t *zero_node, node_t *sub_node, Calculation_Tree_Errors *err);
node_t *SimplifyCaseOne(calculation_tree *tree, node_t * node, node_t *one_node, node_t *sub_node);
node_t *Simplify(calculation_tree *tree, node_t *node, Calculation_Tree_Errors *err);

#endif