/*
 * File:		/src/args.h
 *
 * Project:     nix-compress aka lz1337
 * Author:		Sebastian Büttner
 * Created:		01/04/2012
 *
 */


#ifndef ARGS_H_
#define ARGS_H_

/* C-Standard Includes */
#include <stdio.h>

/* Third-party_includes */

/* Cross-project Includes */

/* Project specific Includes */
#include "structs.h"
#include "bool.h"

/*---------------------------*/

typedef enum mode {MODE_UNSET, MODE_PACK, MODE_UNPACK, MODE_LIST, MODE_HELP} mode;

typedef struct CliArguments {
	
	bool   Verbose;
	mode   Mode;
	char  *archivefile;
	char **files;
	int    filesc;
	char  *targetpath;
} CliArguments;

bool parse_args(int argc, char **argv, CliArguments *args);

#endif /* ARGS_H_ */
