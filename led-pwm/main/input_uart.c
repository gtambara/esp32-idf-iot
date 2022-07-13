/*
  O programa, em conjunto com
  códigos anteriores, permite o controle da intensidade
  luminosa do LED a partir de comandos UART.
*/

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

/*
  Definição de constantes
*/
#define PWMPIN 4
#define ON 1
#define OFF 0
#define REPETITION 1000 // quantas vezes o mesmo ciclo pwm é repetido

/*
  Função do ciclo PWM
*/
void ciclo(float dutycycle, float periodo)
{
  // delay de 1 milissegundo
  int delay = 1;

  gpio_set_level(PWMPIN, ON);
  for (int i = 0; i < dutycycle * 100 * periodo; i++)
  {
    vTaskDelay(delay / portTICK_PERIOD_MS);
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

  // Definicição de pinout
  gpio_pad_select_gpio(PWMPIN);
  gpio_set_direction(PWMPIN, GPIO_MODE_OUTPUT);

  // Setup comunicacao UART
  while (true)
  {
    char c = 0;
    char str[100];
    memset(str, 0, sizeof(str));
    printf("Digite um número de 0 a 9: \n");
    while (c != '\n')
    {
      c = getchar();
      if (c != 0xff)
      {
        str[strlen(str)] = c;
        printf("%c", c);
      }
      vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    if (str[0] == '1')
    {
      dutycycle = 0.1;
    }
    else if (str[0] == '2')
    {
      dutycycle = 0.2;
    }
    else if (str[0] == '3')
    {
      dutycycle = 0.3;
    }
    else if (str[0] == '4')
    {
      dutycycle = 0.4;
    }
    else if (str[0] == '5')
    {
      dutycycle = 0.5;
    }
    else if (str[0] == '6')
    {
      dutycycle = 0.6;
    }
    else if (str[0] == '7')
    {
      dutycycle = 0.7;
    }
    else if (str[0] == '8')
    {
      dutycycle = 0.8;
    }
    else if (str[0] == '9')
    {
      dutycycle = 0.9;
    }
    else
    {
      dutycycle = 1;
    }

    // Execução do PWM sob as condições definidas previamente
    for (int i = 0; i < REPETITION; i++)
    {
      ciclo(dutycycle, periodo);
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}