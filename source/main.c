/*
 * File:		/src/main.c
 *
 * Project:     nix-compress aka lz1337
 * Author:		Sebastian Büttner
 * Created:		11/27/2012
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
#include "archive.h"
#include "args.h"
#include "bool.h"
#include "huffman.h"

/*---------------------------*/

int main(int argc, char **argv) {
	CliArguments args;

	printf("nix-compress (c) 2012-2013 by Florian Zorbach and Sebastian Büttner.\n\n");

	// Parse the Arguments
	parse_args(argc, argv, &args);

	// Handle Parameters
	switch (args.Mode) {

		//Pack an archive
		case MODE_PACK:
			//check if a archive name is given
			if (args.archivefile == NULL) {
				printf("No archive filename given.\n");
				return EXIT_FAILURE;
			}

			//Check if there are files to pack
			if (args.files == NULL || args.filesc == 0) {
				printf("Nothing to add.\n");
				return EXIT_FAILURE;
			}

			//pack archive
			printf("Packing archive %s\n", args.archivefile);
			archive_pack((const char**) args.files, args.filesc, args.archivefile, args.Verbose);

			break;

		//Unpack archive
		case MODE_UNPACK:

			//check if a archive name is given
			if (args.archivefile == NULL) {
				printf("No archive filename given.\n");
				return EXIT_FAILURE;
			}

			printf("Unpacking archive %s\n", args.archivefile);

			//check if a target is given
			if (args.targetpath == NULL) {
				//unpack to current directory
				archive_unpack(args.archivefile, "./", args.Verbose);
			} else {
				//unpack to specific target
				archive_unpack(args.archivefile, args.targetpath, args.Verbose);
			}

			break;

		//list files in archive
		case MODE_LIST:
			archive_show_filetable(args.archivefile);

			break;
		default:
			printf("Unknown action.\n");
		//Show help
		case MODE_HELP:
			printf("\n");
			printf("nix-compress -c [-a] archive.nix file1 [file2 [...]]\n");
			printf("nix-compress -x [-a] archive.nix [targetpath]\n");
			printf("\n");
			printf("  -a, --archive-file    archive file\n");
			printf("  -c, --compress        pack archive\n");
			printf("  -l, --list            only list archive contents\n");
			printf("  -x, --extract         unpack archive\n");
			printf("  -v, --verbose         verbose output\n");
			printf("  -h, --help            shows this help message\n");
			return EXIT_FAILURE;
			break;
	}

	return EXIT_SUCCESS;
}
