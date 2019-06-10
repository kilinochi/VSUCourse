#ifndef _BOARD_CONFIG_H
#define _BOARD_CONFIG_H

#ifdef _STM_DISCOVERY_BOARD_
  #define F_ROC       RCC_AHB1Periph_GPIOD
  #define F_PORT      GPIOD
  #define LED_PIN1       GPIO_Pin_6
  #define LED_PIN2       GPIO_Pin_7
  #define LED_PIN3       GPIO_Pin_8
  #define LED_PIN4       GPIO_Pin_9 


#endif

#ifdef _STM_IAR_BOARD_
  #define F_ROC       RCC_AHB1Periph_GPIOF
  #define F_PORT      GPIOF
  #define G_ROC       RCC_AHB1Periph_GPIOG
  #define G_PORT      GPIOG
  #define A_RCC       RCC_AHB1Periph_GPIOA
  #define A_PORT      GPIOA
  #define LED_PIN1       GPIO_Pin_6
  #define LED_PIN2       GPIO_Pin_7
  #define LED_PIN3       GPIO_Pin_8
  #define LED_PIN4       GPIO_Pin_9
#define LED_PIN5        GPIO_Pin_10
  #define USER_BTN       GPIO_Pin_6
  #define WKUP_BTN       GPIO_Pin_0


#endif


#endif