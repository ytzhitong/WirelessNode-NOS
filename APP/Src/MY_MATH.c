
#include "stdio.h"
#include "MY_MATH.h"

float HexToDecimal(unsigned char *Byte,int num)
{
	return *((float*)&Byte);
}

void DecimalToHex(float floatnum, char* Byte)
{
	char pchar1[4];
	char* pchar=(char*)&floatnum;

	for(int i=0;i<sizeof(float);i++)
	{
		pchar1[3-i]=*pchar;
		pchar++;
	}	
	
	for(int i=0;i<sizeof(float);i++)
	{
		*Byte=pchar1[i];
		Byte++;
	}
}

