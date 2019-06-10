#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#define _STM_IAR_BOARD_
#include "board_config.h"
#define DELAY 200000
static void Delay ( uint32_t delay); 

int main()
{
  GPIO_InitTypeDef	GPIO_InitStruct;
  uint8_t flag;
  uint8_t flag1;
  
  
  
  RCC_AHB1PeriphClockCmd(F_ROC, ENABLE);

  GPIO_InitStruct.GPIO_Pin = LED_PIN1 | LED_PIN2 | LED_PIN3 | LED_PIN4 | LED_PIN5;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

  GPIO_Init(F_PORT, &GPIO_InitStruct);
  
  RCC_AHB1PeriphClockCmd(G_ROC, ENABLE);
  GPIO_InitStruct.GPIO_Pin = USER_BTN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

  GPIO_Init(G_PORT, &GPIO_InitStruct);
  
  
  RCC_AHB1PeriphClockCmd(A_RCC, ENABLE);
  GPIO_InitStruct.GPIO_Pin = WKUP_BTN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

  GPIO_Init(A_PORT, &GPIO_InitStruct);
  
  uint32_t currentLed;
  uint32_t previusLed;
  currentLed = LED_PIN1;
  uint32_t nextLed;
  nextLed = LED_PIN2;

  while(1){
    flag1 = GPIO_ReadInputDataBit(GPIOA, WKUP_BTN);
    flag = GPIO_ReadInputDataBit(GPIOG, USER_BTN);
    if (!flag){
      
      if (nextLed == LED_PIN1) {
        currentLed = LED_PIN2;
        previusLed = LED_PIN3;
        GPIO_ResetBits(F_PORT, previusLed);
        GPIO_SetBits(F_PORT,currentLed);
        nextLed = LED_PIN4;
      } else if (nextLed == LED_PIN4) {
        currentLed = LED_PIN1;
        previusLed = LED_PIN2;
        GPIO_ResetBits(F_PORT, previusLed);
        GPIO_SetBits(F_PORT,currentLed);
        nextLed = LED_PIN3;
      } else if (nextLed == LED_PIN3) {
        currentLed = LED_PIN4;
        previusLed = LED_PIN1;
        GPIO_ResetBits(F_PORT, previusLed);
        GPIO_SetBits(F_PORT,currentLed);
        nextLed = LED_PIN2;
      }else if (nextLed == LED_PIN2) {
        currentLed = LED_PIN3;
        previusLed = LED_PIN4;
        GPIO_ResetBits(F_PORT, previusLed);
        GPIO_SetBits(F_PORT,currentLed);
        nextLed = LED_PIN1;
      }
     
     
      while (!flag) {
        flag = GPIO_ReadInputDataBit(GPIOG, USER_BTN);
      }
    }
    if (flag1){
       if (nextLed == LED_PIN1) {
        currentLed = LED_PIN4;
        previusLed = LED_PIN3;
        GPIO_ResetBits(F_PORT, previusLed);
        GPIO_SetBits(F_PORT,currentLed);
        nextLed = LED_PIN2;
      } else if (nextLed == LED_PIN2) {
        currentLed = LED_PIN1;
        previusLed = LED_PIN4;
        GPIO_ResetBits(F_PORT, previusLed);
        GPIO_SetBits(F_PORT,currentLed);
        nextLed = LED_PIN3;
      } else if (nextLed == LED_PIN3) {
        currentLed = LED_PIN2;
        previusLed = LED_PIN1;
        GPIO_ResetBits(F_PORT, previusLed);
        GPIO_SetBits(F_PORT,currentLed);
        nextLed = LED_PIN4;
      } else if (nextLed == LED_PIN4) {
        currentLed = LED_PIN3;
        previusLed = LED_PIN2;
        GPIO_ResetBits(F_PORT, previusLed);
        GPIO_SetBits(F_PORT,currentLed);
        nextLed = LED_PIN1;
      }
      
      while (flag1) {
        flag1 = GPIO_ReadInputDataBit(GPIOA, WKUP_BTN);
      }
    }
  }
}
static void Delay (uint32_t delay){
    while (--delay) {
      __NOP();
    }
  }

