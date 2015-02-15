/*
 * File:		/src/bin-tree.c
 *
 * Project:     nix-compress aka lz1337
 * Author:		Sebastian Büttner
 * Created:		11/27/2012
 *
 */


#ifndef BIN_TREE_H_
#define BIN_TREE_H_

/* Includes*/

// c-standard lib includes
#include <string.h>

// third-party includes

// cross-project includes

// project-specific includes
#include "bool.h"


typedef struct TreeNode {
	void   *Data;				// 4 Bytes
	struct TreeNode *Parent;	// 4 Bytes
	struct TreePath *Left;		// 4 Bytes
	struct TreePath *Right;		// 4 Bytes
} TreeNode;						// = 16 Bytes

typedef struct TreePath {
	double   Weight;			// 8 Bytes
	TreeNode *Node;				// 4 Bytes
} TreePath;						// = 12 Bytes

extern TreePath* t_create(double Weight);
extern TreeNode* t_insert(TreeNode *parent, TreePath *new_element);
extern void		 t_delete(TreePath *tp);

#endif /* BIN_TREE_H_ */
