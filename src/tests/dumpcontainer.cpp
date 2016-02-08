#include "../precompiled.h"
#define _CRT_SECURE_NO_DEPRECATE

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

const unsigned WIDTH = 16;

template<typename T> T min(T lhs, T rhs)
{
	return (lhs < rhs) ? lhs:rhs;	
}


typedef unsigned char u8;
void dumphex(void *buf, unsigned len)
{
	u8	*u8buf = static_cast<u8*>(buf);
	unsigned start = 0;
	unsigned i;

	while(len)
	{
		for(i=0; i < min(len, WIDTH); i++)
		{
			printf("%02X ", u8buf[start+i]);
		}

		for(i=0; i < (WIDTH - min(len, WIDTH)); i++)
		{
			printf("   ");
		}

		printf(": ");
		for(i=0; i < min(len, WIDTH); i++)
		{
			unsigned char ch;	
			ch = u8buf[start+i];
			if(!isprint(ch)) ch = '.';

			printf("%c", ch);
		}
		printf("\n");
		start += min(len, WIDTH);
		len -= min(len, WIDTH);
	}
}

const char *const header_v1_signature = "SEKRiT*1";
const char *const header_v2_signature = "SEKRiT*2";

const unsigned FL_READONLY = 1;

#pragma pack(push, 1)
struct FileHeaderV2
{
	char signature[8];							// header_v2_signature
	char hash[32];								// plaintext hash
	unsigned char iv[16];						// initial encryption vector (added in v2)
	unsigned size;								// size of data, padded to crypt blocksize
	unsigned realsize;							// "real" size of data, unpadded
	unsigned flags;								// bitmask of FL_* constants 
	unsigned w, h;								// saved width and height... mouser wants this
};
#pragma pack(pop)


int main(int argc, char *argv[])
{
	FileHeaderV2	hdr;

	FILE *f = fopen(argv[1], "rb");
	if(!f)
	{
		perror(argv[1]);
		exit(1);
	}

	fread(&hdr, 1, sizeof(hdr), f);
	fclose(f);

	printf("signature:\n");
	dumphex(hdr.signature, sizeof(hdr.signature));
	printf("hash:\n");
	dumphex(hdr.hash, sizeof(hdr.hash));
	printf("IV:\n");
	dumphex(hdr.iv, sizeof(hdr.iv));
	printf("size:		%d\n", hdr.size);
	printf("realsize:	%d\n", hdr.realsize);
	printf("flags:		0x%08X\n", hdr.flags);
	printf("w:			%d\n", hdr.w);
	printf("h:			%d\n", hdr.h);
}
