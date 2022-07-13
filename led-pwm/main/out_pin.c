/*
  Exemplo de PWM com uma variável que conta
  quantas vezes a entrada foi acionada, imprimindo
  na tela cada incremento desse contador.
*/

#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*
  Definição de constantes
*/
#define PWMPIN 4
#define READPIN 5
#define ON 1
#define OFF 0
#define REPETITION 500 // quantas vezes o mesmo ciclo pwm é repetido

/*
  Função do ciclo PWM
*/
void ciclo(float dutycycle, float periodo, int *count)
{
  // delay de 1 milissegundo
  int delay = 1;

  gpio_set_level(PWMPIN, ON);
  for (int i = 0; i < dutycycle * 100 * periodo; i++)
  {
    vTaskDelay(delay / portTICK_PERIOD_MS);
  }

  // Obtenção do dado digital = leitura do status do led
  if (gpio_get_level(READPIN) == ON)
  {
    (*count) = (*count) + 1;
    printf("pulsos: %d\n", *count);
  }

  gpio_set_level(PWMPIN, OFF);
  for (int i = dutycycle * 100 * periodo; i < 100 * periodo; i++)
  {
    vTaskDelay(delay / portTICK_PERIOD_MS);
  }
}

void app_main(void)
{

  // Inicialização de variáveis
  float periodo = 1;
  float dutycycle = 1; // varia de 0 a 1
  int count = 0;

  // Definicição de pinout
  gpio_pad_select_gpio(PWMPIN);
  gpio_set_direction(PWMPIN, GPIO_MODE_OUTPUT);
  gpio_pad_select_gpio(READPIN);
  gpio_set_direction(READPIN, GPIO_MODE_INPUT);

  while (true)
  {
    for (dutycycle = 0; dutycycle < 1; dutycycle += 0.2)
    {
      for (int i = 0; i < REPETITION; i++)
      {
        ciclo(dutycycle, periodo, &count);
      }
    }
    for (dutycycle = 1; dutycycle > 0.2; dutycycle -= 0.2)
    {
      for (int i = 0; i < REPETITION; i++)
      {
        ciclo(dutycycle, periodo, &count);
      }
    }
  }
}