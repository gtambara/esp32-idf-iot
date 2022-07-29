/*

*/

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#define PWMPIN 4
#define READPIN 5
#define ON 1
#define OFF 0
#define REPETITION 500    // quantas vezes o mesmo ciclo pwm é repetido
#define DEFAULT_VREF 1100 // Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES 10  // Multisampling

static const adc_channel_t channel1 = ADC_CHANNEL_6; // GPIO 34
static const adc_bits_width_t width = ADC_WIDTH_BIT_11;
static const adc_atten_t atten = ADC_ATTEN_DB_11;
static const adc_unit_t unit = ADC_UNIT_1;
static esp_adc_cal_characteristics_t *adc_chars;

typedef struct structRead
{
  uint32_t leitura[NO_OF_SAMPLES];
  int amostragem;
} xStruct;

/*
  Realiza as medições dos sinais analógicos no canal 1.
*/
void taskAnalogRead(void *pvParameters)
{
  adc1_config_width(255);
  adc1_config_channel_atten(channel1, atten);

  while (true)
  {
    if (((xStruct *)pvParameters)->amostragem >= NO_OF_SAMPLES - 1)
    {
      ;
    }
    else
    {

      if (unit == ADC_UNIT_1)
      {
        (((xStruct *)pvParameters)->leitura)[((xStruct *)pvParameters)->amostragem] = adc1_get_raw((adc1_channel_t)channel1);
      }
      else
      {
        int raw;
        adc2_get_raw((adc2_channel_t)channel1, width, &raw);
        (((xStruct *)pvParameters)->leitura)[((xStruct *)pvParameters)->amostragem] = raw;
      }
      (((xStruct *)pvParameters)->amostragem)++;

      // Converte a medição para milivolts
      uint32_t voltage = esp_adc_cal_raw_to_voltage((((xStruct *)pvParameters)->leitura)[((xStruct *)pvParameters)->amostragem], adc_chars);
      printf("ams: %d\tRaw: %d\tVoltage: %dmV\n", (((xStruct *)pvParameters)->amostragem), (((xStruct *)pvParameters)->leitura)[((xStruct *)pvParameters)->amostragem], voltage);
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
  }
}

/*
  Realiza o cálculo da média móvel de amostras NO_OF_SAMPLES.
*/
void taskMovinAverage(void *pvParameters)
{
  uint32_t sum = 0;

  while (true)
  {
    if ((((xStruct *)pvParameters)->amostragem) >= NO_OF_SAMPLES - 1)
    {
      for (int i = 0; i < NO_OF_SAMPLES; i++)
      {
        sum += (((xStruct *)pvParameters)->leitura)[i];
      }
      sum /= NO_OF_SAMPLES;

      // Converte a medição para milivolts
      uint32_t voltage = esp_adc_cal_raw_to_voltage(sum, adc_chars);
      printf("MeanVoltage: %dmV\n", voltage);
      vTaskDelay(pdMS_TO_TICKS(2000));
    }
    (((xStruct *)pvParameters)->amostragem) = 0;
  }
}

/*
  Função do ciclo PWM
*/
void cycle(float dutycycle, float periodo)
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
  uint32_t leiturax[NO_OF_SAMPLES];
  xStruct xParameter = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0};

  // Atribui características definidas para o conversor analógico pra digital
  adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
  esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars);

  // Cria a task de leitura analógica
  xTaskCreate(
      &taskAnalogRead,     // Ponteiro para a função que implementa a Task
      "DataRead",          // Nome da Task
      2048,                // Stack depth - ver documentação
      (void *)&xParameter, // Parâmetro para a task, neste exemplo sem parâmetros
      1,                   // Prioridade da Task
      NULL                 // Task handle - ver documentação
  );

  xTaskCreate(
      &taskMovinAverage,   // Ponteiro para a função que implementa a Task
      "CalcAverage",       // Nome da Task
      2048,                // Stack depth - ver documentação
      (void *)&xParameter, // Parâmetro para a task, neste exemplo sem parâmetros
      1,                   // Prioridade da Task
      NULL                 // Task handle - ver documentação
  );
}

/*

  Como pode ser visto na execução do código, a task de maior delay, mesmo que com maior prioridade, é repetida menos vezes do que a task inicial, tendo em vista apenas o tempo entre os seus loops. Ou seja, o escalonamento da task possui um tempo pequeno o suficiente para não impactar a proporção de distribuição de tempo de atividade das task.

*/