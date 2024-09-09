#include <stdio.h>
#include "decode.h"
#include "types.h"
#include <string.h>

/* Function Definitions */
Status read_and_validate_decode_args(char *argv[],DecodeInfo *decInfo)
{
    /*if CLA input argv[2]->.bmp
      if it is true->store argv[2] in src_image_fname
      if it is false->return e_failure*/
    if(strstr(argv[2],".bmp"))
    {
        decInfo->src_image_fname=argv[2];
    }
    else if(argv[2]==NULL)
    {
        printf("Decoding: ./lsb_steg -d <.bmp_file> [output file]\n");
    }
    else
    {
        printf("Decoding: ./lsb_steg -d <.bmp_file> [output file]\n");
        return e_failure;
    }
    //if CLA input passed or not
    if(argv[3] != NULL)
    {
        strcpy(decInfo->secret_fname,argv[3]);//stored into secret fname
    }
    else
        strcpy(decInfo->secret_fname,"default"); //if not passed ->default name stored in secret fname

    return e_success;
}
//decoding process
Status do_decoding(DecodeInfo *decInfo)//decode definition
{
    if((file_open(decInfo) == e_success)) //decode function calls
    {
        printf("INFO: Done\n");
        printf("INFO: Decoding Magic string signature\n");
        if(decode_magic_string(decInfo->fptr_src_image) == e_success)
        {
            printf("INFO: Done\n");
            if(decode_secret_file_extn_size((4*8),decInfo) == e_success)
            {
                printf("INFO: Done\n");
                if(decode_secret_file_extn(decInfo->fptr_src_image,decInfo)==e_success)
                {
                    printf("INFO: Done \n");
                    if(decode_secret_file_size((sizeof(int)*8),decInfo)==e_success)
                    {
                        printf("INFO: Done\n");
                        if(decode_secret_file_data(decInfo)==e_success)
                        {
                            printf("INFO: Done\nINFO: ## Decoding Done Successfully ##\n");
                        }
                        else
                            printf("Decoding secret message failed\n");
                    }
                    else
                        printf("Decoding file size failed\n");
                }
                else
                    printf("Decoding the extension failed\n");
            }
            else
                printf("Decoding the extension size failed\n");
        }
        else
            printf("Decoding magic string failed\n");
    }
    else
        printf("Unable to open the files\n");
}
Status file_open(DecodeInfo *decInfo)
{
	printf("INFO: Opening required files\n");
    decInfo->fptr_src_image=fopen(decInfo->src_image_fname,"r");
	 printf("Opened %s\n",decInfo->src_image_fname);
    if(decInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr,"ERROR: unable to open the file %s\n",decInfo->src_image_fname);
        return e_failure;
    }
    return e_success;
}
//decoding the magic string
Status decode_magic_string(FILE *fptr_src_image)
{
    char password[20];static int count=0;
    printf("Enter magic string : ");//decoder enter magic string
    scanf("%s",password);
    fseek(fptr_src_image, 54, SEEK_SET);//src image first 54 bytes is header file
    char arr[8],data[5];
    for(int i=0; i<5; i++) //using for loop for find the magic string
    {
        fread(arr,8,1,fptr_src_image);
        decode_byte_from_lsb(arr,&data[i]);
    }
    data[5]= '\0';
    //printf("%s\n",data);
    if(strcmp(password,data)==0)//magic string is matching or not
    {
        return e_success;
    }
    else
    {
        count++;
        if(count != 3)
        {
            printf("Wrong key\nTry again\n");
            decode_magic_string(fptr_src_image);
        }
        else //user enter magic string is not match return failure
        {
            printf("Magic string is not matching\n");
            return e_failure;
        }
    }
}
//decoding th byte_from_lsb
Status decode_byte_from_lsb(char *arr,char *data)
{
    char ch=0,n;
    for(int i=0; i<8; i++)
    {
        n=arr[i]&1;
        ch=(n<<(7-i))|ch;
    }
    //printf("character is %c\n",ch);
    *data=ch;
}
//decoding secret file extn
Status decode_secret_file_extn_size(int size,DecodeInfo *decInfo)
{
	printf("INFO: Decoding Output File Extension Size\n");
    char arr[32];
    fread(arr,32,1,decInfo->fptr_src_image);
    decode_size_from_lsb(&decInfo->secret_extn_size,arr);
    //printf(" decode info is %d\n",decInfo->secret_extn_size);
    return e_success;
}
//decoding the size from lsb
Status decode_size_from_lsb(int *data, char *arr)
{
    int n=0,num=0;
    for(int i=0; i<32; i++)
    {
        n=arr[i]&1;
        num=(n<<(31-i))|num;
    }
    //printf("Num is %d\n",num);
    *data=num;
}
//decoding the secret file extn
Status decode_secret_file_extn(FILE*fptr_src_image,DecodeInfo *decInfo)
{
    printf("INFO: Decoding Output File Extensions\n");
    char arr[8],data[decInfo->secret_extn_size];
    //printf("extn size is %d\n",decInfo->secret_extn_size);
    for(int i=0; i<decInfo->secret_extn_size; i++)
    {
        fread(arr,8,1,fptr_src_image);
        decode_byte_from_lsb(arr,&data[i]);
    }
    data[decInfo->secret_extn_size]='\0';
    //printf("%s\n",data);
    char *str;
    strcat(decInfo->secret_fname,data);
	printf("Output File not mentioned. Creating decoded file as %s\n",decInfo->secret_fname);
    decInfo->fptr_secret_fname=fopen(decInfo->secret_fname,"w");
    if(decInfo->fptr_secret_fname==NULL)
    {
        perror("fopen");
        fprintf(stderr,"ERROR: unable to open the file %s\n",decInfo->secret_fname);
        return e_failure;
    }
    return e_success;
}
//decoding the secret file extn
Status decode_secret_file_size(int size,DecodeInfo *decInfo)
{
	printf("INFO: Decoding Secret File Size\n");
    char arr[32];
    fread(arr,32,1,decInfo->fptr_src_image);
    decode_size_from_lsb(&decInfo->secret_file_size,arr);
    return e_success;
}
//decode the secret file data
Status decode_secret_file_data(DecodeInfo *decInfo)
{
	printf("INFO: Decoding Secret Data\n");
    char arr[8];
    //printf("secret file size %ld\n",decInfo->secret_file_size);
    char data[decInfo->secret_file_size];
    for(int i=0; i<decInfo->secret_file_size-1; i++)
    {
        fread(arr,8,1, decInfo->fptr_src_image);
        decode_byte_from_lsb(arr, &data[i]);
    }
    data[decInfo->secret_file_size]='\0';
    fwrite(data,1,decInfo->secret_file_size-1,decInfo->fptr_secret_fname);

    return e_success;
    //return e_success;
}
