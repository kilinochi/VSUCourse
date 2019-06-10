#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_adc.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


#define _STM_IAR_BOARD_
#include "board_config.h"

#define DELAY 1000

static void Delay (uint32_t delay);
void BTN_and_LED_init (void);
void EXTI0_IRQnHandler(void);
void button_interrupt();

int main() {
  
  /* Leds and Buttons settings */
  BTN_and_LED_init(); 
  
  button_interrupt();
  
  while(1){
      GPIO_ToggleBits(LED_PORT, LED_PIN_1);
      Delay(1000);
  }
}

void button_interrupt() {
  EXTI_DeInit();
  
  // Выбор порта и пина для использующегося как EXTI
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
  // Cтруктура для настройки
  EXTI_InitTypeDef exti;
  // Выбор линии для которой настраивается порождение прерывания
  exti.EXTI_Line = EXTI_Line0;
  // Порождается прерывание (а не событие)
  exti.EXTI_Mode = EXTI_Mode_Interrupt;
  // Прерывание генерируется по переднему фронту импульса
  exti.EXTI_Trigger = EXTI_Trigger_Rising;
  // Разрешается генерация прерывания
  exti.EXTI_LineCmd = ENABLE;
  EXTI_Init(&exti);
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  // Инициализация структуры для настройки контроллера прерываний
  NVIC_InitTypeDef NVIC_INITStruct;
  // Выбор канала контроллера прерываний для настройки
  // В данном случае настраиваются прерывания от 6ого таймера
  NVIC_INITStruct.NVIC_IRQChannel = EXTI0_IRQn;
  // Настройка приоритетов в диапазоне от 0 до 15
  NVIC_INITStruct.NVIC_IRQChannelPreemptionPriority = 0; 
  NVIC_INITStruct.NVIC_IRQChannelSubPriority = 0;
  // Разрешение обработки прерывания выбранного канала
  NVIC_INITStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_INITStruct);
  NVIC_EnableIRQ (EXTI0_IRQn);
  
  //EXTI_GenerateSWInterrupt(EXTI_Line0);

}

void EXTI0_IRQnHandler() {
  if( EXTI_GetITStatus(EXTI_Line0) != RESET) {
    GPIO_ToggleBits(LED_PORT, LED_PIN_1);
    Delay(500000);
  }
  EXTI_ClearITPendingBit(EXTI_Line0);
}

void BTN_and_LED_init (void) {
  /* For LED init */
  
  GPIO_InitTypeDef	GPIO_InitStruct;
  
  RCC_AHB1PeriphClockCmd(LED_RCC, ENABLE);

  GPIO_InitStruct.GPIO_Pin = LED_PIN_1 | LED_PIN_2 | LED_PIN_3 | LED_PIN_4;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

  GPIO_Init(LED_PORT, &GPIO_InitStruct);
  
  /* FOR User Button */
    
  RCC_AHB1PeriphClockCmd(BUTTON_USER_RCC, ENABLE);
  
  GPIO_InitStruct.GPIO_Pin = BUTTON_USER_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  
  GPIO_Init(BUTTON_USER_PORT, &GPIO_InitStruct);
 
  /* FOR WKUP */
  
  RCC_AHB1PeriphClockCmd(BUTTON_WKUP_RCC, ENABLE);
  
  GPIO_InitStruct.GPIO_Pin = BUTTON_WKUP_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  
  GPIO_Init(BUTTON_WKUP_PORT, &GPIO_InitStruct);
  
  
}

static void Delay (uint32_t delay) {
  while (--delay) {
    __NOP();
  }
}

