#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types
#include"common.h"
/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define DMAX_FILE_SUFFIX 15


class DecodeInfo
{
    private:
	/* Stego Image Info */
	char* stego_src_name;
	FILE* fptr_stego_image;

	/*output file*/
	char output_fname[DMAX_FILE_SUFFIX];
	FILE* fptr_output;
	long dsize_secret_file;
	long dsize_secret_file_data;
	char *extn_output_file;
	
        const char *password=MAGIC_STRING;

	/*File open */
	Status open_files_decode();

	/* Extract Magic String */
	Status decode_magic_string(const char * data);

	/* Decode byte from lsb */
	char decode_byte_from_lsb(char* buffer);

	/*Decode size fro lsb */
	int decode_size_from_lsb(char* buffer);

	/*Decode extension size */
	Status decode_extension_size();

	/*Decode secret file size*/
	Status decode_secret_file_size();

	/*Decode secret file extension*/
	Status decode_secret_extension();

	/*Decode secret file data*/
	Status decode_secret_file_data();
   

    public:
	/* Check operation type */
	OperationType check_operation_type(char *argv[]);

	/* Read and validate Decode args from argv */
	Status read_and_validate_decode_args(char *argv[]);

	/* Perform the Decoding */
	Status do_decoding();
};

#endif
