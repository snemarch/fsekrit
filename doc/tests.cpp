


/*
typedef unsigned char u8;
void dumphex(void *buf, unsigned len)
{
	u8	*u8buf = static_cast<u8*>(buf);
	unsigned start = 0;
	unsigned i;

	while(len)
	{
		for(i=0; i<min(len,16); i++)
		{
			printf("%02X ", u8buf[start+i]);
		}

		for(i=0; i< (16-min(len,16)); i++)
		{
			printf("   ");
		}

		printf(": ");
		for(i=0; i<min(len,16); i++)
		{
			char ch;	
			ch = u8buf[start+i];
			if(!isprint(ch)) ch = '.';

			printf("%c", ch);
		}
		printf("\n");
		start += min(len, 16);
		len -= min(len, 16);
	}
}
*/

/*
	char *key     = "0000000000000000";
	char *message = "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF";
	char cipher[128];
	char plain[128];
	char hash[32];

	printf("%d\n", roundup(0, 8));
	printf("%d\n", roundup(1, 8));
	printf("%d\n", roundup(9, 8));
	
	sha256::hash(hash, message, strlen(message));

	xtea::encrypt(key, cipher, message, strlen(message) );
	xtea::decrypt(key, plain, cipher, strlen(message) );

	printf("sha-256:\n");
	dumphex(hash, sizeof(hash) );
	printf("encryption key:\n");
	dumphex(key, strlen(key) );
	printf("plaintext:\n");
	dumphex(message, strlen(message) );
	printf("ciphertext:\n");
	dumphex(cipher, strlen(message) );
	printf("decrypted:\n");
	dumphex(plain,  strlen(message) );
*/


/*
char* lut[] = {
"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B",
"0C", "0D", "0E", "0F", "10", "11", "12", "13", "14", "15", "16", "17",
"18", "19", "1A", "1B", "1C", "1D", "1E", "1F", "20", "21", "22", "23",
"24", "25", "26", "27", "28", "29", "2A", "2B", "2C", "2D", "2E", "2F",
"30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3A", "3B",
"3C", "3D", "3E", "3F", "40", "41", "42", "43", "44", "45", "46", "47",
"48", "49", "4A", "4B", "4C", "4D", "4E", "4F", "50", "51", "52", "53",
"54", "55", "56", "57", "58", "59", "5A", "5B", "5C", "5D", "5E", "5F",
"60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "6A", "6B",
"6C", "6D", "6E", "6F", "70", "71", "72", "73", "74", "75", "76", "77",
"78", "79", "7A", "7B", "7C", "7D", "7E", "7F", "80", "81", "82", "83",
"84", "85", "86", "87", "88", "89", "8A", "8B", "8C", "8D", "8E", "8F",
"90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "9A", "9B",
"9C", "9D", "9E", "9F", "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7",
"A8", "A9", "AA", "AB", "AC", "AD", "AE", "AF", "B0", "B1", "B2", "B3",
"B4", "B5", "B6", "B7", "B8", "B9", "BA", "BB", "BC", "BD", "BE", "BF",
"C0", "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9", "CA", "CB",
"CC", "CD", "CE", "CF", "D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7",
"D8", "D9", "DA", "DB", "DC", "DD", "DE", "DF", "E0", "E1", "E2", "E3",
"E4", "E5", "E6", "E7", "E8", "E9", "EA", "EB", "EC", "ED", "EE", "EF",
"F0", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "FA", "FB", 
"FC", "FD", "FE", "FF" };

int main(int argc, char* argv[])
{
	unsigned char xxx[32];

	if(argc<2)
	{
		printf("FUUUUK\n");
		return 0;
	}
	sha256::hash(xxx, argv[1], strlen(argv[1]));

	for(unsigned i=0; i<sizeof(xxx); i++)
		printf("%s", lut[xxx[i]]);
	printf("\n");
}
*/
