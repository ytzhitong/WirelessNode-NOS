#include "AT.h"
#include "string.h"
#include "uart5.h"

u8 LoraEnterCommand(void)
{
	char src[]="+++";
	HAL_UART_Transmit(&UART5_Handler,(uint8_t *)src,3,1);
	
	
	
	return 1;
}


