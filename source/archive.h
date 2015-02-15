/*
 * File:		/src/archive.c
 *
 * Project:     nix-compress aka lz1337
 * Author:		Florian Zorbach
 * Created:		12/18/2012
 *
 */

#ifndef ARCHIVE_H_
#define ARCHIVE_H_

/* C-Standard Includes */
#include <stdio.h>

/* Third-party_includes */

/* Cross-project Includes */

/* Project specific Includes */
#include "structs.h"
#include "bool.h"

/*---------------------------*/

typedef enum  {
	E_SUCCESS,
	E_UNABLE_TO_DEFLATE_FILE,
	E_UNABLE_TO_INFALTE_FILE,
	E_TARGET_FILE_ALREADY_EXISTS,
	E_COULD_NOT_OPEN_FILE,
	E_INVALID_ARCHIVE_HEADER,
	E_INVALID_FILE_MAGIC,
	E_INCOMPLETE_OR_INVALID_FILETABLE,
	E_COULD_NOT_CREATE_PATH,
	E_TARGET_DIRECTORY_DOES_NOT_EXISTS,
	E_INVALID_CODETABLE,
	E_CODETABLE_ZEROSIZED,
	E_CRC32_CHECK_FAILED
} ReturnCode;

extern ReturnCode archive_pack(const char **files, size_t filecount, const char* archivename, bool verbose);
extern ReturnCode archive_unpack(char *archive, char* destination, bool verbose);
extern ReturnCode archive_show_filetable(char *archive);

extern ReturnCode compress(FILE *file, const char *filename, size_t *size_compressed, size_t *size_orginal, uint32_t *crc32);
extern ReturnCode uncompress(FILE *file, const char *outfilename, size_t *length, uint32_t *crc32);

#endif /* ARCHIVE_H_ */
