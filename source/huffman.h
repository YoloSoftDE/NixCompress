/*
 * File:		/src/huffman.h
 *
 * Project:     nix-compress aka lz1337
 * Author:		Sebastian Büttner
 * Created:		11/28/2012
 *
 */


#ifndef HUFFMAN_H_
#define HUFFMAN_H_

/* C-Standard Includes */

/* Third-party_includes */

/* Cross-project Includes */

/* Project specific Includes */
#include "structs.h"
#include "bin-tree.h"
#include "list.h"
#include "bool.h"

/*---------------------------*/

extern List*              huffman_create(uint32_t *Weights, size_t *count);
extern TreePath*          huffman_transform(List *Table);
extern CodeTable_indexed* huffman_readout(const TreePath *Tree, size_t count);
extern CodeTable_indexed* huffman(uint32_t *Weights);
extern uint32_t* 	      huffman_calc_weight(FILE *fp, int *total);
extern TreePath*          huffman_load(CodeTable* Table);
extern void				  huffman_visualize(const TreeNode *Node, const uint32_t *Weights, int level, int lcount, CT_Entry *Entry);
void					  huffman_print_codetable(CodeTable *Table);
void					  huffman_print_weights(uint32_t *Weights);

#endif /* HUFFMAN_H_ */
