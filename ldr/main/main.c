/*
  Este programa possu a capacidade de realizar a leitura de um sensor, como um  LDR ou um potenciômetro, e avalia a intensidade do  sinal de PWM a ser transferido para um LED. São utilizados conversores analógico digitais e uma função simples de PWM.s

*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include <driver/dac.h>

int RAW_GLOBAL = 0;

/*
  Definição de constantes
*/
#define PWMPIN 4
#define ON 1
#define OFF 0
#define REPETITION 500 // quantas vezes o mesmo ciclo pwm é repetido

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

void taskRead(void *params)
{
  // Setup de pinos do conversor AD
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);

  while (true)
  {
    RAW_GLOBAL = adc1_get_raw(ADC1_CHANNEL_6);
    printf("ADC = %d\n", RAW_GLOBAL);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void taskLED(void *params)
{
  // Setup de pinos do conversor DA
  dac_output_enable(DAC_CHANNEL_1); // Ch 1 = GPIO 25

  int periodo = 1;

  // Definicição de pinout
  gpio_pad_select_gpio(PWMPIN);
  gpio_set_direction(PWMPIN, GPIO_MODE_OUTPUT);

  while (true)
  {
    float signal = ((float)RAW_GLOBAL / 4095); // Normalização do sinal para porcentagem
    for (int i = 0; i < REPETITION; i++)
    {
      ciclo(signal, periodo);
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void app_main()
{

  xTaskCreate(&taskLED, "readLEDControl", 2048, NULL, 1, NULL);
  xTaskCreate(&taskRead, "readDataTask", 2048, NULL, 2, NULL);
}