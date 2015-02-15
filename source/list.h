/*
 * File:		/src/list.h
 *
 * Project:     nix-compress aka lz1337
 * Author:		Sebastian Büttner
 * Created:		11/22/2012
 *
 */



#ifndef LIST_H_
#define LIST_H_

/* C-Standard Includes */
#include <string.h>

/* Third-party_includes */

/* Cross-project Includes */

/* Project specific Includes */
#include "bool.h"

/*---------------------------*/

typedef struct ListElement {
	void *Data;					// 4 Bytes
	struct ListElement *Next;	// 4 Bytes
	struct ListElement *Prev;	// 4 Bytes
} ListElement;					// = 12 Bytes

typedef struct List {
	ListElement *Begin;			// 4 Bytes
	ListElement *End;			// 4 Bytes
} List;							// = 8 Bytes

extern List*    l_create();
extern void     l_insert_after(List *lp, ListElement *e, ListElement *prev_e);
extern bool     l_insert_pos(List *lp, ListElement *e, size_t _pos);
extern void     l_prepend(List *lp, ListElement *e);

extern void     l_append(List *lp, ListElement *e);
extern void     l_delete(List **lp);
extern void     l_delete_all(List *lp);
extern void     l_remove(ListElement *e);
extern void     l_remove_pos(List *lp, size_t pos);

extern bool     l_exists_pos(List *lp, size_t pos);
extern ListElement* l_get_pos(List *lp, size_t pos);
extern ListElement* l_get_by_data(List *lp, void* Data);
extern size_t   l_count(List *lp);


#endif /* LIST_H_ */
