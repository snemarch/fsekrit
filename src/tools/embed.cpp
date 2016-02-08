#include <stdio.h>
#include <string>

#include "../sekrit.h"
#include "../FileEndData.h"

using namespace std;
using namespace sekrit;

size_t fsize(FILE* fil)
{
	size_t	oldpos, size;

	oldpos = ftell(fil);
	fseek(fil, 0, SEEK_END);
	size = ftell(fil);
	fseek(fil, oldpos, SEEK_SET);
	return size;
}


int main(int argc, char* argv[])
{
	if(argc<4)
	{
		printf("Usage: embed file.exe file.txt passphrase");
		return 0;
	}

	FILE *f = fopen(argv[2], "rb");
	if(!f)
	{
		perror(argv[2]);
		return 0;
	}
	size_t poof = fsize(f);

	vector<unsigned char> plaintext(poof);
	fread(&plaintext[0], 1, poof, f);
	fclose(f);

	Sekrit	sekrit(argv[3]);
	sekrit.SetPlaintext(plaintext);

	vector<unsigned char> ciphertext;
	sekrit.GetEncryptedBlob(ciphertext);

	EndData::Write(argv[1], ciphertext);
	
/*
	vector<unsigned char>	vec;
	Sekrit	sek(key);

	vec.resize(strlen((char*)message));
	memcpy(&vec[0], message, vec.size());

	sek.SetPlaintext(vec);
	sek.GetEncryptedBlob(vec);

	EndData::Write("Release\\fsekrit.exe", vec);
*/
}
