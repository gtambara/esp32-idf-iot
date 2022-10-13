/*
  O código realiza a publicacao de uma mensagem em um servidor mqtt recebida via LORA
*/

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "connect.h"
#include "lora.h"

// Pinos do Lora presentes no arquivo lora.h

uint8_t buf[32];

static const char *TAG = "MQTT_EXAMPLE";

//Recepção de dados via Lora, semelhante aos exemplos já feitos

void task_rx(void *p)
{
   int x;
   for(;;) {
    // Colocando no modo receptor o dispositivo lora
      lora_receive();
      while(lora_received()) {
         x = lora_receive_packet(buf, sizeof(buf));
         buf[x] = 0;
         printf("Recebido: %s\n", buf);
         lora_receive();
      }
      vTaskDelay(10 / portTICK_PERIOD_MS);
   }
}

// Armazena mensagem de erro

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "´último erro %s: 0x%x", message, error_code);
    }
}

// Tratador de eventos do MQTT

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
		
		// publica no cliente mqtt com o topico especificado quando conectado
        msg_id = esp_mqtt_client_publish(client, "/topic/qos1", "data_3", 0, 1, 0);

        msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);

        msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);

        msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
        break;
    case MQTT_EVENT_DISCONNECTED:
        break;

    case MQTT_EVENT_SUBSCRIBED:
		// publica a mensagem recebida, outrora era "tambs"
        msg_id = esp_mqtt_client_publish(client, "/topic/qos0", buf, 0, 0, 0);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        break;
    case MQTT_EVENT_PUBLISHED:
        break;
    case MQTT_EVENT_DATA:
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;
    case MQTT_EVENT_ERROR:
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
        }
        break;
    default:
    
        break;
    }
}

// início de servidor MQTT

static void mqtt_app_start(void){
  esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri =  "mqtt://test.mosquitto.org:1883",
    };
     esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
     esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
     esp_mqtt_client_start(client);
}

// rotina principal

void app_main(void)
{
  lora_init();
  lora_set_frequency(915e6);
  lora_enable_crc();
  xTaskCreate(&task_rx, "task_rx", 2048, NULL, 5, NULL);
  ESP_ERROR_CHECK(nvs_flash_init());
  wifi_init();
  ESP_ERROR_CHECK(wifi_connect_sta("rede", "senha", 10000));
  mqtt_app_start();
}
