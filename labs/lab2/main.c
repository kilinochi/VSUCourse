#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stm32f4xx_tim.h"
#include "misc.h"



#define _STM_IAR_BOARD_
#include "board_config.h"

#define DELAY 500

static void Delay (uint32_t delay);
void TIM6_DAC_IRQHandler(void);
void RCC_WaitForPLLStartUp(void);
void TIM1_UP_TIM10_IRQHandler(void);
int HSE_my (void);
int HSE_PLL_my (void);
void BTN_and_LED_init (void);
void TIM6_init (void);

int main() {

  /* сброс до таймера HSI и 16 МГц */
  RCC_DeInit();
  //SystemCoreClockUpdate();
  /* включение HSE 25MHz */
 // HSE_my();
  
  
  /* настройка светодиодов и кнопок */
  BTN_and_LED_init();
  
  /* For шим */
  
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource8,GPIO_AF_TIM13);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE);
  
  // Блок инициализации структур для конфигурации таймера,
  // контроллера прерываний и выходного канала таймера
 TIM_TimeBaseInitTypeDef TIM_BaseStruct;
 
 TIM_BaseStruct.TIM_Prescaler = 0;
 TIM_BaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
 TIM_BaseStruct.TIM_Period = 65535; /* 10kHz PWM */
  TIM_BaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_BaseStruct.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM13, &TIM_BaseStruct);
   TIM_Cmd(TIM13, ENABLE);
   
   TIM_OCInitTypeDef TIM_OCStruct;
   
   TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_Low;
    
    TIM_OCStruct.TIM_Pulse = 10000; /* 25% duty cycle */
    TIM_OC1Init(TIM13, &TIM_OCStruct);
    TIM_OC1PreloadConfig(TIM13, TIM_OCPreload_Enable);
  
  uint8_t readbit = 0x00;
  uint32_t TIM_PULSE = 10000;
  while(1){
      readbit = GPIO_ReadInputDataBit(BUTTON_USER_PORT, BUTTON_USER_PIN);
      while (!readbit) {
        if (TIM_PULSE != 0) {
          TIM_PULSE -= 1;
          TIM_SetCompare1(TIM10, TIM_PULSE);
        }
        Delay(DELAY);
        readbit = GPIO_ReadInputDataBit(BUTTON_USER_PORT, BUTTON_USER_PIN);
      }
      readbit = GPIO_ReadInputDataBit(BUTTON_WKUP_PORT, BUTTON_WKUP_PIN);
      while (readbit) {
        if (TIM_PULSE != 65535) {
          TIM_PULSE += 1;
          TIM_SetCompare1(TIM10, TIM_PULSE);
        }
        Delay(DELAY);
        readbit = GPIO_ReadInputDataBit(BUTTON_WKUP_PORT, BUTTON_WKUP_PIN);        
      }
  }
}

void TIM6_DAC_IRQHandler() {
  if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET) {
  // USER CODE HERE
  GPIO_ToggleBits(LED_PORT, LED_PIN_1);
    
  // Clear IRQ bit
  TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
  }
}
void RCC_WaitForPLLStartUp(void) {
  while ( (RCC->CR & RCC_CR_PLLRDY) == 0 ) {
    __NOP();
  }
}
void TIM1_UP_TIM10_IRQHandler() {
  if (TIM_GetITStatus(TIM10, TIM_IT_Update) != RESET) {
    ///GPIO_ToggleBits(LED_PORT, LED_PIN_2);
    TIM_ClearITPendingBit(TIM10, TIM_IT_Update);
  }
}
int HSE_my (void) {
  /* For HSE 25 + TIM6 */ 
  // Разрешение тактирования внешнего генератора частоты
  RCC_HSEConfig(RCC_HSE_ON);
  // Условие прерывает работу функции настройки системной частоты,
  // при ошибке настройки внешнего генератора
  if (RCC_WaitForHSEStartUp() == ERROR) {
    return 1;
  }
  RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);
  return 0;
}
int HSE_PLL_my (void) { 
  // Условие прерывает работу функции настройки системной частоты,
  // при ошибке настройки внешнего генератора
  RCC_HSEConfig(RCC_HSE_ON);
  if (RCC_WaitForHSEStartUp() == ERROR) {
    return 1;
  }
   // Конфигурация непосредственно PLL (4)
  RCC_PLLConfig(RCC_PLLSource_HSE, 25, 336, 2, 5);
  // Разрешение работы PLL
  RCC_PLLCmd(ENABLE);
  // Пользовательская функция, проверяет корректность работы PLL
  // путем проверки соответствующих флагов
  RCC_WaitForPLLStartUp();
  // Установка частоты шин AHB1 и APB1
  RCC_HCLKConfig(RCC_SYSCLK_Div2);
  RCC_PCLK1Config(RCC_HCLK_Div1);
  // Установка в качестве системной тактовой частоты PLL
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  return 0;
}
void BTN_and_LED_init (void) {
  /* For LED init */
  
  GPIO_InitTypeDef	GPIO_InitStruct;
  
  RCC_AHB1PeriphClockCmd(LED_RCC, ENABLE);

  GPIO_InitStruct.GPIO_Pin = LED_PIN_1 | LED_PIN_2 | LED_PIN_3 | LED_PIN_4;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
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
void TIM6_init (void) {
  /* For TIM6 */
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
  
  // Инициализация структуры для конфигурирования таймеров
  TIM_TimeBaseInitTypeDef TIM_INITStruct;
  // Заполняет поля структуры значениями по умолчанию
  TIM_TimeBaseStructInit(&TIM_INITStruct);
  // Задание частоты работы таймера, относительно частоты
  // шины, к которой подключен таймер (значение от 0x0000 до
  // 0xFFFF, поэтому с -1
  ///TIM_INITStruct.TIM_Prescaler = 0x0000;
  TIM_INITStruct.TIM_Prescaler = 84 - 1;
  // Задание длительности генерируемого импульса
  TIM_INITStruct.TIM_Period = 1000/2;
  // Инициализация настроек таймера
  TIM_TimeBaseInit(TIM6, &TIM_INITStruct);
  // Разрешение прерывания от таймера 6 по переполнению (более
  // детально работа прерываний будет рассмотрена позднее)
  TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
  // Включение таймера
  TIM_Cmd(TIM6, ENABLE);
  // Разрешение обработки прерывания от таймера
  NVIC_EnableIRQ(TIM6_DAC_IRQn);
}
static void Delay (uint32_t delay) {
  while (--delay) {
    __NOP();
  }
}

