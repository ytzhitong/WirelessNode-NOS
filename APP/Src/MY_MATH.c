
#include "stdio.h"
#include "MY_MATH.h"

float HexToDecimal(unsigned char *Byte,int num)
{
	return *((float*)&Byte);
}

void DecimalToHex(float floatnum,unsigned char* Byte)
{
	char* pchar=(char*)&floatnum;
	for(int i=0;i<sizeof(float);i++)
	{
		*Byte=*pchar;
		pchar++;
		Byte++;
	}
}

