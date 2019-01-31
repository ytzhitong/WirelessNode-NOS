
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MY_GPIO_H
#define __MY_GPIO_H

/* Includes ------------------------------------------------------------------*/

#define LED_PORT GPIOC
#define LED_PIN  GPIO_PIN_9

#define LORA_WAKE_PORT GPIOB
#define LORA_WAKE_PIN  GPIO_PIN_9

#define KEY_PORT GPIOC
#define KEY_PIN  GPIO_PIN_6

#define LED(n)	(n?HAL_GPIO_WritePin(LED_PORT,LED_PIN,GPIO_PIN_SET):HAL_GPIO_WritePin(LED_PORT,LED_PIN,GPIO_PIN_RESET))
#define LED_TogglePin	HAL_GPIO_TogglePin(LED_PORT,LED_PIN)

#define LORA_WAKE(n)	(n?HAL_GPIO_WritePin(LORA_WAKE_PORT,LORA_WAKE_PIN,GPIO_PIN_SET):HAL_GPIO_WritePin(LORA_WAKE_PORT,LORA_WAKE_PIN,GPIO_PIN_RESET))

#define KEY_PIN_STATE()       HAL_GPIO_ReadPin(KEY_PORT, KEY_PIN)

void GPIO_Init(void);
void LED_task(void);

#endif /* __MY_GPIO_H */