#include "driver/ledc.h"

#include "ledRgbIrrigacao.h"

//Função para configuração do LED de irrigação e do timer
void setupLEDIrrigacao()
{
  //Configuração do timer LED Control
  ledc_timer_config_t ledc_timer = {
    .duty_resolution = LEDC_TIMER_BIT,
    .freq_hz = LEDC_BASE_FREQ,
    .speed_mode = LEDC_SPEED_MODE,
    .timer_num = LEDC_TIMER
  };
  ledc_timer_config(&ledc_timer);

  //Configuração do LED de irrigação
  ledc_channel_config_t ledc_channel[1] = {
    {
      .gpio_num = LED_IRRIGACAO_PIN,
      .speed_mode = LEDC_SPEED_MODE,
      .channel = LEDC_CHANNEL_IRRIGACAO,
      .intr_type = LEDC_INTR_DISABLE,
      .timer_sel = LEDC_TIMER,
      .duty = 0,
      .hpoint = 0
    }
  };

    ledc_channel_config(&ledc_channel[0]);
    ledc_set_duty(LEDC_SPEED_MODE, LEDC_CHANNEL_IRRIGACAO, 256);
    ledc_update_duty(LEDC_SPEED_MODE, LEDC_CHANNEL_IRRIGACAO);
}

//Função que atualiza a intensidade do LED de irrigação
void setLEDIrrigacao(uint32_t irrigacao)
{
  ledc_set_duty(LEDC_SPEED_MODE, LEDC_CHANNEL_IRRIGACAO, irrigacao);
  ledc_update_duty(LEDC_SPEED_MODE, LEDC_CHANNEL_IRRIGACAO);
}