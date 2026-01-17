#ifndef ASM_TRANSCRIPTION
#define ASM_TRANSCRIPTION

#include "tokens.h"
#include "root_functions.h"

Program_Errors TranscriptionIntoAssembler(program_tree *tree, const char *asm_file_name);
void WriteToAsmFileRecursive(program_tree *tree, FILE *fp, node_t *node, ssize_t *mark, ssize_t *mark_call);

#endif