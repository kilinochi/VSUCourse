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
void TIM1_UP_TIM13_IRQHandler(void);
int HSE_my (void);
int HSE_PLL_my (void);
void BTN_and_LED_init (void);
void TIM6_init (void);

int main() {

   // RCC_DeInit();
  //HSE_init();
  SystemCoreClockUpdate();
  /* setting buttons and  */
  BTN_and_LED_init();
  /* For PWM */
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource8,GPIO_AF_TIM13);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM13, ENABLE);
  
  // Block initialization structures for the configuration of the timer
  // interrupt controller and timer channel output
  TIM_TimeBaseInitTypeDef TIM_INITStruct;
  TIM_OCInitTypeDef TIM_OUTStruct;
  NVIC_InitTypeDef NVIC_INITStruct;
  TIM_TimeBaseStructInit(&TIM_INITStruct);
  TIM_INITStruct.TIM_Prescaler = 0;
  TIM_INITStruct.TIM_Period = 65535;
  //Configuration of the counter, for the account to increase
  TIM_INITStruct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM13, &TIM_INITStruct);
  // Block initialization fields to configure the controller interrupt
  // Enable overflow interrupt from the 13th timer
  NVIC_INITStruct.NVIC_IRQChannel = TIM1_UP_TIM13_IRQn;
  NVIC_INITStruct.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_INITStruct.NVIC_IRQChannelSubPriority = 0;
  NVIC_INITStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_INITStruct);
  // Channel operation mode - PWM generation
  TIM_OUTStruct.TIM_OCMode = TIM_OCMode_PWM1;
  // Enable the operation of the timer output channel
  TIM_OUTStruct.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OUTStruct.TIM_OCPolarity = TIM_OCPolarity_High;
  // Pulse length (in cycles)
  TIM_OUTStruct.TIM_Pulse = 10000;
  // Initialize timer settings
  TIM_OC1Init(TIM13, &TIM_OUTStruct);
  // Enable interrupt handling from overflow timer 13
  TIM_ITConfig(TIM13, TIM_IT_Update,ENABLE);
  // Enable work timer
  TIM_Cmd(TIM13, ENABLE);
  
  uint8_t readbit = 0x00;
  uint32_t TIM_PULSE = 10000;
  while(1){
      readbit = GPIO_ReadInputDataBit(BUTTON_USER_PORT, BUTTON_USER_PIN);
      while (!readbit) {
        if (TIM_PULSE != 0) {
          TIM_PULSE -= 1;
          TIM_SetCompare1(TIM13, TIM_PULSE);
        }
        Delay(DELAY);
        readbit = GPIO_ReadInputDataBit(BUTTON_USER_PORT, BUTTON_USER_PIN);
      }
      readbit = GPIO_ReadInputDataBit(BUTTON_WKUP_PORT, BUTTON_WKUP_PIN);
      while (readbit) {
        if (TIM_PULSE != 65535) {
          TIM_PULSE += 1;
          TIM_SetCompare1(TIM13, TIM_PULSE);
        }
        Delay(DELAY);
        readbit = GPIO_ReadInputDataBit(BUTTON_WKUP_PORT, BUTTON_WKUP_PIN);        
      }
  }
}

void TIM6_DAC_IRQHandler() {
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET) {
		GPIO_ToggleBits(LED_PORT, LED_PIN_1);
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);	
	}	
}

void RCC_WaitForPLLStartUp(void) {
  while ( (RCC->CR & RCC_CR_PLLRDY) == 0 ) {
    __NOP();
  }
}
void TIM8_UP_TIM13_IRQHandler() {
  if (TIM_GetITStatus(TIM13, TIM_IT_Update) != RESET) {
    ///GPIO_ToggleBits(LED_PORT, LED_PIN_2);
    TIM_ClearITPendingBit(TIM13, TIM_IT_Update);
  }
}

void HSE_init() {
	RCC_HSEConfig(RCC_HSE_ON);
	if (RCC_WaitForHSEStartUp() == ERROR) {
		return;
	}
	RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);
}

void PLL_init () { 
	// The condition interrupts the function of setting the system frequency,
	// if the external generator setting fails
	RCC_HSEConfig(RCC_HSE_ON);
	if (RCC_WaitForHSEStartUp() == ERROR) {
		return;
	}
	// PLL configuration
	RCC_PLLConfig(RCC_PLLSource_HSE, 25, 336, 2, 5);
	// Enable PLL
	RCC_PLLCmd(ENABLE);
	RCC_WaitForPLLStartUp();
	// Set the bus frequency AHB1 and APB1
	RCC_HCLKConfig(RCC_SYSCLK_Div2);
	RCC_PCLK1Config(RCC_HCLK_Div1);
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
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
  TIM_TimeBaseInitTypeDef TIM_INITStruct;
  TIM_TimeBaseStructInit(&TIM_INITStruct);
  ///TIM_INITStruct.TIM_Prescaler = 0x0000;
  TIM_INITStruct.TIM_Prescaler = 25 - 1;
  TIM_INITStruct.TIM_Period = 1000/2;
  TIM_TimeBaseInit(TIM6, &TIM_INITStruct);
  TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM6, ENABLE);
  NVIC_EnableIRQ(TIM6_DAC_IRQn);
}
static void Delay (uint32_t delay) {
  while (--delay) {
    __NOP();
  }
}