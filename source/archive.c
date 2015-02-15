/*
 * File:		/src/archive.c
 *
 * Project:     nix-compress aka lz1337
 * Author:		Florian Zorbach
 * Created:		12/18/2012
 *
 */

#define CRC32POLY 0x04C11DB7
#ifdef _WIN32
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

/* C-Standard Includes */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>

#ifdef _WIN32
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#endif

/* Third-party_includes */

/* Cross-project Includes */

/* Project specific Includes */
#include "archive.h"
#include "structs.h"
#include "huffman.h"

/*---------------------------*/

bool archive_readheader (Header *header, FILE *file);
bool archive_readfiletable(FT_FileEntry *filetable, uint32_t offset_filetable, size_t filecount, FILE *file);
char* create_path(const char* Base, char *Path);
char* path_remove_device(char *Base);
void IterateCRC32(uint32_t *crc32, uint8_t data);
char *str_replace(char *search , char *replace , char *subject);
char* path_transform_to_nix(char *path);
char* path_transform_from_nix(char *path);
char* human_readable_size(size_t size);

ReturnCode
archive_pack(const char **files, size_t filecount, const char* archivename, bool verbose)
{
    FILE *file;
    Header header;
    FT_FileEntry *filetable;
    FT_FileEntry *entry;
    size_t i;
    size_t filepointer;

    // Open File
    if ((file = fopen(archivename, "w+b")) == NULL)
    {
            return E_COULD_NOT_OPEN_FILE;
    }

    // Create FileHeader
    header.magic = 0x6E616366; // ('ncaf')
    header.version = 1;
    header.ft_entry_count = filecount;
    header.offset_filetable = 0;

    fwrite(&header, sizeof(Header), 1, file);
    fflush(file);

    // Create FileTable (StructSize * FileCount)
    filetable = (FT_FileEntry*)calloc(filecount, sizeof(FT_FileEntry));

    // Foreach File
    for (i = 0; i < filecount; i++) {
            // Compress File to Archive
            entry = &filetable[i];
            entry->path.str = (char *)files[i];
            entry->path.len = strlen(files[i]);
            entry->offset_container = ftell(file);

            if (verbose) {
            	printf("Inflating `%s` ", entry->path.str);
                fflush(stdout);
            }

            entry->path.str = path_transform_to_nix(entry->path.str);

            filepointer = ftell(file);
            if ( compress(file, files[i], &entry->size_compressed, &entry->size_original, &entry->crc32) != E_SUCCESS) {
                    //Reset previous state
                    filecount--;
                    i--;

                    //Resize the array and "reset" the current entry
                    filetable = (FT_FileEntry*)realloc(filetable, sizeof(FT_FileEntry) * filecount);
                    memset(entry, 0, sizeof(FT_FileEntry));

                    //Reset filepointer to previous position
                    fseek(file, filepointer, SEEK_SET);
					#ifdef _WIN32
						_chsize(file, filepointer);
					#else
						ftruncate(fileno(file), filepointer);
					#endif

			        if (verbose) {
			        	printf("...failed\n");
			        }
            } else {
                if (verbose) {
                	printf(" (inflated %.2f%%)\n", (1 - ( (double)(ftell(file) - filepointer) / (double)entry->size_original ) )*100 );
                }
            }
    } // End Foreach

    // Write FileTable
    header.offset_filetable = ftell(file);
    header.ft_entry_count   = filecount;

    for (i = 0; i < filecount; i++) {
            entry = &filetable[i];
            fwrite(&entry->path.len, sizeof(uint16_t), 1, file);
            fwrite(entry->path.str, 1, entry->path.len, file);
            fwrite(&entry->size_compressed, sizeof(size_t), 1, file);
            fwrite(&entry->size_original, sizeof(size_t), 1, file);
            fwrite(&entry->offset_container, sizeof(uint32_t), 1, file);
            fwrite(&entry->crc32, sizeof(uint32_t), 1, file);
            fflush(file);
    }
    // See archive_readfiletable

    //Maybe correct filecount in header
    fseek(file, 0, SEEK_SET);

    fwrite(&header.magic, sizeof(uint32_t), 1, file);
    fwrite(&header.version, sizeof(uint8_t), 1, file);
    fwrite(&header.ft_entry_count, sizeof(uint32_t), 1, file);
    fwrite(&header.offset_filetable, sizeof(uint32_t), 1, file);
    fflush(file);

    return E_SUCCESS;
}

ReturnCode
archive_unpack(char *archive, char* destination, bool verbose)
{
    FILE *file;
    Header header;
    FT_FileEntry *filetable;
    FT_FileEntry *entry;
    size_t i;
    char *TargetPath;

    if ((file = fopen(archive, "rb")) == NULL)
    {
            // Open File failed
            return E_COULD_NOT_OPEN_FILE;
    }

    //Read File Header
    if (!archive_readheader(&header, file)) {
            return E_INVALID_ARCHIVE_HEADER;
    }

    if (!header.magic == 0x6E616366) { // ('ncaf')
            return E_INVALID_FILE_MAGIC;
    }

    //Read File Table
    filetable = (FT_FileEntry*)calloc( header.ft_entry_count, sizeof(FT_FileEntry));
    if (! archive_readfiletable(filetable, header.offset_filetable, header.ft_entry_count, file)) {
            return E_INCOMPLETE_OR_INVALID_FILETABLE;
    }

    //Begin to uncompress the files
    for (i = 0; i < header.ft_entry_count; i++) {
            entry = &filetable[i];

            if (verbose) {
            	printf("Deflating `%s`\n", entry->path.str);
            	fflush(stdout);
            }

            entry->path.str = path_transform_from_nix(entry->path.str);

            if ((TargetPath = create_path(destination, entry->path.str)) != NULL) {

                    //seek to container position offset
                    fseek(file, entry->offset_container, SEEK_SET);

                    if ( uncompress(file, TargetPath, &entry->size_original, &entry->crc32) != E_SUCCESS ) {

                        if (verbose) {
                        	printf("Warning: %s could not be deflated.\n", entry->path.str);
                        }
                    }
            } else {

                if (verbose) {
                	printf("Could not create path\n");
                }
            }
            free(TargetPath);
    }

    fclose(file);

    return E_SUCCESS;
}

ReturnCode
archive_show_filetable(char *archive) {
	FILE *file;
	Header header;
	FT_FileEntry *filetable;
	FT_FileEntry *entry;
	size_t i;
	char *TargetPath;

	if ((file = fopen(archive, "rb")) == NULL)
	{
			// Open File failed
			return E_COULD_NOT_OPEN_FILE;
	}

	//Read File Header
	if (!archive_readheader(&header, file)) {
			return E_INVALID_ARCHIVE_HEADER;
	}

	if (!header.magic == 0x6E616366) { // ('ncaf')
			return E_INVALID_FILE_MAGIC;
	}

	printf("Formatversion: %2u\n"
		   "Filecount:    %3u\n\n"
		   "--------------------------------------------------------------------------------\n"
		   "  # | path                                     |   size   |  comp.  | checksum\n"
		   "--------------------------------------------------------------------------------\n",
		   header.version,
		   header.ft_entry_count);

	//Read File Table
	filetable = (FT_FileEntry*)calloc( header.ft_entry_count, sizeof(FT_FileEntry));
	if (! archive_readfiletable(filetable, header.offset_filetable, header.ft_entry_count, file)) {
			return E_INCOMPLETE_OR_INVALID_FILETABLE;
	}

	//Show the files
	for (i = 0; i < header.ft_entry_count; i++) {
		entry = &filetable[i];

		printf("%3u | %40s | %8s | %6.2f%% | %#8X \n",
				   i,
				   strlen(entry->path.str) <= 40 ? entry->path.str : (entry->path.str + (strlen(entry->path.str) - 40)),
				   human_readable_size(entry->size_original),
				   (1- ((double)entry->size_compressed/(double)entry->size_original))*100,
				   entry->crc32);
	}

	fclose(file);

	return E_SUCCESS;
}

char*
create_path(const char *Base, char *Path) {
        #ifdef _WIN32
                struct _stat st;
        #else
                struct stat st;
        #endif
        char *BasePath;
        char *PathPart;
        char *NextPart;
        char *FullPath;
        int i;

        BasePath = (char*)calloc(1, strlen(Base)+1);
        strncpy(BasePath, Base, strlen(Base));

        //Trim the last PathSeparator
        if (BasePath[strlen(Base)-1] == PATH_SEPARATOR[0]) {
                BasePath[strlen(Base)-1] = '\0';
        }

        //Check if the Basedir exists
        #ifdef _WIN32
                if ((i = _stat(BasePath, &st)) != 0) {
                        return NULL;
                }
                #else
                if ((i = stat(BasePath, &st)) != 0) {
                        return NULL;
                }
        #endif

        //Create the full sized PathString (make sure it has always 0 in it, so we do not have to insert them on our own every time...)
        FullPath = (char*)calloc(1, strlen(Base) + strlen(Path) + 1);

        //Copy Basestring
        strncpy(FullPath, Base, strlen(Base));

        //Create
        PathPart = strtok(Path, PATH_SEPARATOR);
        NextPart = strtok(NULL, PATH_SEPARATOR);

        //printf("Path: %s\nNextPath: %s\n", PathPart, NextPart);
        while (NextPart != NULL) {
                //Append the current PathPart to FullPath
                //We do not need to set a zero at the end as there is already one due to calloc allocation...
                strncpy(FullPath + strlen(FullPath), PathPart, strlen(PathPart));
                FullPath[ strlen(FullPath)  ] = PATH_SEPARATOR[0];

                #ifdef _WIN32
                        _mkdir(FullPath);
                #else
                        mkdir(FullPath, 0750); // Octal representation!!!
                #endif

                PathPart = NextPart;

                //Get next PathPart
                NextPart = strtok(NULL, PATH_SEPARATOR);
        }

        FullPath[strlen(FullPath)-1] = '\0';

		#ifdef _WIN32
        		if (_stat(FullPath, &st) != 0) {
                        //Directroy does not exist or is inaccessible
                        return NULL;
                }
	   	#else
        		if (stat(FullPath, &st) != 0) {
						//Directroy does not exist or is inaccessible
						return NULL;
				}
	    #endif


        //We did it!
        FullPath[strlen(FullPath)] = PATH_SEPARATOR[0];
        strncpy(FullPath + strlen(FullPath), PathPart, strlen(PathPart));

        return FullPath;
}

char*
path_remove_device(char *Base) {
	if (strncmp((char*) Base+1, ":\\", 2) == 0) {
			return Base + 3;
	}
	return Base;
}


bool
archive_readfiletable(FT_FileEntry *filetable, uint32_t offset_filetable, size_t filecount, FILE *file) {
	size_t i;
	FT_FileEntry *entry;

	if (file != NULL) {
		fseek(file, offset_filetable, SEEK_SET);

		for (i = 0; i < filecount; i++) {
			entry = &filetable[i];
			fread(&entry->path.len, sizeof(uint16_t), 1, file);
			entry->path.str = (char*)calloc( 1, entry->path.len + 1 );
			fread(entry->path.str, 1, entry->path.len, file); //malloc!
			fread(&entry->size_compressed, sizeof(size_t), 1, file);
			fread(&entry->size_original, sizeof(size_t), 1, file);
			fread(&entry->offset_container, sizeof(uint32_t), 1, file);
			fread(&entry->crc32, sizeof(uint32_t), 1, file);

		}

		return true;
	}
	return false;
}

bool
archive_readheader (Header *header, FILE *file) {
	fseek(file, 0, SEEK_SET);

	fread(&header->magic, sizeof(uint32_t), 1, file);
	fread(&header->version, sizeof(uint8_t), 1, file);
	fread(&header->ft_entry_count, sizeof(uint32_t), 1, file);
	fread(&header->offset_filetable, sizeof(uint32_t), 1, file);

	return true;
}

char*
path_transform_to_nix(char *path) {
	//only if we are on windows
	#ifdef _WIN32
		char *NixPath;

		//replace all windows style path elements to unix styles
		NixPath = str_replace("\\", "/", path);

		//remove windows device
		return path_remove_device(NixPath);

	#else
		return path;
	#endif
}

char*
path_transform_from_nix(char *path) {

	//only if we are on windows
	#ifdef _WIN32

		//replace all unix style path elements with windows ones
		return str_replace("/", "\\", path);
	#else
		return path;
	#endif
}


// *file => ArchiveFile to write to
ReturnCode
compress(FILE *file, const char *filename, size_t *size_compressed, size_t *size_orginal, uint32_t *crc32)
{
    CodeTable_indexed  *codetable;
    uint32_t *weights;
    FILE  *fp;                      // File to compress
    CT_Entry *entry;
    WriteHelper2 *helper;
    int bytecount = 0;
    int i = 0;
    int real_buffer_size = 0;
    int count = 0;
    uint8_t readbuffer[512];
    uint64_t writebuffer = 0;


    helper = (WriteHelper2*)&writebuffer;
    *crc32 = 0;

    // Open File
    if ((fp = fopen(filename, "rb")) == NULL )
    {
            return E_COULD_NOT_OPEN_FILE;
    }
    fseek(fp, 0, SEEK_SET);

    // Create Huffman
    weights = huffman_calc_weight(fp, &bytecount);

    codetable = huffman(weights);

    fwrite(&(codetable->Save.Length), 2, 1, file);
    // Write CodeTable
    for (i = 0; i < codetable->Save.Length; i++)
    {
    	entry = codetable->Save.codetable[i];
    	fwrite((void*)&(entry->original), sizeof(uint8_t), 1, file);
    	fwrite((void*)&(entry->length_replacement), sizeof(uint8_t), 1, file);
    	fwrite((void*)&(entry->replacement), sizeof(uint32_t), 1, file);
    }

    *size_compressed = 0;
    *size_orginal    = 0;

    // Compress File
    while (!feof(fp))
    {
            if ((real_buffer_size = fread(readbuffer, 1, sizeof(readbuffer), fp)) != 0)
            {
                    *size_orginal += real_buffer_size;
                    for (i = 0; i < real_buffer_size; i++)
                    {
                            // Write Replacement to buffer
                            if ((entry = codetable->Index[readbuffer[i]]) == NULL) {
                                  return E_INVALID_CODETABLE;
                            }

                            IterateCRC32(crc32, readbuffer[i]);

                            writebuffer = ((uint64_t)entry->replacement << (64 - count - entry->length_replacement)) | writebuffer;
                            count += entry->length_replacement;
                            while (count >= 8 || (i+1 == real_buffer_size && feof(fp) && count > 0))
                            {
                                    (*size_compressed)++;

                                    // Write to Outfile
                                    fputc(helper->read[7], file);

                                    // Remove Byte from Buffer (Shift-left)
                                    writebuffer = (writebuffer << 8);
                                    count -= 8;
                            } // End While (count >= 8)
                    } // End For(each byte)
            } // End If
    } // End While(!eof)

    fclose(fp);
    fflush(file);

    return E_SUCCESS;
} // End Method

ReturnCode
uncompress(FILE *file, const char *outfilename, size_t *length, uint32_t *crc32_expect)
{
    FILE *fp;

    int i;
    uint64_t buffer = 0;
    int count = 0;
    int write_count = 0;
    uint32_t crc32;
    CT_Entry *entry;

    CodeTable table;
    TreePath *tree, *currentPath;
    fread(&(table.Length), 2, 1, file);
    table.codetable = (CT_Entry**)calloc(table.Length, sizeof(CT_Entry*));

    crc32 = 0;

    // Load Tree from File

    for (i = 0; i < table.Length; i++)
    {
            table.codetable[i] = (CT_Entry*)malloc(sizeof(CT_Entry));
            entry = table.codetable[i];

            fread((void*)&(entry->original), sizeof(uint8_t), 1, file);
            fread((void*)&(entry->length_replacement), sizeof(uint8_t), 1, file);
            fread((void*)&(entry->replacement), sizeof(uint32_t), 1, file);
    }

    if (table.Length == 0) {
            return E_INVALID_CODETABLE;
    }


    tree = huffman_load(&table);



    if ((fp = fopen(outfilename, "w+b")) == NULL)
    {
            // Open File failed
            return E_COULD_NOT_OPEN_FILE;
    }

    while (*length > write_count)
    {
            // Keep the Buffer at min 32 Bit
            while(count <= 32)
            {

                    //buffer = buffer << 8;

                    buffer = buffer | (((uint64_t)fgetc(file) & 0xFF) << (56 - count));

                    count += 8;
            }

            currentPath = tree;

            while(currentPath->Node->Data == NULL)
            {

                    if (((buffer >> 63) & 1) == 0)
                    {
                            // LEFT
                    		currentPath = currentPath->Node->Left;
                    }
                    else
                    {
                            // RIGHT
                            currentPath = currentPath->Node->Right;
                    }
                    buffer = buffer << 1;
                    count--;

                    if (currentPath == NULL) {
                    	return E_UNABLE_TO_DEFLATE_FILE;
                    }
            }

            // Calculate CRC32

            IterateCRC32(&crc32, *((char*)currentPath->Node->Data));

            fputc(*((char*)currentPath->Node->Data), fp);
            write_count++;
    }

    fclose(fp);

    if (crc32 != *crc32_expect) {
    	 return E_CRC32_CHECK_FAILED;
    }

    return E_SUCCESS;
}

void
IterateCRC32(uint32_t *crc32, uint8_t data) {
	int crc_c;

	// Calculate CRC32

	for (crc_c = 0; crc_c < 8; crc_c++)
	{
			if (((*crc32 & 0x80000000) ? 1 : 0) != ((uint8_t)data >> crc_c))
				   *crc32 = (*crc32 << 1) ^ CRC32POLY;
			else
				   *crc32 <<= 1;
	}
}

/*
 * Taken from http://www.binarytides.com/str_replace-for-c/
 */
char*
str_replace(char *search , char *replace , char *subject)
{
    char  *p = NULL , *old = NULL , *new_subject = NULL ;
    int c = 0 , search_size;
    search_size = strlen(search);
    //Count how many occurences
    for(p = strstr(subject , search) ; p != NULL ; p = strstr(p + search_size , search))
    {
        c++;
    }
    //Final size
    c = ( strlen(replace) - search_size )*c + strlen(subject);
    //New subject with new size
    new_subject = malloc( c );
    //Set it to blank
    strcpy(new_subject , "");
    //The start position
    old = subject;
    for(p = strstr(subject , search) ; p != NULL ; p = strstr(p + search_size , search))
    {
        //move ahead and copy some text from original subject , from a certain position
        strncpy(new_subject + strlen(new_subject) , old , p - old);
        //move ahead and copy the replacement text
        strcpy(new_subject + strlen(new_subject) , replace);
        //The new start position after this search match
        old = p + search_size;
    }
    //Copy the part after the last search match
    strcpy(new_subject + strlen(new_subject) , old);
    return new_subject;
}

char*
human_readable_size(size_t size) {
	char *ext = " KMG";
	int i;
	char *str = (char*)calloc(1, 8);

	for (i = 0; i < strlen(ext); i++) {
		if (pow(1024, i) <= size && size <= pow(1024, i+1)) {
#ifdef _WIN32
			_snprintf(str, 8, "%.0f %cB", size/pow(1024, i), ext[i]);
#else
			snprintf(str, 8, "%.0f %cB", size/pow(1024, i), ext[i]);
#endif
			return str;
		}
	}

	return str; //doof.
}
