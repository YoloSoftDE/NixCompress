/*
* File:     /src/huffman.c
*
* Project:     nix-compress aka lz1337
* Author:       Sebastian Büttner
* Created:      11/28/2012
*
*/


/* C-Standard Includes */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* Third-party_includes */

/* Cross-project Includes */

/* Project specific Includes */
#include "structs.h"
#include "huffman.h"
#include "bin-tree.h"

//#define VISUALIZATION

#if VISUALIZATION
#include "simple_draw.h"
#endif

/*---------------------------*/

#define VIS_PIX_PER_LEVEL 30
#define VIS_PIX_SPACING   600

void
huffman_readout_r(const TreeNode *Node, CodeTable_indexed *Table, CT_Entry *Entry);

/*---------------------------*/

/*
* @Summary: Convert
* @Returns:
* @Param  :
*/
char *
int2bin(unsigned long long int a, int buf_size)
{
    char *buffer;
    int i;


    buffer = (char *)calloc(1, buf_size + 1);

    buffer += (buf_size);

    for ( i = buf_size - 1; i >= 0; i--) {
        *(--buffer) = (a & 1) + '0';

        a >>= 1;
    }

    return buffer;
}

/*
* @Summary: Find the two entrys with the lowest weight and attach them as subnode to the tree
* @Returns: void
* @Param  :
*       List    *Table: A List of Trees
*
*/
void
huffman_transform_minimals(List *Table)
{
    TreePath *Path, *Minimals = t_create(0);
    ListElement *e;

    Minimals->Node->Right = (TreePath *)Table->Begin->Next->Data;

    for (e = Table->Begin->Next->Next; e != Table->End; e = e->Next) {
        Path = (TreePath *)e->Data;
        if (Minimals->Node->Right == 0x00 ||
            Path->Weight <= Minimals->Node->Right->Weight) {
            Minimals->Node->Left  = Minimals->Node->Right;
            Minimals->Node->Right = Path;
        } else if (Minimals->Node->Left == 0x00 ||
                   Path->Weight <= Minimals->Node->Left->Weight) {
            Minimals->Node->Left  = Path;
        }
    }
    l_remove(l_get_by_data(Table, Minimals->Node->Left));
    l_remove(l_get_by_data(Table, Minimals->Node->Right));

    Minimals->Node->Left->Node->Parent  = Minimals->Node;
    Minimals->Node->Right->Node->Parent = Minimals->Node;
    Minimals->Weight = Minimals->Node->Left->Weight + Minimals->Node->Right->Weight;

    e = (ListElement *)calloc(1, sizeof(ListElement));
    e->Data = Minimals;

    l_append(Table, e);

    return;
}


/*
* @Summary: Creates a forest based on the calculated weights
* @Returns: A List of Trees (Forest)
* @Param  :
*       uint32_t    *Weights:
*       size_t      *count  : Number of Trees in Forest
*/
List *
huffman_create(uint32_t *Weights, size_t *count)
{
    TreePath *Tree;
    int      index;
    List     *TreeList;
    ListElement *new_element;

    // Initialize Counter
    *count = 0;

    // Create List
    TreeList = l_create();

    // Loop through all possible bytes
    for (index = 0; index < 256; index++) {

        // If byte appears in the file
        if (Weights[index] != 0) {
            // Create a BinaryTree & Copy byte into Tree
            Tree             = t_create( Weights[ index ] );
            Tree->Node->Data = (char *)calloc(1, sizeof(char));
            memcpy(Tree->Node->Data, &index, sizeof(char));

            // Add Tree to Forest
            new_element      = (ListElement *)calloc(1, sizeof(ListElement));
            new_element->Data = Tree;
            l_append(TreeList, new_element);

            // Increase TreeCounter
            (*count)++;
        }
    }
    return TreeList;
}

/*
* @Summary: Transform the Huffman-Forest into the Huffman-Tree
* @Returns: The final Huffman-Tree
* @Param  :
*       List    *Table: One Forest
*/
TreePath *
huffman_transform(List *Table)
{
    TreePath *Tree;

    // While more than one tree in the forest
    while (l_count(Table) > 1) {
        huffman_transform_minimals(Table);
    }

    // Select remaining tree from forest
    Tree = (TreePath *)Table->Begin->Next->Data;

    // Remove Forest - Greenpeace is sad now
    l_delete(&Table);

    // Return lonely tree
    return Tree;
}


/*
* @Summary: Readout the HuffmanTree and
* @Returns:
* @Param  :
*       List    *Table: One Forest
*/
CodeTable_indexed *
huffman_readout(const TreePath *Tree, size_t count)
{
    CT_Entry    *ct_entry = (CT_Entry *)calloc( 1, sizeof(CT_Entry));
    CodeTable_indexed *Table;

    Table = (CodeTable_indexed *)calloc( 1, sizeof(CodeTable_indexed) );
    Table->Save.codetable = (CT_Entry **)calloc( count, sizeof(CT_Entry *) );
    Table->Save.Length    = 0;

    huffman_readout_r(Tree->Node, Table, ct_entry);

    return Table;
}


/*
* @Summary: Readout the huffman tree recursive and insert it into a table
* @Returns: void
* @Param  :
*       TreeNode            *Node   : The Source Binary-Tree
*       CodeTable_indexed   *Table  : The Target Table
*       CT_Entry            *Entry  : The current CT_Entry passed in each recursion until a TreeNode has no childs
*/
void
huffman_readout_r(const TreeNode *Node, CodeTable_indexed *Table, CT_Entry *Entry)
{
    uint8_t index;

    if (Node->Left != NULL) {
        Entry->length_replacement++;
        Entry->replacement <<= 1;
        Entry->replacement |= 0;
        huffman_readout_r(Node->Left->Node, Table, Entry);
    }
    if (Node->Right != NULL) {
        Entry->length_replacement++;
        Entry->replacement <<= 1;
        Entry->replacement |= 1;
        huffman_readout_r(Node->Right->Node, Table, Entry);
    }
    if (Node->Data != NULL) {
        index = (*((char *)Node->Data));

        Table->Index[index] = (CT_Entry *)calloc(1, sizeof(CT_Entry));

        Table->Index[index]->original           = index;
        Table->Index[index]->length_replacement = Entry->length_replacement;
        Table->Index[index]->replacement        = Entry->replacement;

        Table->Save.codetable[Table->Save.Length] = Table->Index[index];
        Table->Save.Length++;
    }
    Entry->length_replacement--;
    Entry->replacement >>= 1;
}

/*
* @Summary: Calculate Weights (Count each byte in file)
* @Returns: Array of Weights
* @Param  :
*       FILE    *fp   : The file
*       int     *total: The Number of different bytes in the file (Number of entries in Weight-Array)
*/
uint32_t *
huffman_calc_weight(FILE *fp, int *total)
{
    uint8_t buffer[512];
    int i;
    int real_buffer_size;
    uint32_t *Weights;

    Weights = (uint32_t *)calloc(sizeof(uint32_t), 256);

    if (fp != NULL) {
        fseek(fp, 0, SEEK_SET);
        while (!feof(fp)) {
            if ((real_buffer_size = fread(buffer, 1, sizeof(buffer), fp)) != 0) {

                for (i = 0; i < real_buffer_size; i++) {
                    Weights[buffer[i]]++;
                }

                *total += real_buffer_size;
            }
        }
        fseek(fp, 0, SEEK_SET);
    }

    return Weights;
}

/*
* @Summary: Create the whole huffman thing
* @Returns: A code-table based on the generated huffmantree
* @Param  :
*       uint32_t *Weights: The calculated weights
*/
CodeTable_indexed *
huffman(uint32_t *Weights)
{
    size_t            count;

    List              *wood;
    TreePath          *huffmantree;
    CodeTable_indexed *codetable;

    CT_Entry           Entry;

    // Initialize the Huffman-Binary-Tree
    wood        = huffman_create(Weights, &count);

    // Generate the Huffman-Binary-Tree
    huffmantree = huffman_transform(wood);

    // Read out data and store in a table
    codetable   = huffman_readout(huffmantree, count);

#if VISUALIZATION
    // Visualize Binary Tree
    ClearGraphic();
    huffman_visualize(huffmantree->Node, Weights, 1, VIS_PIX_SPACING, &Entry);
#endif

    return codetable;
}

/*
* @Summary: Load HuffmanTree from CodeTable (stored in the compressed file)
* @Returns: The Huffman-Binary-Tree
* @Param  :
*       CodeTable   *Table: The CodeTable of the compressed file
*/
TreePath *
huffman_load(CodeTable *Table)
{
    TreePath *Tree;
    TreePath *CurrentPath;
    CT_Entry *CurrentEntry;
    int i, j;

    Tree = t_create(0);

    for (i = 0; i < Table->Length; i++) {
        CurrentPath  = Tree;
        CurrentEntry = Table->codetable[i];

        for (j = CurrentEntry->length_replacement - 1; j >= 0; j--) {
            if ((CurrentEntry->replacement >> j & 0x1) == 0) {
                //LEFT NODE
                if (CurrentPath->Node->Left == NULL) {
                    CurrentPath->Node->Left = t_create(0);
                }
                CurrentPath = CurrentPath->Node->Left;
            } else {
                //RIGHT NODE
                if (CurrentPath->Node->Right == NULL) {
                    CurrentPath->Node->Right = t_create(0);
                }
                CurrentPath = CurrentPath->Node->Right;

            }
        }
        //Save char as data of node
        CurrentPath->Node->Data = &CurrentEntry->original;
    }


    return Tree;
}

/*
* @Summary: Debug-Method to print the CodeTable
* @Returns: void
* @Param  :
*       CodeTable   *Table: The CodeTable to print
*/
void
huffman_print_codetable(CodeTable *Table)
{
    int i;

    printf("------------------------------------------------------------\n");
    for (i = 0; i < Table->Length; i++) {
        printf("%#X (%s) -> %0#X (%s)\n", Table->codetable[i]->original,
               int2bin(Table->codetable[i]->original, 8),
               Table->codetable[i]->replacement,
               int2bin(Table->codetable[i]->replacement, Table->codetable[i]->length_replacement));
    }
}

/*
* @Summary: Debug-Method to print the Weight-Array
* @Returns: void
* @Param  :
    uint32_t    *Weights: The Weight-Array to print
*/
void
huffman_print_weights(uint32_t *Weights)
{
    int i;

    printf("------------------------------------------------------------\n");
    for (i = 0; i < 256; i++) {
        if (Weights[i] > 0) {
            printf("%#X -> %u\n", i,
                   Weights[i]);
        }
    }
}

#if VISUALIZATION
/*
* @Summary: Debug-Method to visualize a Binary-Tree using SimpleDraw
* @Returns: void
* @Param  :
    TreeNode    *Node   : TreeNode to start displaying
    uint32_t    *Weights: The Weight-Array for displaying additional information
    int         level   :
    int         lastx   :
    CT_Entry    *Entry  :

*/
void huffman_visualize(const TreeNode *Node, const uint32_t *Weights, int level, int lastx, CT_Entry *Entry)
{
    uint8_t index;

    char buf[20] = {0};

    int basePosX;
    int basePosY;

    if (level == 1)
        MoveTo(lastx, 0);

    basePosY = level * VIS_PIX_PER_LEVEL;

    if (Node->Left != NULL) {
        Entry->length_replacement++;
        Entry->replacement <<= 1;
        Entry->replacement |= 0;

        basePosX = lastx - (VIS_PIX_SPACING / pow(2, level));

        // Draw Line
        DrawTo(basePosX, basePosY);
        MoveTo(basePosX, basePosY);

        huffman_visualize(Node->Left->Node, Weights, level + 1, basePosX, Entry);

    }
    MoveTo(lastx, basePosY - VIS_PIX_PER_LEVEL);

    if (Node->Right != NULL) {
        Entry->length_replacement++;
        Entry->replacement <<= 1;
        Entry->replacement |= 1;

        basePosX = lastx + (VIS_PIX_SPACING / pow(2, level));

        // Draw Line
        DrawTo(basePosX, basePosY);
        MoveTo(basePosX, basePosY);

        huffman_visualize(Node->Right->Node, Weights, level + 1, basePosX, Entry);
    }
    if (Node->Data != NULL) {
        buf[0] = (*((char *)Node->Data));
        buf[1] = 0;
        PlaceText(lastx, basePosY - VIS_PIX_PER_LEVEL, buf);

        snprintf(buf, 10, "%10d", Weights[(*((char *)Node->Data))]);

        PlaceText(lastx, basePosY - VIS_PIX_PER_LEVEL + 20, buf);
    }
    Entry->length_replacement--;
    Entry->replacement >>= 1;
}
#endif