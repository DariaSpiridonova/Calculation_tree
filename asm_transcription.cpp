#include "asm_transcription.h"

Program_Errors TranscriptionIntoAssembler(program_tree *tree, const char *asm_file_name)
{
    ASSERTS(tree);

    Program_Errors err = NO_ERROR;
    if ((err = CalculationTreeVerify(tree)))
    {
        return err;
    }

    FILE *asm_file = fopen(asm_file_name, "w");
    if (!OpenFileSuccess(asm_file, asm_file_name))
        return ERROR_DURING_OPENING_FILE;

    ssize_t mark = 0;

    WriteToAsmFileRecursive(tree, asm_file, tree->root, &mark);

    if (!CloseFileSuccess(asm_file, asm_file_name))
        return ERROR_DURING_CLOSING_FILE;

    return err;
}

void WriteToAsmFileRecursive(program_tree *tree, FILE *fp, node_t *node, ssize_t *mark)
{
    if (node == NULL)
        return;

    WriteToAsmFileRecursive(tree, fp, node->left, mark);
    WriteToAsmFileRecursive(tree, fp, node->right, mark);
    ssize_t index = 0;

    switch((int)node->type)
    {
        case (int)VAR_TYPE: // all
            if (!(node->parent->type == ASSIGN_TYPE && node->parent->left == node) && !(node->parent->type == COMM_TYPE && strcmp(node->parent->name, "print")))
            {
                for (ssize_t i = 0; i < tree->variables_s.variables_size; i++)
                {
                    if (node->name == tree->variables_s.variables[i].name)
                    {
                        index = i;
                        break;
                    }
                }
                fprintf(fp, "PUSH %zd  \n", index);
                fprintf(fp, "POPR SI   \n");
                fprintf(fp, "PUSHM [SI]\n");
            }
            break;

        case (int)NUM_TYPE: // all
            fprintf(fp, " PUSH %lf\n ", node->number);
            break;

        case (int)COND_TYPE:
            if      (!strcmp(node->name, "if")) // did not all 
            {
                fprintf(fp, ":%zd\n", (*mark)++);
            } 
            else if (!strcmp(node->name, "while"))
            {

            }
            break;

        case (int)FUNC_TYPE:  

            break;

        case (int)SEM_POINT_TYPE: // all
            fprintf(fp, "; ****************************************\n");
            break;

        case (int)ASSIGN_TYPE: // all
            for (ssize_t i = 0; i < tree->variables_s.variables_size; i++)
            {
                if (node->left->name == tree->variables_s.variables[i].name)
                {
                    index = i;
                    break;
                }
            }
            fprintf(fp, "PUSH %zd \n", index);
            fprintf(fp, "POPR DI  \n");
            fprintf(fp, "POPM [DI]\n");
            break;

        case (int)COMPARE_TYPE:
            if      (!strcmp(node->name, "=="))
            {
                fprintf(fp, "JE :%zd\n", (*mark)++);
            }
            else if (!strcmp(node->name, ">"))
            {
                fprintf(fp, "JA :%zd\n", (*mark)++);
            }
            else if (!strcmp(node->name, "<"))
            {
                fprintf(fp, "JB :%zd\n", (*mark)++);
            }
            else if (!strcmp(node->name, ">="))
            {
                fprintf(fp, "JAE :%zd\n", (*mark)++);
            }
            else if (!strcmp(node->name, "<="))
            {
                fprintf(fp, "JBE :%zd\n", (*mark)++);
            }
            fprintf(fp, "PUSH 0\n");
            fprintf(fp, "JMP :%zd\n", *mark);
            fprintf(fp, ":%zd\n", *mark - 1);
            fprintf(fp, "PUSH 1\n");
            fprintf(fp, ":%zd\n", (*mark)++);
            break;

        case (int)SIGN_TYPE: // all
            if      (!strcmp(node->name, "+"))
            {
                fprintf(fp, "ADD\n");
            }
            else if (!strcmp(node->name, "-"))
            {
                fprintf(fp, "SUB\n");
            }
            else if (!strcmp(node->name, "*"))
            {
                fprintf(fp, "MUL\n");
            }
            else if (!strcmp(node->name, "/"))
            {
                fprintf(fp, "DIV\n");
            }
            else if (!strcmp(node->name, "^"))
            {
                fprintf(fp, "SQRT\n");
            }
            break;

        case (int)COMM_TYPE: // all
        if      (!strcmp(node->name, "getnum"))
        {
            fprintf(fp, "IN\n");
            for (ssize_t i = 0; i < tree->variables_s.variables_size; i++)
            {
                if (node->right->name == tree->variables_s.variables[i].name)
                {
                    index = i;
                    break;
                }
            }
            fprintf(fp, "PUSH %zd \n", index);
            fprintf(fp, "POPR DI  \n");
            fprintf(fp, "POPM [DI]\n");
        }
        else if (!strcmp(node->name, "print"))
        {
            fprintf(fp, "OUT\n");
        }
            break;

        default:
            printf("A non-existent type was found");
            printf(" \"%s\" ", node->name);
            break;
    }

    if (node->parent != NULL && node->parent->type == COND_TYPE && node->parent->left == node)
    {
        fprintf(fp, "PUSH 0   \n");
        fprintf(fp, "JNE  :%zd\n", (*mark)++);
        fprintf(fp, "POPR   BP\n");
        fprintf(fp, "POPR   BP\n");
        fprintf(fp, "JMP  :%zd\n", *mark);
        fprintf(fp, ":%zd     \n", *mark - 1);
    }
}