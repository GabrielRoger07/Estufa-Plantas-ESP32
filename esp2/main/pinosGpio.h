#ifndef PINOSGPIO_H

#define PINOSGPIO_H

// defines para o led rgb, que funciona via PWM
#define LED_RED_PIN 17
#define LED_GREEN_PIN 22
#define LED_BLUE_PIN 23

#define LEDC_CHANNEL_RED LEDC_CHANNEL_0
#define LEDC_CHANNEL_GREEN LEDC_CHANNEL_1
#define LEDC_CHANNEL_BLUE LEDC_CHANNEL_2

#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_SPEED_MODE LEDC_HIGH_SPEED_MODE
#define LEDC_TIMER_BIT LEDC_TIMER_8_BIT
#define LEDC_BASE_FREQ 256

//#define BOTAO 0 para acordar a esp
#define BOTAO GPIO_NUM_0

// defines dos pinos do encoder para o controle da intensidade do led
#define PIN_DT GPIO_NUM_19
#define PIN_CLK GPIO_NUM_18
#define PIN_SW GPIO_NUM_21

// defines dos pinos do encoder para o controle do sistema de ventilação
#define pinoDt GPIO_NUM_5
#define pinoClk GPIO_NUM_2
#define pinoSw GPIO_NUM_4

#endif