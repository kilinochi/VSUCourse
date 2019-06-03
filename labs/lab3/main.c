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
void TIM3_IRQHandler(void);
void BTN_and_LED_init (void);
void TIM3_Init(void);
void TIM4_Init(void);
void TIM4_IRQHandler(void);
void TIM3_TIM4_Priority(void);
void task_priority(void);
void EXTI9_5_IRQHandler(void);
u16 readADC1(u8 channel);
void adc_init();



int main() {
  /* Leds and Buttons settings */
  BTN_and_LED_init(); 
  /* for ADC */ 
  ADC_INIT();
  /* enable temperature sensor */
  ADC_TempSensorVrefintCmd(ENABLE);
  int channel = 16;
  while(1){
    uint16_t temperature = readADC1(channel);
    GPIO_ResetBits(LED_PORT, LED_PIN_1 | LED_PIN_2 | LED_PIN_3 | LED_PIN_4);
    if (temperature > 960) {  /// если больше 20 зажигаем первый диод
          GPIO_SetBits(LED_PORT, LED_PIN_1);
    }
    if (temperature > 970) { // если больше 30 - второй. и т.д.
        GPIO_SetBits(LED_PORT, LED_PIN_2);
    }
    if (temperature > 980) {
        GPIO_SetBits(LED_PORT, LED_PIN_3);
    }
    if (temperature > 990) {
        GPIO_SetBits(LED_PORT, LED_PIN_4);
    }
    Delay(5000000); 
  }
}

void adc_leds_poten_control() {
  int channel = 10;
  uint16_t count = readADC1(channel);
  while(1){
    if (abs(count - readADC1(channel)) >= 20)  {
      count = readADC1(channel);
      GPIO_ResetBits(LED_PORT, LED_PIN_1 | LED_PIN_2 | LED_PIN_3 | LED_PIN_4);
      if ((count > 13) && (count < 1024)) {
        GPIO_SetBits(LED_PORT, LED_PIN_1);
      }
      if ((count > 1023) && (count < 2048)) {
          GPIO_SetBits(LED_PORT, LED_PIN_1 | LED_PIN_2);
      }
      if ((count > 2047) && (count < 3072)) {
        GPIO_SetBits(LED_PORT, LED_PIN_1 | LED_PIN_2 | LED_PIN_3);
      }
      if ((count > 3071) && (count < 4096)) {
        GPIO_SetBits(LED_PORT, LED_PIN_1 | LED_PIN_2 | LED_PIN_3 | LED_PIN_4);
      }
      
    }
  }
  
}

void adc_init() {
       ADC_InitTypeDef ADC_InitStructure;
       ADC_StructInit(&ADC_InitStructure);
 
       ADC_CommonInitTypeDef adc_init;
       ADC_CommonStructInit(&adc_init);
       /* разрешаем тактирование AЦП1 */
       RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
       /* сбрасываем настройки АЦП */
       ADC_DeInit();
 
       /* АЦП1 и АЦП2 работают независимо */
       adc_init.ADC_Mode = ADC_Mode_Independent;
       adc_init.ADC_Prescaler = ADC_Prescaler_Div2;
       /* выключаем scan conversion */
       ADC_InitStructure.ADC_ScanConvMode = DISABLE;
       /* Не делать длительные преобразования */
       ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
 
       /* Начинать преобразование программно, а не по срабатыванию триггера */
       ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;
       /* 12 битное преобразование. результат в 12 младших разрядах результата */
       ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
 
       /* инициализация */
       ADC_CommonInit(&adc_init);
 
       ADC_Init(ADC1, &ADC_InitStructure);
       /* Включаем АЦП1 */
       ADC_Cmd(ADC1, ENABLE);
}

u16 readADC1(u8 channel){
  // Данные преобразуются на АЦП1, с переданного канала, c
  // первым приоритетом преобразования, преобразование не должно
  // превышать 28 циклов работы АЦП
  ADC_RegularChannelConfig(ADC1, channel, 1,ADC_SampleTime_28Cycles);
  // Разрешение работы АЦП1
  ADC_SoftwareStartConv(ADC1);
  // Ожидание конца преобразования
  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
  // Сохранение результата в u16 переменную
  return ADC_GetConversionValue(ADC1);
}

void button_interrupt() {
   /* Leds and Buttons settings */
  //BTN_and_LED_init(); 
  
  GPIO_ResetBits(LED_PORT, LED_PIN_2 | LED_PIN_3 | LED_PIN_4);
  GPIO_SetBits(LED_PORT, LED_PIN_1);
  
  /* For USER irq */
  
  EXTI_DeInit();
  
  // Выбор порта и пина для использующегося как EXTI
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource6);
  // Cтруктура для настройки
  EXTI_InitTypeDef exti;
  // Выбор линии для которой настраивается порождение прерывания
  exti.EXTI_Line = EXTI_Line6;
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
  NVIC_INITStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
  // Настройка приоритетов в диапазоне от 0 до 15
  NVIC_INITStruct.NVIC_IRQChannelPreemptionPriority = 0; //
  NVIC_INITStruct.NVIC_IRQChannelSubPriority = 0;
  // Разрешение обработки прерывания выбранного канала
  NVIC_INITStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_INITStruct);
  NVIC_EnableIRQ (EXTI9_5_IRQn);
  
  EXTI_GenerateSWInterrupt(EXTI_Line6);
  EXTI_GenerateSWInterrupt(EXTI_Line6);
  EXTI_GenerateSWInterrupt(EXTI_Line6);
}

void EXTI10_IRQn_Handler() {
  if( EXTI_GetITStatus(EXTI_Line6) != RESET) {
    GPIO_ToggleBits(LED_PORT, LED_PIN_1);
  }
  EXTI_ClearITPendingBit(EXTI_Line6);
}


void task_priority() {
  /* Leds and Buttons settings */
  BTN_and_LED_init();
  
  /* For TIM3 */
  TIM3_Init();
  
  /* For TIM4 */
  TIM4_Init();
  
  /* For Priority */
  TIM3_TIM4_Priority();
}
  

void TIM3_TIM4_Priority() {
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  // Инициализация структуры для настройки контроллера прерываний
  NVIC_InitTypeDef NVIC_INITStruct;
  
  /* Priority TIM3 */
  // Выбор канала контроллера прерываний для настройки
  // В данном случае настраиваются прерывания от 6ого таймера
  NVIC_INITStruct.NVIC_IRQChannel = TIM3_IRQn;
  // Настройка приоритетов в диапазоне от 0 до 15
  NVIC_INITStruct.NVIC_IRQChannelPreemptionPriority = 0; //
  NVIC_INITStruct.NVIC_IRQChannelSubPriority = 0;
  // Разрешение обработки прерывания выбранного канала
  NVIC_INITStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_INITStruct);

  
  /* Priority TIM4 */
  // Инициализация структуры для настройки контроллера прерываний
  NVIC_InitTypeDef NVIC_INITStruct1;
  // Выбор канала контроллера прерываний для настройки
  // В данном случае настраиваются прерывания от 7ого таймера
  NVIC_INITStruct1.NVIC_IRQChannel = TIM4_IRQn;
  // Настройка приоритетов в диапазоне от 0 до 15
  NVIC_INITStruct1.NVIC_IRQChannelPreemptionPriority = 3; //
  NVIC_INITStruct1.NVIC_IRQChannelSubPriority = 0;
  // Разрешение обработки прерывания выбранного канала
  NVIC_INITStruct1.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_INITStruct1);

}

void TIM3_Init(){
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  // Инициализация структуры для конфигурирования таймеров
  TIM_TimeBaseInitTypeDef TIM_INITStruct;
  // Заполняет поля структуры значениями по умолчанию
  TIM_TimeBaseStructInit(&TIM_INITStruct);
  // Задание частоты работы таймера, относительно частоты
  // шины, к которой подключен таймер (значение от 0x0000 до
  // 0xFFFF, поэтому с -1
  ///TIM_INITStruct.TIM_Prescaler = 0x0000;
  TIM_INITStruct.TIM_Prescaler = 25-1;
  // Задание длительности генерируемого импульса
  TIM_INITStruct.TIM_Period = 2000;
  // Инициализация настроек таймера
  TIM_TimeBaseInit(TIM3, &TIM_INITStruct);
  // Разрешение прерывания от таймера 6 по переполнению (более
  // детально работа прерываний будет рассмотрена позднее)
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
  // Включение таймера
  TIM_Cmd(TIM3, ENABLE);
  // Разрешение обработки прерывания от таймера
  NVIC_EnableIRQ(TIM3_IRQn);
}  

void TIM4_Init(){
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  // Инициализация структуры для конфигурирования таймеров
  TIM_TimeBaseInitTypeDef TIM_INITStruct;
  // Заполняет поля структуры значениями по умолчанию
  TIM_TimeBaseStructInit(&TIM_INITStruct);
  // Задание частоты работы таймера, относительно частоты
  // шины, к которой подключен таймер (значение от 0x0000 до
  // 0xFFFF, поэтому с -1
  ///TIM_INITStruct.TIM_Prescaler = 0x0000;
  TIM_INITStruct.TIM_Prescaler = 25-1;
  // Задание длительности генерируемого импульса
  TIM_INITStruct.TIM_Period = 500;
  // Инициализация настроек таймера
  TIM_TimeBaseInit(TIM4, &TIM_INITStruct);
  // Разрешение прерывания от таймера 6 по переполнению (более
  // детально работа прерываний будет рассмотрена позднее)
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
  // Включение таймера
  TIM_Cmd(TIM4, ENABLE);
  // Разрешение обработки прерывания от таймера
  NVIC_EnableIRQ(TIM4_IRQn);
}

void TIM4_IRQHandler() {
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) {
  // USER CODE HERE
    for (int i = 0; i < 20; i++) {
      GPIO_ToggleBits(LED_PORT, LED_PIN_4);
      Delay(50);
    }
    GPIO_ResetBits(LED_PORT, LED_PIN_4);
    
    // Clear IRQ bit
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
  }
}

void TIM3_IRQHandler() {
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
    // USER CODE HERE
    for (int i = 0; i < 160; i++) {
      GPIO_ToggleBits(LED_PORT, LED_PIN_1);
      Delay(50);
    }
    GPIO_ResetBits(LED_PORT, LED_PIN_1);
    
    // Clear IRQ bit
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
  }
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

