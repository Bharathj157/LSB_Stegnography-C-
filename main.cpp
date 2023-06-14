#include <iostream>
#include <string>
#include <cstring>
#include "encode.h"
#include "decode.h"
#include "types.h"

//function definition for operation type

OperationType EncodeInfo::check_operation_type(char** argv)
{
    if(strcmp(argv[1],"-e")==0)      //for encoding
	return e_encode;
    else if(strcmp(argv[1],"-d")==0)  //for decoding
	return e_decode;
    else
	return e_unsupported;
}

int main(int argc,char*argv[])
{
    
       EncodeInfo encInfo;
       DecodeInfo decInfo;

    //validation for input arguments
    if(argc>=3)
    {
	if((encInfo.check_operation_type(argv))==e_encode)        //checking for e_encode
	{
	    printf("Selected Encoding\n");

	   
	    if(encInfo.read_and_validate_encode_args(argv)==e_failure)
	    {
		printf("Unsuccessful reading and validating\n");
	    }
	    else
	    {
		printf("Successful reading and validating\n");
		
		
		   if(encInfo.do_encoding()==e_success)         //function call encoding
		   printf("Encoding Successful\n");
		   else
		   printf("ENcoding failure\n");
		   
		 
	    }

	}
	else if((encInfo.check_operation_type(argv))==e_decode)     //checking for e_decode
	{
	    printf("Selected Decoding\n");
        
	    if(decInfo.read_and_validate_decode_args(argv)==e_success)
	    {
		printf("Successfull reading and validation\n");
		
		   if(decInfo.do_decoding()==e_success)        //function call decoding
		   printf("Decoding Successful\n");
		   else
		   printf("Decoding unsuccessful");
		 
	    }
	    else
		printf("UNsuccessful reading and validating\n");
		


	}
	else                                               //printing error msg for other option
	    printf("Error:Entered option is not valid\n");
	    
    }
    else
	printf("Error:Enter Cmd line inputs more than 3 or equal to 3\n");

}
