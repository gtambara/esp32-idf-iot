/*

  Esse codigo objetiva realizar um teste em laboratório de comunicação entre dois ESPs32 por módulos LORA. Idealmente um sinal de GPS fariam parte dos dados transmitidos, contudo, pela incapacidade de utilizar o módulo GPS no laboratório, foi realizada a comunicação simplificada apenas por uma mensagem de texto.

*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lora.h"

uint8_t buf[64];

/*
  Função responsável pela recepção dos dados no buffer 'buf'
*/
void task_rx(void *p)
{
  int x;
  for (;;)
  {
    lora_receive(); // put into receive mode
    while (lora_received())
    {
      x = lora_receive_packet(buf, sizeof(buf));
      buf[x] = 0;
      printf("Received: %s\n", buf);
      lora_receive();
    }
    vTaskDelay(1);
  }
}

/*
  Função responsável pelo envio da mensagem de texto 'carta'
*/
void task_tx(void *p)
{

  char[] carta = "TesteDoTambs";

  for (;;)
  {
    vTaskDelay(pdMS_TO_TICKS(5000));
    lora_send_packet((uint8_t *)"TesteDoTambs", sizeof(carta));
    printf("packet sent...\n");
    vTaskDelay(1);
  }
}

void app_main()
{
  lora_init();
  lora_set_frequency(915e6);
  lora_enable_crc();
  xTaskCreate(&task_rx, "task_rx", 2048, NULL, 3, NULL);
  xTaskCreate(&task_rx, "task_tx", 2048, NULL, 3, NULL);
}