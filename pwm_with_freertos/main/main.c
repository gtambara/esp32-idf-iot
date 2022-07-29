/*
  Esse código busca trabalhar com três tasks, sendo uma responsável pela coleta de um valor analógico (um potenciômetro por exemplo), outra responsável por absorver as 10 últimas medidas e calcular a média entre eles, e a terceira é responsável por criar um sinal analógico de saída proporcional ao valor de entrada. Nos instantes em que as tasks são executadas, há uma mensagem na saída padrão (terminal), assim como uma contagem do tempo de execução.
*/

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include <driver/dac.h>
#include "esp_adc_cal.h"
#include <time.h>
#include <sys/time.h>

#define DEFAULT_VREF 1100 // Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES 10  // Multisampling

int RAW_GLOBAL = 0;

static const dac_channel_t dac_canal = DAC_CHANNEL_1; // GPIO 25 (saída analógica)
static const adc_channel_t channel1 = ADC_CHANNEL_6;  // GPIO 34 (entrada analógica)
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
  // Setup de pinos do conversor AD
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);

  while (true)
  {
    int64_t t_inicio = esp_timer_get_time(); // time stamp 1
    if (((xStruct *)pvParameters)->amostragem <= NO_OF_SAMPLES - 1)
    {
      int temp = 0;
      temp = adc1_get_raw(channel1);
      if (temp <= 4095 && temp >= 0)
      {
        (((xStruct *)pvParameters)->leitura)[((xStruct *)pvParameters)->amostragem] = temp;
        (((xStruct *)pvParameters)->amostragem)++;

        // Converte a medição para milivolts
        printf("----------\nams: %d\tRaw: %d\n----------\n", (((xStruct *)pvParameters)->amostragem), (((xStruct *)pvParameters)->leitura)[((xStruct *)pvParameters)->amostragem - 1]);
      }
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    int64_t t_fim = esp_timer_get_time(); // time stamp 2
    printf("TaskREAD > %lld milisegundos.\n", (t_fim - t_inicio) / 1000);
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
    int64_t t_inicio = esp_timer_get_time(); // time stamp 1
    if ((((xStruct *)pvParameters)->amostragem) >= NO_OF_SAMPLES - 1)
    {
      for (int i = 0; i < NO_OF_SAMPLES - 1; i++)
      {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        sum += (((xStruct *)pvParameters)->leitura)[i];
      }
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      sum /= NO_OF_SAMPLES;
      RAW_GLOBAL = sum;
      sum = 0;

      // Converte a medição para milivolts
      uint32_t voltage = esp_adc_cal_raw_to_voltage(sum, adc_chars);
      printf("MeanVoltage: %dmV\n", voltage);
      for (int i = 0; i < NO_OF_SAMPLES - 1; i++)
      {
        (((xStruct *)pvParameters)->leitura[i]) = 0;
      }
      (((xStruct *)pvParameters)->amostragem) = 0;
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    int64_t t_fim = esp_timer_get_time(); // time stamp 2
    printf("TaskMovingAVRG > %lld milisegundos.\n", (t_fim - t_inicio) / 1000);
  }
}

void taskDAC(void *params)
{
  // Setup de pinos do conversor DA
  dac_output_enable(dac_canal); // Ch 1 = GPIO 25

  while (true)
  {
    int64_t t_inicio = esp_timer_get_time();         // time stamp 1
    float signal = ((float)RAW_GLOBAL / 4095 * 255); // Normalização do sinal para porcentagem
    printf("\n ----- \n >>>> signal value: %.2f Volt(s)\n ----- \n", ((float)RAW_GLOBAL / 4095 * 3.3));
    dac_output_voltage(dac_canal, (uint8_t)signal);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    int64_t t_fim = esp_timer_get_time(); // time stamp 2
    printf("TaskDAC > %lld milisegundos.\n", (t_fim - t_inicio) / 1000);
  }
}

void app_main(void)
{
  xStruct xParameter = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0};

  // Atribui características definidas para o conversor analógico pra digital
  adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
  esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars);

  // Cria a task de leitura analógica
  xTaskCreate(
      &taskAnalogRead,     // Ponteiro para a função que implementa a Task
      "DataRead",          // Nome da Task
      4096,                // Stack depth - ver documentação
      (void *)&xParameter, // Parâmetro para a task, neste exemplo sem parâmetros
      3,                   // Prioridade da Task
      NULL                 // Task handle - ver documentação
  );

  xTaskCreate(
      &taskMovinAverage,   // Ponteiro para a função que implementa a Task
      "CalcAverage",       // Nome da Task
      4096,                // Stack depth - ver documentação
      (void *)&xParameter, // Parâmetro para a task, neste exemplo sem parâmetros
      1,                   // Prioridade da Task
      NULL                 // Task handle - ver documentação
  );

  xTaskCreate(&taskDAC, "dacControl", 8012, NULL, 3, NULL);
}
