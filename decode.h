#ifndef DECODE_H
#define DECODE_H

#include "types.h" //Contains user defined types

/*
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 100
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image;
    char secret_fname[20];
    FILE *fptr_secret_fname;
    int secret_file_size;
    int secret_extn_size;

} DecodeInfo;


/* Decoding function prototype */

/* Read and validate decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status file_open(DecodeInfo *decInfo);
/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);
 /* compare magic string*/
Status decode_magic_string(FILE *fptr_src_image);

Status decode_byte_from_lsb(char *arr,char *data);

Status decode_size_from_lsb(int *data,char *arr);

Status decode_secret_file_extn_size(int size,DecodeInfo *decInfo);

Status decode_secret_file_extn(FILE *fptr_src_image,DecodeInfo *decInfo);

Status decode_secret_file_size(int size,DecodeInfo *decInfo);

Status decode_secret_file_data(DecodeInfo *decInfo);
#endif
