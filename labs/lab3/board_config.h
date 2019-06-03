#ifndef _BOARD_CONFIG_H
#define _BOARD_CONFIG_H

#ifdef _STM_DISCOVERY_BOARD_
  #define LED_RCC       RCC_AHB1Periph_GPIOD
  #define LED_PORT      GPIOD
  #define LED_PIN       GPIO_Pin_12


#endif

#ifdef _STM_IAR_BOARD_
  #define LED_RCC       RCC_AHB1Periph_GPIOF
  #define LED_PORT      GPIOF
  #define LED_PIN_1     GPIO_Pin_6
  #define LED_PIN_2     GPIO_Pin_7    
  #define LED_PIN_3     GPIO_Pin_8
  #define LED_PIN_4     GPIO_Pin_9

  #define BUTTON_USER_RCC       RCC_AHB1Periph_GPIOG
  #define BUTTON_USER_PORT      GPIOG
  #define BUTTON_USER_PIN       GPIO_Pin_6

  #define BUTTON_WKUP_RCC       RCC_AHB1Periph_GPIOA
  #define BUTTON_WKUP_PORT      GPIOA
  #define BUTTON_WKUP_PIN       GPIO_Pin_0

  #define JOYSTICK_RCC          RCC_AHB1Periph_GPIOG
  #define JOYSTICK_PORT         GPIOG
  #define JOYSTICK_UP           GPIO_Pin_7
  #define JOYSTICK_DOWN         GPIO_Pin_8
  #define JOYSTICK_LEFT         GPIO_Pin_11
  #define JOYSTICK_RIGHT        GPIO_Pin_12

#endif


#endif