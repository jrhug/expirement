#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int  UINT32;

static void buffer2hex(const UINT16 len, UINT8* buf, FILE *fptr)
{
	char delim[1] = " ";
	char* token;
	UINT8 val;
	
	fwrite(&len,1,sizeof(len),fptr);

	token = strtok((char*)buf, delim);
	while(token != NULL)
	{
		//temp = (int)*token;
		//memcpy(&temp,(int*)token,sizeof(UINT32));
		//sprintf(&val, "%02X", *token);
		printf("%d: %02X\n",__LINE__,*((UINT16*)token));
		
		fwrite(token,1,sizeof(UINT16),fptr);
		token = strtok(NULL," ");		 
	}
}

static void tokenize(const UINT16 len, UINT8* buf, FILE *fptr)
{
	int i;
	UINT8 val;
	
	fwrite(&len,1,sizeof(len),fptr);
	for(i=0;i<strlen(buf);)
	{
		if(buf[i] != 0x20)
		{
			// Convert string to hex to save to binary file
			val = strtol(&buf[i],NULL,16);
			val |= strtol(&buf[i+1],NULL,16);
			buf += 2;
			fwrite(&val,1,sizeof(val),fptr);
		}
		else
		{
			buf++;
		}
	}
}

static void name2hex(const UINT8 len, UINT8* buf, FILE *fptr)
{
	int   i;

	// Write name length	
	fwrite(&len,1,sizeof(len),fptr);
	for(i=0;i<len;i++,buf++)
	{
		// Write name
		fwrite(buf,1,sizeof(UINT8),fptr);
	}
}

static void process_file(FILE *p_in, FILE *p_out)
{
	UINT8 buf[4096];
	UINT8 name[32];
	UINT8 name_len,ev_len;
	UINT32 len;
	UINT8 *ptr = buf;
	int i;
	
	memset(buf,0,sizeof(buf));
	
	while(fscanf(p_in,"%[^,] %*c %u %*c %[^\n]\n",name,&len,buf)!= EOF)
	{		
		name2hex(strlen(name),name,p_out);
		// Tokenize the buffer
		// NOTE: String Length of buffer includes spaces. Hence saving the length
		//       from the file.
		tokenize (len,buf,p_out);
	}	
}

static void calc_chksum(FILE *fptr)
{
	UINT16 val;
	UINT16 chksum = 0;
	UINT8  hex_val;
	
	while(!feof(fptr))
	{
		fread(&val,sizeof(val),1,fptr);
		chksum += val;
	}
	printf("Checksum: %04X\n",(0 - chksum) & 0x0000ffff);
}

int main(int argc, char *argv[]) {
	FILE *fptr;
	FILE *output;
	UINT32 space;
	
	printf("argc %d %s\n",argc,argv[0]);
	if (argc < 2)
	{
		printf("Args: Enter a file name.\n");
	}
	else
	{
		printf("Hello\n");
		fptr = fopen(argv[1],"r");
		output = fopen("mybin4.bin","wb");

		// Open file
		if (fptr != NULL)
		{
			// Process file, create files
			process_file(fptr,output);
			fclose(fptr);
			fclose(output);	
			printf("Binary File Completed.");
			
			output = fopen("mybin4.bin","rb");
			calc_chksum(output);
			fclose(output);
		}
		else
		{
			printf("File Open Error: %d",ferror(fptr));
		}
	}
	return 0;
}
