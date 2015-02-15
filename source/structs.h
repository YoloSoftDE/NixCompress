/*
 * File:		/src/srcuts.h
 *
 * Project:     nix-compress aka lz1337
 * Author:		Sebastian Büttner
 * Created:		11/26/2012
 *
 */


#ifndef STRUCTS_H_
#define STRUCTS_H_

/* Includes*/

// c-standard lib includes
#include <stdlib.h>
#include <stdint.h>

// third-party includes

// cross-project includes

// project-specific includes
#include "bool.h"


/* String with prepended length info */

typedef struct FT_String {
	uint16_t  len;
	char      *str;
} FT_String;


/* File Header */

typedef struct Header {
	uint32_t  magic;
	uint8_t   version;
	uint32_t  offset_filetable;
	uint32_t  ft_entry_count;
} Header;


/* FileTable */

typedef struct FT_FileEntry {
	FT_String path;
	size_t    size_compressed;
	size_t    size_original;
	uint32_t  offset_container;
	uint32_t  crc32;
} FT_FileEntry;


/* Countainer */

typedef struct CT_Entry {
	uint8_t  original;
	uint8_t  length_replacement;
	uint32_t replacement;
} CT_Entry;

typedef struct CodeTable {
	uint16_t  Length;
	CT_Entry  **codetable;
} CodeTable;

typedef struct Container {
	uint16_t  size_codetable;
	CT_Entry  **codetable;
	char      *encoded;
} Container;

//helper

typedef struct CodeTable_indexed {
	CodeTable Save;
	CT_Entry  *Index[256];
} CodeTable_indexed;

typedef struct WriteHelper {
    long long int read:8;
    long long int reserved:56;
} WriteHelper;

typedef union WriteHelper2 {
	char read[8];
} WriteHelper2;

/* CompressedFile */

typedef struct CompressedFile {
	Header       *header;
	FT_FileEntry **filetable;
	Container    **data;
} CompressedFile;


#endif /* STRUCTS_H_ */
