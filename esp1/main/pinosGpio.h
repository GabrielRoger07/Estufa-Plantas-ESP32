#ifndef PINOSGPIO_H
#define PINOSGPIO_H

// Setando os pinos da placa que serão usados

#define LED_RED_PIN   17
#define LED_GREEN_PIN 18
#define LED_BLUE_PIN  19
#define BUZZER_PIN    22

#define LED_IRRIGACAO_PIN 16

// Constantes que serão usadas nos sensores em que há o PWM (led da temperatura e umidade, led da irrigação e buzzer)

#define LED_

#define LEDC_CHANNEL_RED        LEDC_CHANNEL_0
#define LEDC_CHANNEL_GREEN      LEDC_CHANNEL_1
#define LEDC_CHANNEL_BLUE       LEDC_CHANNEL_2
#define BUZZER_CHANNEL          LEDC_CHANNEL_3
#define LEDC_CHANNEL_IRRIGACAO  LEDC_CHANNEL_4

#define LEDC_TIMER        LEDC_TIMER_0
#define LEDC_SPEED_MODE   LEDC_HIGH_SPEED_MODE
#define LEDC_TIMER_BIT    LEDC_TIMER_8_BIT
#define LEDC_BASE_FREQ    256

#endif