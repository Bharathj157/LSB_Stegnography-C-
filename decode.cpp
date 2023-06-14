#include <iostream>
#include <cstring>
#include "common.h"
#include "decode.h"
/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status DecodeInfo::open_files_decode()
{
    
    // Stego Image file
    fptr_stego_image = fopen(stego_src_name, "r");
    // Do Error handling
    if (fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n",stego_src_name);

        return e_failure;
    }
    // No failure return e_success
    return e_success;
}

//Definition for validate input arguments

Status DecodeInfo::read_and_validate_decode_args(char *argv[])
{
  //validation for .bmp file 
    if(strstr(argv[2],".bmp")!=NULL)
    {
        stego_src_name=argv[2];
    }
    else
        return e_failure;

  //validation for output file 
    if(argv[3]==NULL)
    {
        output_fname[0]='o';
        output_fname[1]='u';
        output_fname[2]='t';
        output_fname[3]='p';
        output_fname[4]='u';
        output_fname[5]='t';
        output_fname[6]='\0';
    }
    else
    {   
          for(int i=0;i<strlen(argv[3]);i++)	
          output_fname[i]=argv[3][i];
    }

    return e_success;
}

Status DecodeInfo::decode_magic_string(const char *data)
{
    char arr[strlen(data)];
    char buffer[8];
    int i;
    
    //using fseek function to skip 54 bytes (header)
    fseek(fptr_stego_image,54,SEEK_SET);

    for(i=0;i<strlen(data);i++)
    { 
    //reading 8 bytes of data from stego image
	fread(buffer,8,1,fptr_stego_image);
	
   //storing each charcter in a array in each iteration 	
	arr[i]=decode_byte_from_lsb(buffer);
    }
    arr[i]='\0';
    
    //cmp magic string value with decoded magic string 
    if(strcmp(arr,data)==0)
	return e_success;
    else
	return e_failure;
}

//Definition Decode byte from lsb

char DecodeInfo::decode_byte_from_lsb(char* buffer)
{
    char ch=0;
    for(int i=0;i<8;i++)
    {
        //logic to get a bit of a encoded data of 8bit data
	ch=(buffer[i]&1)<<i|ch;
    }
    return ch;
}

//Definition for decode Extension Size

Status DecodeInfo::decode_extension_size()
{
   char buffer[32];
   
   //reading 32 bytes of data from stego image
   fread(buffer,32,1,fptr_stego_image);
   
   //storing the size after decoding is done
   int size=decode_size_from_lsb(buffer);

   //storing size in a structure
   dsize_secret_file=size;

   return e_success;
}

//Definition for Decoding size from lsb

int DecodeInfo::decode_size_from_lsb(char* buffer)
{
    int size=0;
    for(int i=0;i<32;i++)
    {
     //logic to get a bit of a encoded data of 32bit data
	size=(buffer[i]&1)<<i|size;
    }
    return size;
}

//Definition for Decode secret file extension

Status DecodeInfo::decode_secret_extension()
{
    int i;
    char arr[dsize_secret_file+1];
    char buffer[8];

    for(i=0;i<dsize_secret_file;i++)
    {
      //reading 8 bytes of data from stego image
        fread(buffer,8,1,fptr_stego_image);

      //storing each charcter in an array after decoding
 	arr[i]=decode_byte_from_lsb(buffer);

    }
    arr[i]='\0';

  //storing extension in a structure
    extn_output_file=strcat(output_fname,arr);

    return e_success;
}

//Definition for Decode secret file size

Status DecodeInfo::decode_secret_file_size()
{
   char buffer[32];
   //reading 8 bytes of data from stego image
   fread(buffer,32,1,fptr_stego_image);
   
   int size=decode_size_from_lsb(buffer);


   //storing decoded vale in a struct
   dsize_secret_file_data=size;
 
   return e_success;
}

//Definition for decode secret file data

Status DecodeInfo::decode_secret_file_data()
{
   //opening output file in a write mode
    fptr_output=fopen(output_fname, "w");
    char arr[dsize_secret_file_data];
    char buffer[8];
    int i;
    for(i=0;i<dsize_secret_file_data;i++)
    {
       //reading 8 bytes from stego image
	fread(buffer,8,1,fptr_stego_image);
	
	arr[i]=decode_byte_from_lsb(buffer);
	
	//after decoding writing data into a output file
	fwrite(&arr[i],1,1,fptr_output);
    }
    return e_success;
}


Status DecodeInfo::do_decoding()
{
    printf("Started Decoding\n");
    if(open_files_decode()==e_success)
    {
        printf("Open files successful\n");
    }
    else
    {
        printf("Open files failure\n");
        return e_failure;
    }
    if(decode_magic_string(password)==e_success)
    {
        printf("Decoding Magic String Successful\n");
    }
    else
    {
	printf("Decoding Magic String Unseccessful\n");
	return e_failure;
    }
    if(decode_extension_size()==e_success)
    {
	printf("Decoding of extension size in Successful\n");
    }
    else
    {
	printf("Decoding of extension size is unSuccessful\n");
    }

    if(decode_secret_extension()==e_success)
    {
	printf("Decoding of secret extension is Successful\n");
    }
    else
    {
	printf("Decoding of secret extension is unsuccessful\n");

    }

    if(decode_secret_file_size()==e_success)
    {
	printf("Decoding of secret file size is successful\n");
    }
    else
    {
       printf("Decoding of secret file size is unsuccessful\n");
    }
    if(decode_secret_file_data()==e_success)
    {
	printf("Decoding of secret file data  is successful\n");
    }
    else
    {
       printf("Decoding of secret file data is unsuccessful\n");
    }


    return e_success;
}
