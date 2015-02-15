/*
 * File:		/src/args.c
 *
 * Project:     nix-compress aka lz1337
 * Author:		Sebastian Büttner
 * Created:		01/04/2013
 *
 */


/* Includes*/

// c-standard lib includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// third-party includes

// cross-project includes

// project-specific includes
#include "args.h"

/*---------------------------*/

bool
parse_args(int argc, char **argv, CliArguments *args) {
	int   i, j;
	char *argument;
	char shortarg;
	size_t arglen;

	args->Mode        = MODE_UNSET;
	args->Verbose     = false;
	args->archivefile = NULL;
	args->files       = NULL;
	args->targetpath  = NULL;
	args->filesc      = 0;

	for (i = 1; i < argc; i++) {
		argument = argv[i];

		if (strncmp(argument, "--", 2) == 0) {
			argument += 2;
			arglen = strlen(argument);

			if (strncmp( argument, "compress", arglen ) == 0) {

				if (args->Mode != MODE_UNSET) {
					printf("Multiple operation modes set. Aborting.\n");
					return false;
				}
				args->Mode = MODE_PACK;
			} else if (strncmp( argument, "extract", arglen ) == 0) {

				if (args->Mode != MODE_UNSET) {
					printf("Multiple operation modes set. Aborting.\n");
					return false;
				}
				args->Mode = MODE_UNPACK;
			} else if (strncmp( argument, "list", arglen ) == 0) {

				if (args->Mode != MODE_UNSET) {
					printf("Multiple operation modes set. Aborting.\n");
					return false;
				}
				args->Mode = MODE_LIST;
			} else if (strncmp( argument, "archive-file", arglen ) == 0) {
					if (++i >= argc) {

						printf("No archivename given.\n");
						return false;
					}

					args->archivefile = argv[i];
			} else if (strncmp( argument, "help", arglen ) == 0) {
					args->Mode != MODE_HELP;
			} else if (strncmp( argument, "verbose", arglen ) == 0) {
				args->Verbose = true;
			}
		} else if (strncmp(argument, "-", 1) == 0) {
			argument++;
			for (j = 0; j < strlen(argument); j++) {
				shortarg = argument[j];

				switch (shortarg) {
				//Create archive
				case 'c':

					if (args->Mode != MODE_UNSET) {
						printf("Multiple operation modes set. Aborting.\n");
						return false;
					}
					args->Mode = MODE_PACK;
				break;
				//Extract archive
				case 'x':

					if (args->Mode != MODE_UNSET) {
						printf("Multiple operation modes set. Aborting.\n");
						return false;
					}
					args->Mode = MODE_UNPACK;
				break;
				//List archive
				case 'l':

					if (args->Mode != MODE_UNSET) {
						printf("Multiple operation modes set. Aborting.\n");
						return false;
					}
					args->Mode = MODE_LIST;
				break;
				//Verbose mode
				case 'v':

					args->Verbose = true;
				break;
				//help mode
				case 'h':

					args->Mode = MODE_HELP;
				break;
				//Archive name
				case 'a':

					if (j != strlen(argument) -1) {

						printf("Invalid parameter `archive-file`. Not allowed here.\n");
						return false;
					}

					if (++i >= argc) {

						printf("No archivename given.\n");
						return false;
					}

					args->archivefile = argv[i];
				break;
				}
			}
		} else {
			/*
			 * nix-compress -c|x archive.nix file1[|target] [file2]
			 */

			if (args->archivefile == NULL) {
				args->archivefile = argument;
			} else {

				switch (args->Mode) {
				case MODE_UNPACK:

					if (args->targetpath != NULL) {

						printf("You can only specify one target path to unpack to!\n");
						return false;
					}
					args->targetpath = argument;
					break;
				case MODE_PACK:

					args->filesc++;
					args->files = (char**)realloc(args->files, args->filesc * sizeof(char*));
					args->files[ args->filesc -1 ] = argument;
					break;
				default:
					break;
				}
			}
		}
	}

	return true;
}

