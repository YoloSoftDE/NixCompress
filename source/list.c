/*
 * File:		/src/list.c
 *
 * Project:     nix-compress aka lz1337
 * Author:		Sebastian Büttner
 * Created:		11/22/2012
 *
 */



/* C-Standard Includes */
#include <stdlib.h>

/* Third-party_includes */

/* Cross-project Includes */

/* Project specific Includes */
#include "list.h"

/*---------------------------*/

/*
 * @Summary: Initializes a double-linked list
 * @Returns: Listpointer
 * @Param  : void
 */

List*
l_create() {
	List* lp;													// 4 Bytes, stack
	lp        = (List*)    malloc( sizeof(List) );				// 8 Bytes (List), heap
	lp->Begin = (ListElement*) malloc( sizeof(ListElement) );	// 12 Bytes (ListElement), heap
	lp->End   = (ListElement*) malloc( sizeof(ListElement) );	// 12 Bytes (ListElement), heap
	lp->Begin->Next = lp->End;
	lp->Begin->Prev = NULL;
	lp->End->Prev   = lp->Begin;
	lp->End->Next   = NULL;

	lp->Begin->Data = 0x0;
	lp->End->Data = 0x0;

	return lp;													// = 32 Bytes
}

/*
 * @Summary: Deleted a list structure
 * @Returns: void
 * @Param  : Initialized List-pointer
 */

void
l_delete(List **lp) {
	if (*lp) {
		l_delete_all(*lp);

		free((*lp)->Begin);
		free((*lp)->End);

		free(*lp);

		*lp = NULL;
	}
}

/*
 * @Summary: Insert a initialized element after the named element in list
 * @Returns: void
 * @Param  : Initialized list pointer
 * @Param  : Element to be inserted
 * @Param  : Element to insert after
 */

void
l_insert_after(List *lp, ListElement *e, ListElement *prev_e) {
	ListElement *Prev;		// 4 Bytes, stack
	ListElement *Next;		// 4 Bytes, stack

	Prev = prev_e;
	Next = prev_e->Next;

	e->Prev = Prev;
	e->Next = Next;
	Prev->Next = e;
	Next->Prev = e;
}							// = 0 Bytes

/*
 * @Summary: Append an element to list
 * @Returns: void
 * @Param  : List pointer
 * @Param  : Element to be appended
 */

void
l_append(List *lp, ListElement *e) {
	l_insert_after(lp, e, lp->End->Prev);	//0 Bytes
}

/*
 * @Summary: Prepend an element to list
 * @Returns: void
 * @Param  : List pointer
 * @Param  : Element to be prepended
 */

void
l_prepend(List *lp, ListElement *e) {
	l_insert_after(lp, e, lp->Begin);
}

/*
 * @Summary: Insert Element at certain pos in list
 * @Returns: true if insert worked
 * @Param  : List pointer
 * @Param  : Element to be inserted
 * @Param  : Pos where the element shoudl be inserted
 */

bool
l_insert_pos(List *lp, ListElement *e, size_t _pos) {
	ListElement *ep = lp->Begin;
	size_t i;

	for (i = 0; i < _pos; i++) {
		if (!ep->Next) {
			return false;
		}
		ep = ep->Next;
	}

	l_insert_after(lp, e, ep);
	return true;
}

/*
 * @Summary: Removes an Element from the list
 * @Returns: void
 * @Param  : List pointer
 * @Param  : Element to be removed
 */

void
l_remove(ListElement *e) {
	ListElement *Prev;
	ListElement *Next;

	Prev = e->Prev;
	Next = e->Next;
	Prev->Next = Next;
	Next->Prev = Prev;

	//free( e->Data );
	free ( e );
}

/*
 * @Summary: Removes an Element at a certain pos from list
 * @Returns: void
 * @Param  : List pointer
 * @Param  : Pos of Element to be removed
 */

void
l_remove_pos(List *lp, size_t pos) {
	ListElement *ep;
	size_t i;


	for (i = 0, ep = lp->Begin->Next;
		 ep != lp->End && i < pos;
		 i++, ep = ep->Next) {
	}

	if (i == pos && ep != lp->Begin && ep != lp->End) {
		l_remove(ep);
	}
}

/*
 * @Summary: Checks if an Element exists at certain pos
 * @Returns: true if element exists
 * @Param  : List pointer
 * @Param  : pos to check
 */

bool
l_exists_pos(List *lp, size_t pos) {
	ListElement *ep;
	size_t i;

	ep = lp->Begin->Next;
	for (i = 0; ep != lp->End && i < pos; i++) {
		ep = ep->Next;
	}

	if (i == pos && ep != lp->Begin && ep != lp->End) {
		return true;
	}

	return false;
}

/*
 * @Summary: Get Element at pos
 * @Returns: pointer to Element
 * @Param  : List pointer
 * @Param  : Pos to get
 */

ListElement*
l_get_pos(List *lp, size_t pos) {
	ListElement *ep;
	size_t i;

	for (i = 0, ep = lp->Begin->Next;
		 ep != lp->End && i < pos;
		 i++, ep = ep->Next) {

	}

	if (i == pos && ep != lp->Begin && ep != lp->End) {
		return ep;
	}

	return NULL;
}


ListElement*
l_get_by_data(List *lp, void* data) {
	ListElement *ep;

	ep = lp->Begin->Next;
	while ( ep != lp->End ) {
		if (ep->Data == data) {
			return ep;
		}

		ep = ep->Next;
	}

	return NULL;
}

/*
 * @Summary: Deleted every element in list; cleanup
 * @Returns: void
 * @Param  : List pointer
 */

void
l_delete_all(List *lp) {
	ListElement *ep;
	ListElement *__n_ep;

	for (ep = lp->Begin->Next; ep != lp->End;) {
		__n_ep = ep->Next;
		//free( ep->Data );
		free (ep);
		ep = __n_ep;
	}
	lp->Begin->Next = lp->End;
	lp->End->Prev   = lp->Begin;
}

/*
 * @Summary: Count elements in list
 * @Returns: total count of elements
 * @Param  : List pointer
 */

size_t
l_count(List *lp) {
	size_t c = 0;
	ListElement *ep;

	for (ep = lp->Begin->Next; ep != lp->End; ep = ep->Next) {
		c++;
	}

	return c;
}
