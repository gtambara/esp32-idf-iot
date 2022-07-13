/*
  O código realiza o piscar de um led
  externo a cada 1 segundo.
*/

#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*
  Definição de constantes
*/
#define PIN 4
#define ON 1
#define OFF 0

void app_main(void)
{
  /*
   Setup de variáveis
  */
  gpio_pad_select_gpio(PIN);
  gpio_set_direction(PIN, GPIO_MODE_OUTPUT);

  while (true)
  {
    printf("a\n");
    gpio_set_level(PIN, ON);
    // o argumento da função vTaskDelay em ticks contudo ao dividir pela constante, pode-se apenas inserir o valor em milisegundos
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    gpio_set_level(PIN, OFF);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}