#include "driver/ledc.h"

#include "buzzer.h"

//Função para configuração do BUZZER e do timer
void setupBuzzer()
{
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_BIT,
        .freq_hz = LEDC_BASE_FREQ,
        .speed_mode = LEDC_SPEED_MODE,
        .timer_num = LEDC_TIMER
    };
    ledc_timer_config(&ledc_timer);

    //Configurações do BUZZER
    ledc_channel_config_t ledc_channel[1] = {
      {
          .gpio_num = BUZZER_PIN,
          .speed_mode = LEDC_SPEED_MODE,
          .channel = BUZZER_CHANNEL,
          .intr_type = LEDC_INTR_DISABLE,
          .timer_sel = LEDC_TIMER,
          .duty = 0,
          .hpoint = 0
      }
    };

    ledc_channel_config(&ledc_channel[0]);
    ledc_set_duty(LEDC_SPEED_MODE, BUZZER_CHANNEL, 0);
    ledc_update_duty(LEDC_SPEED_MODE, BUZZER_CHANNEL);
}

//Função que atualiza a intensidade do som do BUZZER
void setBuzzerIntensity(uint32_t buzzer)
{
    ledc_set_duty(LEDC_SPEED_MODE, BUZZER_CHANNEL, buzzer);
    ledc_update_duty(LEDC_SPEED_MODE, BUZZER_CHANNEL);
}