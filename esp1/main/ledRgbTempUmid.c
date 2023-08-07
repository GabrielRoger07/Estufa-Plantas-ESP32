#include "driver/ledc.h"

#include "ledRgbTempUmid.h"

//Função para configuração dos canais do LED RGB e o timer
void setupLEDTempUmid()
{
  //Configuração do timer LED Control
  ledc_timer_config_t ledc_timer = {
    .duty_resolution = LEDC_TIMER_BIT,
    .freq_hz = LEDC_BASE_FREQ,
    .speed_mode = LEDC_SPEED_MODE,
    .timer_num = LEDC_TIMER
  };
  ledc_timer_config(&ledc_timer);

  //Configuração dos canais do LED RGB
  ledc_channel_config_t ledc_channel[3] = {
    {
      .gpio_num = LED_RED_PIN,
      .speed_mode = LEDC_SPEED_MODE,
      .channel = LEDC_CHANNEL_RED,
      .intr_type = LEDC_INTR_DISABLE,
      .timer_sel = LEDC_TIMER,
      .duty = 0,
      .hpoint = 0
    },
    {
      .gpio_num = LED_GREEN_PIN,
      .speed_mode = LEDC_SPEED_MODE,
      .channel = LEDC_CHANNEL_GREEN,
      .intr_type = LEDC_INTR_DISABLE,
      .timer_sel = LEDC_TIMER,
      .duty = 0,
      .hpoint = 0
    },
    {
      .gpio_num = LED_BLUE_PIN,
      .speed_mode = LEDC_SPEED_MODE,
      .channel = LEDC_CHANNEL_BLUE,
      .intr_type = LEDC_INTR_DISABLE,
      .timer_sel = LEDC_TIMER,
      .duty = 0,
      .hpoint = 0
    }
  };

  for (int i = 0; i < 3; i++) {
    ledc_channel_config(&ledc_channel[i]);
    ledc_set_duty(LEDC_SPEED_MODE, ledc_channel[i].channel, 0);
    ledc_update_duty(LEDC_SPEED_MODE, ledc_channel[i].channel);
  }
}

//Função que atualiza a intensidade dos LEDs vermelho, verde e azul
void setLEDTempUmidIntensity(uint32_t red, uint32_t green, uint32_t blue)
{
  ledc_set_duty(LEDC_SPEED_MODE, LEDC_CHANNEL_RED, red);
  ledc_set_duty(LEDC_SPEED_MODE, LEDC_CHANNEL_GREEN, green);
  ledc_set_duty(LEDC_SPEED_MODE, LEDC_CHANNEL_BLUE, blue);
  ledc_update_duty(LEDC_SPEED_MODE, LEDC_CHANNEL_RED);
  ledc_update_duty(LEDC_SPEED_MODE, LEDC_CHANNEL_GREEN);
  ledc_update_duty(LEDC_SPEED_MODE, LEDC_CHANNEL_BLUE);
}