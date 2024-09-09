#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc,char *argv[])
{
	if(argv[1] == NULL|| argv[2] ==NULL)
	{
		printf("./a.out: Encoding: ./a.out -e <.bmp file> <.txt file> [output file]\n");
		printf("./a.out: Decoding: ./a.out -d <.bmp file> [output file]\n");
	}
	else
	{
    int ret= check_operation_type(argv);
    if(ret==e_encode)
    {
        printf("Encoding\n");
    EncodeInfo encInfo;
    if((read_and_validate_encode_args(argv,&encInfo))==e_success)
    
    {
        //printf("validate successfully\n");
        do_encoding(argv,&encInfo);
    }
    else
    {
        printf("INFO: Validation part Error\n");
    }
    }
    else if(ret==e_decode)
    {
        printf("INFO: ## Decoding Procedure Started ##\n");
        DecodeInfo decInfo;
        if(( read_and_validate_decode_args(argv,&decInfo))==e_success)
        {
            do_decoding(&decInfo);
        }
        else
            printf("INFO: Decoding Validation part error\n");
    }
    else
        printf("unsuppoted error\n");
	}
	
}

OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1],"-e")==0)
        return e_encode;
    else if(strcmp(argv[1],"-d")==0)
        return e_decode;
    else
        return e_unsupported;
}

