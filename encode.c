#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"

/* Function Definitions */

Status read_and_validate_encode_args(char *argv[],EncodeInfo *encInfo) 
{
	/*argv[2]=>.bmp
	  condition is true store argv[2] in src_image_fname
	  condition is failure return e_failure*/
	if((strstr(argv[2],".bmp")))
	{
		encInfo->src_image_fname=argv[2];
	}
	else if(argv[2]==NULL)
	{
		printf("INFO: Encoding: ./a.out -e <.bmp file> <.txt file>[output file]\n");
	}

	else
	{
		printf("INFO: Encoding: ./a.out -e <.bmp file> <.txt file>[output file]\n");
		return e_failure;
	}
	/*argv[3]=>.txt or .c or .sh
	  condition is true store argv[3] in secret_fname
	  condition is false return e_failure*/

	if(argv[3]==NULL)
	{
		printf("INFO: Encoding: ./a.out -e <.bmp file> <.txt file>[output file]\n");
		return e_failure;
	}
	else if(strstr(argv[3],".txt"))
	{
		encInfo->secret_fname=argv[3];
		strcpy(encInfo->extn_secret_file,".txt");
	}
	else if(strstr(argv[3],".sh"))
	{
		encInfo->secret_fname=argv[3];
		strcpy(encInfo->extn_secret_file,".sh");

	}
	else if(strstr(argv[3],".c"))
	{
		encInfo->secret_fname=argv[3];
		strcpy(encInfo->extn_secret_file,".c");
	}
	else
	{
		printf("INFO: Encoding: ./a.out -e <.bmp file> <.txt file>[output file]\n");
		return e_failure;
	}

	/*check argv[4] is passed or not
	  passed=>argv[4]=>.tmp
	  condition is true store argv[4] in stego_image_fname
	  condition is false return e_failure
	  condition is not passed set default.bmp stored stego_image_fanme*/

	if(argv[4] !=NULL)
	{
		if((strstr(argv[4],".bmp")))
		{
			encInfo->stego_image_fname=argv[4];
		}
		else
		{
			printf("INFO: Encoding: ./a.out -e <.bmp file> <.txt> output.bmp\n");
			return e_failure;
		}
	}
	else
	{
		printf("INFO: Output File not mentioned. Creating default.bmp as default\n");
		encInfo->stego_image_fname="default.bmp";
	}
	return e_success;


}
/* Encoding process starts */
/* Encoding function calls */
Status do_encoding(char *argv[],EncodeInfo *encInfo)
{
	if((open_files(encInfo)==e_success))
	{
		printf("INFO: ## Encoding Procedure Started ##\n");
		printf("INFO: Checking for %s size\nINFO: Done. Not Empty\n",encInfo->secret_fname);
		if(check_capacity(encInfo)==e_success)
		{
			printf("INFO: Checking for %s Capacity to handle %s\n",encInfo->src_image_fname,encInfo->secret_fname);
			printf("INFO: Done. Found OK\n");
			if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
			{
				printf("INFO: Done\n");
				if(encode_magic_string(MAGIC_STRING,encInfo)==e_success)
				{
					printf("INFO: Done\n");
					int len=0;
					if(strstr(argv[3],".txt")){len=4;}
					else if(strstr(argv[3],".sh")){len=3;}
					else if(strstr(argv[3],".c")){len=2;}
					if((encode_secret_file_extn_size((len),encInfo)==e_success))
					{
						printf("INFO: Done\n");
						if((encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_success))
						{
							printf("INFO: Done\n");
							if(encode_secret_file_size((encInfo->size_secret_file),encInfo)==e_success)
							{
								printf("INFO: Done\n");
								if(encode_secret_file_data(encInfo)==e_success)
								{
									printf("INFO: Done\n");
									if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
									{
										printf("INFO: Done\n");
										printf("INFO: ## Encoding Done Successfully ##\n");
									}
									else
										printf("Copying remaining data is failed\n");
								}
								else
									printf("Secret file encoding is failed\n");
							}
							else
								printf("Secret file extension encoding size is failed\n");
						}
						else
							printf("Secret file extension is failed\n");

					}
					else
						printf("secret file extension size if failed\n");
				}

				else
					printf("Magic string is failed\n");


			}
			else
				printf("Copy header is failed\n");


		}
		else
			printf("Capacity is not in the limit\n");
	}
	else 
		printf("Error in opening the files\n");

}
/*checking the image capacity and secret file size */
Status check_capacity(EncodeInfo *encInfo)
{
	/* Checking the capacity of the data is greater than secret data the sapace is available*/
	encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image);
	encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);
	int len=encInfo->size_secret_file;
	int len1=strlen(MAGIC_STRING);
	//	printf("imagecapacity:%d\n",encInfo->image_capacity);
	//	printf("secret file size: %ld\n",encInfo->size_secret_file);
	/*condition is trur return e_success
	  condition is false return e_failure*/
	if(encInfo->image_capacity>(54+(len1*8)+(4*8)+(4*8)+(4*8)+(len*8)))
	{
		return e_success;
	}
	else
		return e_failure;

}
/* checking the secret file size*/
uint get_file_size(FILE *fptr)
{

	if(fptr==NULL)
	{
		printf("Error unable to open file\n");
	}
	else
		fseek(fptr,0,SEEK_END);
	return ftell(fptr);


}

/*copying the 54 header file from sourse to destinition*/
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
	printf("INFO: Copying Image Header\n");
	rewind(fptr_src_image);char buffer[54];
	fread(buffer,54,1,fptr_src_image);
	fwrite(buffer,54,1,fptr_dest_image);
	return e_success;
}



/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
	uint width, height;
	// Seek to 18th byte
	fseek(fptr_image, 18, SEEK_SET);

	// Read the width (an int)
	fread(&width, sizeof(int), 1, fptr_image);
	//	printf("width = %u\n", width);

	// Read the height (an int)
	fread(&height, sizeof(int), 1, fptr_image);
	//	printf("height = %u\n", height);

	// Return image capacity
	return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
	printf("INFO: Opening required files\n");
	printf("INFO: Opened %s\n",encInfo->src_image_fname);
	// Src Image file
	encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
	// Do Error handling
	if (encInfo->fptr_src_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

		return e_failure;
	}

	// Secret file
	printf("INFO: Opened %s\n",encInfo->secret_fname);
	encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
	// Do Error handling
	if (encInfo->fptr_secret == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

		return e_failure;
	}

	// Stego Image file
	printf("INFO: Opened %s\n",encInfo->stego_image_fname);
	printf("INFO: Done\n");
	encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
	// Do Error handling
	if (encInfo->fptr_stego_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

		return e_failure;
	}

	// No failure return e_success
	return e_success;
}
/* encoding the magic string find the length of magic string
   pass the array and magic string change the bit from msb to lsb*/
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
	printf("INFO: Encoding Magic String Signature\n");
	char arr[8];
	int l=strlen(MAGIC_STRING);
	for(int i=0; i<l; i++)
	{
		fread(arr,8,1,encInfo->fptr_src_image);
		encode_byte_to_lsb(magic_string[i],arr);
		fwrite(arr,8,1,encInfo->fptr_stego_image);
	}
	return e_success;
}
/*encoding the byte from lsb side*/
Status encode_byte_to_lsb(char data, char *image_buffer)
{
	int n;
	for(int i=0; i<8; i++)
	{
		n=((unsigned)data >>(7-i))&1;
		image_buffer[i]=image_buffer[i]&(~1)|n;
		//printf("%d",image_buffer[i]);
	}
	return e_success;
	//printf("\n");
}
/*encoding the secret file size is an integer value*/
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
	printf("INFO: Encoding %s File extension size\n",encInfo->secret_fname);
	char arr[32];
	fread(arr,32,1,encInfo->fptr_src_image);
	printf("exten size is %d\n",size);
	encode_size_to_lsb(size,arr);
	fwrite(arr,32,1,encInfo->fptr_stego_image);
	return e_success;
}
/*encoding the size from lsb*/
Status encode_size_to_lsb(int data, char *image_buffer)
{
	int num;
	for(int i=0; i<32; i++)
	{
		num=(((unsigned)data>>(31-i))&1);
		image_buffer[i]=image_buffer[i]&(~1)|num;

	}
	return e_success;
}
/* encoding the secret file extn*/
Status encode_secret_file_extn(const char *file_extn,EncodeInfo *encInfo)
{
	printf("INFO: Encoding %s File Extension\n",encInfo->secret_fname);
	int len=strlen(file_extn);
	printf("Extension length is %d\n",len);
	char arr[8];
	for(int i=0; i<len; i++)
	{
		fread(arr,8,1,encInfo->fptr_src_image);
		encode_byte_to_lsb(file_extn[i],arr);
		fwrite(arr,8,1,encInfo->fptr_stego_image);
	}
	return e_success;
}
/*encode the secret file size*/
Status encode_secret_file_size(long file_size,EncodeInfo *encInfo)
{
	printf("INFO: Encoding %s File Size\n",encInfo->secret_fname);
	char arr[32];
	fread(arr,32,1,encInfo->fptr_src_image);
	encode_size_to_lsb(file_size,arr);
	fwrite(arr,32,1,encInfo->fptr_stego_image);
	return e_success;
}

/*encoding the secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo)
{
	printf("INFO: Encoding %s File Data\n",encInfo->secret_fname);
	char secret_data[MAX_SECRET_BUF_SIZE];
	FILE *fptr=fopen(encInfo->secret_fname,"r");
	char arr[8];
	fscanf(fptr," %[^\n]",secret_data);
	int len=strlen(secret_data);
	for(int i=0; i<encInfo->size_secret_file; i++)
	{
		fread(arr,8,1,encInfo->fptr_src_image);
		encode_byte_to_lsb(secret_data[i],arr);
		fwrite(arr,8,1,encInfo->fptr_stego_image);
	}
	return e_success;
}

/*copiying remaing data from sorce to destination*/
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
	printf("INFO: Copying Left Over Data\n");
	char ch;
	while(fread(&ch,1,1,fptr_src))
	{
		fwrite(&ch,1,1,fptr_dest);
	}
	return e_success;
}

