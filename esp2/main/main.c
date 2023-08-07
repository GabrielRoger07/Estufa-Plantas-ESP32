// Includes
#include <stdio.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/semphr.h"
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include <unistd.h>
#include "nvs.h"
#include "esp_err.h"
#include "ledRgb.h"

#include "esp_sleep.h"
#include "driver/rtc_io.h"

#include "esp_system.h"


#include "wifi.h"
#include "mqtt.h"

// variável global para armazenar a quantidade de vezes que a esp foi acordada
RTC_DATA_ATTR int Acordou = 0;

//declaração das variaveis globais para o encoder que controla o led
volatile int encoder_value = 0;
volatile bool button_pressed = false;
volatile bool air_conditioner_on = false;
RTC_DATA_ATTR int temperature = 100; // inicialmente o led vai começar com 100%

// declaração das variáveeis globais para o encoder do controle de ventilação
volatile int encoderValue = 0;
volatile bool buttonPressed = false;
volatile bool airFlux_on = false;
RTC_DATA_ATTR int valorVento = 35; // incialmente o sistema de ventilação vai começar em 35%

// criando semáforos
SemaphoreHandle_t conexaoWifiSemaphore;
SemaphoreHandle_t conexaoMQTTSemaphore;

// funções do encoder que controla a intensidade do led
static void IRAM_ATTR encoder_isr_handler(void *arg) 
{
  if (air_conditioner_on) // se o sistema está ligado
  {
    static int8_t lookup_table[] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};
    static uint8_t encoder_state = 0;
    encoder_state = ((encoder_state << 2) + (gpio_get_level(PIN_DT) << 1) + gpio_get_level(PIN_CLK)) & 0x0f;
    //trata as condições para a correta implementação da porcentagem de funcionamento do led
    if (temperature <= 100 && temperature >= 0) // se a porcentagem de funcionamento do led está entre 0 e 100% 
    {
      if (temperature <= 2 && lookup_table[encoder_state] < 0)
      {
        temperature = 0;
      }
      else if (temperature >= 99 && lookup_table[encoder_state] > 0)
      {
        temperature = 100; // Incrementa ou decrementa a temperatura
      }
      else
      {
        temperature += lookup_table[encoder_state];
      }
    }
  }
}

static void IRAM_ATTR button_isr_handler(void *arg)
{
  button_pressed = true; // botao pressionado
}

void encoder_task(void *arg)
{
  int last_temperature = temperature; // porcentagem atual do led é guardada numa variável auxiliar
  while (1)
  {
    if (last_temperature != temperature) // se for diferente, o que quer dizer que girou o encoder
    {
      printf("Intensidade da luz: %d%%\n", temperature);
      last_temperature = temperature;  // atualiza a variável auxiliar
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}


void button_task(void *arg)
{
  while (1)
  {
    if (button_pressed) // se o botão for pressionado
    {
      air_conditioner_on = !air_conditioner_on; // estado do botão for not false, ou seja, ligado
      if (air_conditioner_on)
      {
        printf("Controle Ligado\n"); // liga o ar condicionado
      }
      else
      {
        printf("Controle desligado\n"); // se não desliga
      }
      button_pressed = false; // atualiza o estado do botão
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

// funções para o encoder que controla o fluxo de ar

/*
nas seguintes funções a implementação do segundo encoder segue a mesma
lógica do primeiro acima.
*/

static void IRAM_ATTR encoder_isr_handler_fluxoAr(void* arg) {
    if(airFlux_on) {
        static int8_t lookup_table[] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};
        static uint8_t encoder_state = 0;
        encoder_state = ((encoder_state << 2) + (gpio_get_level(pinoDt) << 1) + gpio_get_level(pinoClk)) & 0x0f;
        if(valorVento <= 100 && valorVento >= 0 ){
            if((valorVento == 0 && lookup_table[encoder_state]<0) || (valorVento == 100 && lookup_table[encoder_state]>0)){
            
            }else{
                valorVento += lookup_table[encoder_state]; // Incrementa ou decrementa a temperatura    

            }
        }
    }
}

static void IRAM_ATTR button_isr_handler_fluxoAr(void* arg) {
    buttonPressed = true;
}

void encoder_task_fluxoAr(void* arg) {
    int last_temperature = valorVento;
    while(1) {
        if (last_temperature != valorVento) {
            printf("Intensidade do Fluxo de Ar: %d%%\n", valorVento);
            last_temperature = valorVento;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void button_task_fluxoAr(void* arg) {
    while(1) {
        if (buttonPressed) {
            airFlux_on = !airFlux_on;
            if(airFlux_on){
                printf("Ventilação Ligada\n");
            } else {
                printf("Ventilação Desligada\n");
            }
            buttonPressed = false;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
/*fim do bloco de funções que implementam o funcionamento do segundo encoder, usado para
o controle do nível do fluxo de ar*/

// Tarefa para lidar com a conexão ao Wi-Fi
void conectadoWifi(void *params)
{
  while (true)
  {
    if (xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY))
    {
      // Processamento Internet
      mqtt_start();
    }
  }
}

// Tarefa para lidar com a comunicação com o servidor
void trataComunicacaoComServidor(void *params)
{
  char mensagem3[50];
  char mensagem2[50];
  char mensagem4[50];
  //char JsonAtributos[200];
  int valor;
  if (xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY))
  {
    while (true)
    {
      // envia via mqtt para o servidor central o nivel de intensidade da luz do led
      sprintf(mensagem3, "{\"Intensidade da Luz\": %d}", temperature);
      mqtt_envia_mensagem("v1/devices/me/attributes", mensagem3);
      valor = ((100-temperature)*256)/100;
      setLEDIntensity(256, valor, 256);
      grava_valor_nvs("IntensidadeLuz", temperature); // grava na memomria flash da esp o valor da intensidade
      //le_valor_nvs("IntensidadeLuz"); -- usado para printar no terminal o valor armazenado
      
      // envia mensagem para o servidor central a quantidade de vezes que a esp foi acordada
      sprintf(mensagem2, "{\"Quantidade de Wakeup\":  %d}", Acordou);
      mqtt_envia_mensagem("v1/devices/me/attributes", mensagem2);
      grava_valor_nvs("WakesUp", Acordou); // grava na memoria flash da esp o número de vezes que a esp acordou
      //le_valor_nvs("WakesUp"); -- usado para printar no terminal o valor armazenado
      
      sprintf(mensagem4, "{\"Intensidade Ventilação\": %d}", valorVento);
      mqtt_envia_mensagem("v1/devices/me/attributes", mensagem4);
      grava_valor_nvs("valorVento", valorVento); // grava na memoria flash da esp o nível de funcionamento do sistema de ventilação
      //le_valor_nvs("valorVento"); -- usado para printar no terminal o valor armazenado

      vTaskDelay(500 / portTICK_PERIOD_MS); // task a cada meio segundo
    }
  }
}

void app_main()
{
  setupLED(); // chama a função para settar o led
  gpio_install_isr_service(0);
// definições das caracteristicas dos pinos e chamadas das funções do encoder para o controle do led
  gpio_set_direction(PIN_DT, GPIO_MODE_INPUT);
  gpio_set_pull_mode(PIN_DT, GPIO_PULLUP_ONLY);
  gpio_set_intr_type(PIN_DT, GPIO_INTR_ANYEDGE);
  gpio_isr_handler_add(PIN_DT, encoder_isr_handler, NULL);

  gpio_set_direction(PIN_CLK, GPIO_MODE_INPUT);
  gpio_set_pull_mode(PIN_CLK, GPIO_PULLUP_ONLY);
  gpio_set_intr_type(PIN_CLK, GPIO_INTR_ANYEDGE);
  gpio_isr_handler_add(PIN_CLK, encoder_isr_handler, NULL);

  gpio_set_direction(PIN_SW, GPIO_MODE_INPUT);
  gpio_set_pull_mode(PIN_SW, GPIO_PULLUP_ONLY);
  gpio_set_intr_type(PIN_SW, GPIO_INTR_POSEDGE);
  gpio_isr_handler_add(PIN_SW, button_isr_handler, NULL);

// definições das características dos pinos e chamadas das funções do encoder para o controle do led
  gpio_set_direction(pinoDt, GPIO_MODE_INPUT);
  gpio_set_pull_mode(pinoDt, GPIO_PULLUP_ONLY);
  gpio_set_intr_type(pinoDt, GPIO_INTR_ANYEDGE);
  gpio_isr_handler_add(pinoDt, encoder_isr_handler_fluxoAr, NULL);

  gpio_set_direction(pinoClk, GPIO_MODE_INPUT);
  gpio_set_pull_mode(pinoClk, GPIO_PULLUP_ONLY);
  gpio_set_intr_type(pinoClk, GPIO_INTR_ANYEDGE);
  gpio_isr_handler_add(pinoClk, encoder_isr_handler_fluxoAr, NULL);

  gpio_set_direction(pinoSw, GPIO_MODE_INPUT);
  gpio_set_pull_mode(pinoSw, GPIO_PULLUP_ONLY);
  gpio_set_intr_type(pinoSw, GPIO_INTR_POSEDGE);
  gpio_isr_handler_add(pinoSw, button_isr_handler_fluxoAr, NULL); 


  // tasks para o funcionamento do primeiro encoder (intensidade do led)
  xTaskCreate(encoder_task, "encoder_task", 2048, NULL, 10, NULL);
  xTaskCreate(button_task, "button_task", 2048, NULL, 10, NULL);

  // tasks para o funcionamento do segundo encoder (nível do fluxo de ar)
  xTaskCreate(encoder_task_fluxoAr, "encoder_task_fluxoAr", 2048, NULL, 5, NULL);
  xTaskCreate(button_task_fluxoAr, "button_task_fluxoAr", 2048, NULL, 5, NULL);

  esp_err_t ret = nvs_flash_init(); // inicia o nvs e guarda em ret

  // evita um laço infinito ao conectar
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  conexaoWifiSemaphore = xSemaphoreCreateBinary();
  conexaoMQTTSemaphore = xSemaphoreCreateBinary();

  // inicia wifi
  wifi_start();

  /*
  implementação do deep sleep:
    seta o botão da gpio para dar o wakeup na placa (tirar do deep sleep) 
  */
  rtc_gpio_pullup_en(BOTAO);
  rtc_gpio_pulldown_dis(BOTAO);
  esp_sleep_enable_ext0_wakeup(BOTAO, 0);
  printf("Acordou %d vezes \n", Acordou++);

  xTaskCreate(&conectadoWifi, "Conexão ao MQTT", 4096, NULL, 1, NULL);
  xTaskCreate(&trataComunicacaoComServidor, "Comunicação com Broker", 4096, NULL, 1, NULL);
  //if (gpio_get_level(BOTAO) == 0)
  
  vTaskDelay(60000 / portTICK_PERIOD_MS);
  printf("entrando em modo Deep Sleep\n");
  esp_deep_sleep_start(); // inicia o deep sleep.
  
}
