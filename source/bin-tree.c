/*
 * File:		/src/bin-tree.c
 *
 * Project:     nix-compress aka lz1337
 * Author:		Sebastian Büttner
 * Created:		11/27/2012
 *
 */


/* Includes*/

// c-standard lib includes
#include <stdlib.h>

// third-party includes

// cross-project includes

// project-specific includes
#include "bin-tree.h"

/*---------------------------*/


TreePath*
t_create(double Weight) {
	TreePath    *tp;										// 4 Bytes, stack
	TreeNode    *tn;										// 4 Bytes, stack
	tn        = (TreeNode*) malloc( sizeof(TreeNode) );		// 16 Bytes (TreeNode), heap
	tp        = (TreePath*) malloc( sizeof(TreePath) );		// 12 Bytes (TreePath), heap
	tn->Left  = 0x00;
	tn->Right = 0x00;
	tn->Parent= 0x00;

	tn->Data  = 0x00;

	tp->Node  = tn;
	tp->Weight = Weight;
	return tp;												// = 28 Bytes
}

TreeNode*
t_insert(TreeNode *parent, TreePath *new_element) {
	if (parent->Left == 0x00) {
		parent->Left = new_element;
		new_element->Node->Parent = parent->Left->Node;
	} else if (parent->Right == 0x00) {
		parent->Right = new_element;
		new_element->Node->Parent = parent->Right->Node;
	} else {
		t_insert(parent->Left->Node, new_element);
	}

	return new_element->Node;
}

void
t_delete(TreePath *e) {
	TreeNode *pe;
	if (e) {
		pe = e->Node->Parent;
		t_delete( e->Node->Left );
		t_delete( e->Node->Right );
		free(e->Node);
		free(e);

		pe->Left = 0x00;
		pe->Right= 0x00;
	}
}

