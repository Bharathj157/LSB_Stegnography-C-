#include <iostream>
#include <cstring>
#include <string>
#include "encode.h"
#include "common.h"
#include "types.h"
#include "decode.h"

using std::cout;
using std::endl;

Status EncodeInfo::read_and_validate_encode_args(char *argv[])
{   
    //validation for .bmp file
    if(strstr(argv[2],".bmp")!=NULL)
    {
	src_image_fname=argv[2];
    }
    else
    { 
	return e_failure;
    }

    //validation for secret_file
    if(argv[3]!=NULL)
    {
	int flag=1,len,num=0;
	secret_fname=argv[3];
	len=strlen(argv[3]);
	for(int i=len-1;i>=0;i-- )
	{
	    if(argv[3][i]=='.')
	    {
		flag=0;
		for(int j=i;argv[3][j]!='\0';j++)
		{                          
		    extn_secret_file[num++]=argv[3][j];
		}
		extn_secret_file[num]='\0';
		break;
	    }
	}
	if(flag==1)
	    return e_failure;

    }
    else 
    {
	return e_failure;
    }
    //validation for output image
    if(argv[4]==NULL)
    {
	stego_image_fname="stego.bmp";
    }
    else
    {    
	if(strstr(argv[4],".bmp")!=NULL)
	{
	    stego_image_fname=argv[4];
	}
	else
	{
	    int len=strlen(argv[4]);
		argv[4][len]='.';
		argv[4][len+1]='b';
		argv[4][len+2]='m';
		argv[4][len+3]='p';
		argv[4][len+4]='\0';
		stego_image_fname=argv[4];
	//	cout<<encInfo.stego_image_fname<<endl;
	}
    }
    return e_success;    
}


Status EncodeInfo::open_files()
{
    // Src Image file
    fptr_src_image = fopen(src_image_fname, "r");
    // Do Error handling
    if (fptr_src_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n",src_image_fname);

	return e_failure;
    }

    // Secret file
    fptr_secret = fopen(secret_fname, "r");
    // Do Error handling
    if (fptr_secret == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n",secret_fname);

	return e_failure;
    }
   // else
  //  {
    //    printf("NOT NULL\n");
  //  }

    // Stego Image file
    fptr_stego_image = fopen(stego_image_fname, "w");
    // Do Error handling
    if (fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n",stego_image_fname);

	return e_failure;
    }

    // No failure return e_success
    return e_success;
}


uint EncodeInfo::get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    std::cout<<"width = "<<width<<std::endl;
    

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    std::cout<<"height = "<<height<<std::endl;

    // Return image capacity
    return width * height * 3;
}


uint EncodeInfo::get_file_size(FILE *ptr)
{
    fseek(ptr,0,SEEK_END);
    //returing size of a file
    return ftell(ptr); 
}


Status EncodeInfo::check_capacity()
{
    //checking size of a src image is enough to hold the data of the secret file data 
    if(get_image_size_for_bmp(fptr_src_image) >= (54+(strlen(MAGIC_STRING))+4+strlen(extn_secret_file)+4+get_file_size(fptr_secret))*8)
    {
	return e_success;
    }
    else
    {
	return e_failure;
    }
}

Status EncodeInfo::copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char arr[54],ch1,ch2;

    fseek(fptr_src_image,0,SEEK_SET);
    //reading starting 54 bytes of data from src image
    fread(arr,54,1,fptr_src_image);
    fseek(fptr_dest_image,0,SEEK_SET);
     //Writing starting 54 bytes of data to stego image
    fwrite(arr,54,1,fptr_dest_image);

    return e_success;
}

Status EncodeInfo::encode_magic_string(const char *magic_string)
{
    int size=strlen(magic_string);
   // const char *data;
   // data=magic_string;
    encode_data_to_image(magic_string, size,fptr_src_image,fptr_stego_image);
    return e_success;
}

Status EncodeInfo::encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char image_buffer[8];
    for(int i=0;i<size;i++)
    {
       //reading 8 bytes data from src image every iteration
	fread(image_buffer,8,1,fptr_src_image);

	//calling byte to lsb function
	encode_byte_to_lsb(data[i], image_buffer);

	//writinig 8 bytes data to stego image
	fwrite(image_buffer,8,1,fptr_stego_image);
    }

    return e_success;
}

Status EncodeInfo::encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=0;i<8;i++)
    {  
    //clearing 0th bit
	image_buffer[i]=(image_buffer[i]&~(1));
    //geting a bit
	if(data&(1<<i))
	    image_buffer[i]|=1;  //replaceing a bit if 1
	else
	    image_buffer[i]|=0;  //replacing a bit if 0
    }

    return e_success;
}

//Definition for encode secret file extension

Status EncodeInfo::encode_secret_file_extn( char *file_extn)
{
    int size=strlen(file_extn);
  
    encode_data_to_image(file_extn, size,fptr_src_image,fptr_stego_image);

    return e_success;
}

Status EncodeInfo::encode_secret_file_size(long file_size)
{
    char image_buffer[32];
     //reading 32 bytes data from src image 
    fread(image_buffer,32,1,fptr_src_image);
    
    //calling size to lsb function
    encode_size_to_lsb(file_size,image_buffer);
    
     //reading 32 bytes data to stego image 
    fwrite(image_buffer,32,1,fptr_stego_image);
    return e_success;
}

Status EncodeInfo::encode_size_to_lsb(int data, char *image_buffer)
{
    for(int i=0;i<32;i++)
    {
    //clearing 0th bit
	image_buffer[i]=(image_buffer[i]&~(1));
	//geting a bit
	if(data&(1<<i))
	    image_buffer[i]|=1;   //replaceing a bit if 1
	else
	    image_buffer[i]|=0;   //replaceing a bit if 0
    }

    return e_success;
}

	
Status EncodeInfo::encode_secret_file_data()
{
   // int size=strlen(extn_secret_file);

    int size=size_secret_file;
    char image_buffer[8];
    char data[size];

    //making file pointer to point to the 0th position
    fseek(fptr_secret,0,SEEK_SET);
    
    //reading data size times from the secret file
    fread(data,size,1,fptr_secret);
    
    for(int i=0;i<size;i++)
    {   
     //reading 8 bytes data from src image every iteration
	fread(image_buffer,8,1,fptr_src_image);
	
	//calling byte to lsb function
	encode_byte_to_lsb(data[i], image_buffer);
	
	 //reading 8 bytes data to stego image every iteration
	fwrite(image_buffer,8,1,fptr_stego_image);
    }
    return e_success;
}

Status EncodeInfo::copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    //reading each byte data from src image untill it reaches EOF
    while(fread(&ch,1,1,fptr_src)!=0)
    {
	fwrite(&ch,1,1,fptr_dest);
    }
    return e_success;
}


Status EncodeInfo::do_encoding()
{
    printf("Started Encoding\n");
    if(open_files()==e_success)
    {
	cout<<"Open files successful\n";
    }
    else
    {
       cout<<"Open files failure\n";
       return e_failure;
    }
    if(check_capacity()==e_success)
    {
	cout<<"Check capacity successful\n";
    }
    else
    {
	cout<<"check capacity failure\n";
	return e_failure;
    }
    if(copy_bmp_header(fptr_src_image,fptr_stego_image)==e_success)
    {
	cout<<"Copy header successfull\n";
    }
    else
    {
	cout<<"Copy header failure\n";
	return e_failure;
    }
    if(encode_magic_string(password)==e_success)
    {
	cout<<"Encoding of Magic_String Successfull\n";
    }
    else
    {
	cout<<"Encoding of Magic_String is unSuccessfull\n";
	return e_failure;
    }

    if(encode_secret_file_size(strlen(extn_secret_file))==e_success)
    {
	cout<<"Encoding of secret file extension size successful\n";
    }
    else
    {
	cout<<"Encoding of secret file extension size unsuccessful\n";
    }
    if(encode_secret_file_extn(extn_secret_file)==e_success)
    {
	cout<<"Encoding of secret file extension is successful\n";
    }
    else
    {
	cout<<"Encoding of secret file extension is unsuccessful\n";
    }
    if(encode_secret_file_size(size_secret_file=get_file_size(fptr_secret))==e_success)
    {
	printf("Encoding of secret file size is successful\n");
    }
    else
    {
	printf("Encoding of secret file size is unsuccessful\n");
    }
    if(encode_secret_file_data()==e_success)
    {
	cout<<"Encoding of secret file data is successful\n";
    }
    else
    {
	cout<<"Encoding of secret file data is unsuccessful\n";
    }
    if(copy_remaining_img_data(fptr_src_image,fptr_stego_image)==e_success)
    {
	cout<<"Copying remaining image data is successfull\n";
    }
    else
    {
	cout<<"Copying remaining image data is unsuccessfull\n";
    }

    return e_success;
}

