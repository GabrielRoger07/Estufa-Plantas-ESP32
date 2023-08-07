#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/semphr.h"
#include "wifi.h"
#include "mqtt.h"
#include "buzzer.h"
#include "ledRgbTempUmid.h"
#include "ledRgbIrrigacao.h"
#include "driver/gpio.h"
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "sdkconfig.h"
#include "dht11.h"
#include <unistd.h>
#include "nvs.h"
#include "driver/ledc.h"
#include "esp_err.h"

// Semáforos para controle de conexão Wi-Fi e MQTT
SemaphoreHandle_t conexaoWifiSemaphore;
SemaphoreHandle_t conexaoMQTTSemaphore;

// Variáveis para armazenar o estado da leitura do DHT11 e as mensagens a serem enviadas
bool dht11_Valido = false;
int temperatura = 0;
int umidade = 0;
char mensagem1[50];
char mensagem2[50];
char mensagem3[50];
char mensagem4[50];
char mensagem5[50];

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
  if (xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY))
  {
    while (true)
    {
      // Se houver uma leitura válida do sensor DHT11
      if(dht11_Valido){
        // Formatando as mensagens para serem enviadas e gravando os valores de temperatura e umidade no NVS
        sprintf(mensagem1, "{\"Temperatura\": %d}", temperatura);
        sprintf(mensagem2, "{\"Umidade\": %d}", umidade);
        grava_valor_nvs("Temperatura", temperatura);
        grava_valor_nvs("Umidade", umidade);
        if(temperatura >= 22 && temperatura <= 35 && umidade <= 50 && umidade >= 35){
            // Setando o LED de temperatura e umidade como verde e o BUZZER como desligado
            setLEDTempUmidIntensity(256, 0, 256);
            setBuzzerIntensity(0);
            // Formatando as mensagens para serem enviadas e gravando os valores do LED verde, LED vermelho e BUZZER no NVS
            sprintf(mensagem3, "{\"Led Temp e Umidade\": %s}", "Verde");
            sprintf(mensagem4, "{\"Buzzer\": %d}", 0);
            grava_valor_nvs("LedTempUmidVd", 1);
            grava_valor_nvs("LedTempUmidVm", 0);
            grava_valor_nvs("Buzzer", 0);
        }else{
            // Setando o LED de temperatura e umidade como vermelho e o BUZZER como ligado
            setLEDTempUmidIntensity(0, 256, 256);
            setBuzzerIntensity(256);
            // Formatando as mensagens para serem enviadas e gravando os valores do LED verde, LED vermelho e BUZZER no NVS
            sprintf(mensagem3, "{\"Led Temp e Umidade\": %s}", "Vermelho");
            sprintf(mensagem4, "{\"Buzzer\": %d}", 1);
            // Gravando os valores do LED verde, LED vermelho e BUZZER no NVS
            grava_valor_nvs("LedTempUmidVd", 0);
            grava_valor_nvs("LedTempUmidVm", 1);
            grava_valor_nvs("Buzzer", 1);
        }
        // Enviando os valores de temperatura, umidade, LED e BUZZER via MQTT
        mqtt_envia_mensagem("v1/devices/me/telemetry", mensagem1);
        mqtt_envia_mensagem("v1/devices/me/telemetry", mensagem2);
        mqtt_envia_mensagem("v1/devices/me/attributes", mensagem3);
        mqtt_envia_mensagem("v1/devices/me/attributes", mensagem4);
        //le_valor_nvs("Umidade");
    }
        // Aguardando 1 segundo para repetir o processo
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
  }
}

void readDHT11(void *params)
{
	while (true) {
		struct dht11_reading dht11_data = DHT11_read();
		if (dht11_data.status == DHT11_OK) {
      // Caso de leitura bem sucedida
			temperatura = dht11_data.temperature;
			umidade = dht11_data.humidity;
			dht11_Valido = true;
			ESP_LOGI("DHT11", "Leitura realizada com sucesso");
		} else {
			switch (dht11_data.status) {
        // Erro de leitura por conta de CRC mismatch
			case DHT11_CRC_ERROR:
				ESP_LOGE("DHT11", "Falha de leitura com status %d (CRC MISMATCH)", dht11_data.status);
				break;
        // Erro de leitura por conta de timeout
			case DHT11_TIMEOUT_ERROR:
				ESP_LOGE("DHT11", "Falha de leitura com status %d (TIMEOUT)", dht11_data.status);
				break;
        // Erro de leitura genérico
			default:
				ESP_LOGE("DHT11", "Falha de leitura com status %d", dht11_data.status);
			}
		}
		if (dht11_Valido) {
			ESP_LOGI("DHT11", "Temperatura atual: %d", temperatura);
			ESP_LOGI("DHT11", "Umidade atual: %d", umidade);
		}
    // Aguardando 1 segundo para repetir o processo
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

void app_main(){
    // Inicializando sensores (LEDs, BUZZER e DHT11)
    setupLEDTempUmid();
    setupBuzzer();
    setupLEDIrrigacao();
    grava_valor_nvs("LedIrrigacao", 0);
    sprintf(mensagem5, "{\"Led Irrigacao\": %d}", 0);
    mqtt_envia_mensagem("v1/devices/me/attributes", mensagem5);
    DHT11_init(GPIO_NUM_23);

    // Inicializa o NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    conexaoWifiSemaphore = xSemaphoreCreateBinary();
    conexaoMQTTSemaphore = xSemaphoreCreateBinary();
    
    wifi_start();

    xTaskCreate(&conectadoWifi,  "Conexão ao MQTT", 4096, NULL, 1, NULL);
    xTaskCreate(&trataComunicacaoComServidor, "Comunicação com Broker", 4096, NULL, 1, NULL);
    xTaskCreate(&readDHT11, "Lendo DHT11", 4096, NULL, 3, NULL);
}