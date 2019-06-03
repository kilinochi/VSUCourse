#ifndef _BOARD_CONFIG_H
#define _BOARD_CONFIG_H

#ifdef _STM_DISCOVERY_BOARD_
 #define LED_RCC       RCC_AHB1Periph_GPIOF
  #define LED_PORT      GPIOF
  #define BUTTON_WKUP_RCC       RCC_AHB1Periph_GPIOG
  #define BUTTON_WKUP_PORT      GPIOG
  #define BUTTON_USER_RCC       RCC_AHB1Periph_GPIOA
  #define BUTTON_USER_PORT      GPIOA
  #define LED_PIN_1       GPIO_Pin_6
  #define LED_PIN_2       GPIO_Pin_7
  #define LED_PIN_3       GPIO_Pin_8
  #define LED_PIN_4       GPIO_Pin_9
#define LED_PIN5        GPIO_Pin_10
  #define BUTTON_USER_PIN       GPIO_Pin_6
  #define BUTTON_WKUP_PIN       GPIO_Pin_0


#endif

#ifdef _STM_IAR_BOARD_
  #define LED_RCC       RCC_AHB1Periph_GPIOF
  #define LED_PORT      GPIOF
  #define BUTTON_WKUP_RCC       RCC_AHB1Periph_GPIOG
  #define BUTTON_WKUP_PORT      GPIOG
  #define BUTTON_USER_RCC       RCC_AHB1Periph_GPIOA
  #define BUTTON_USER_PORT      GPIOA
  #define LED_PIN_1       GPIO_Pin_6
  #define LED_PIN_2       GPIO_Pin_7
  #define LED_PIN_3       GPIO_Pin_8
  #define LED_PIN_4       GPIO_Pin_9
#define LED_PIN5        GPIO_Pin_10
  #define BUTTON_USER_PIN       GPIO_Pin_6
  #define BUTTON_WKUP_PIN       GPIO_Pin_0


#endif


#endif