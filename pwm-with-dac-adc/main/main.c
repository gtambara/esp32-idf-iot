/*
    Exemplo de utilizacao do hardware DAC.
    Os pinos 25 e 26 podem ser utilizados para essa funcao.
    Teste diferentes valores e modifique o exemplo para alterar
    o valor de intensidade de um LED automaticamente 0-255-0.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include <driver/dac.h>

/*
  Função de delay
*/
void delays(int delay)
{
  for (int i = 0; i < delay; i++)
  {
    vTaskDelay(delay / portTICK_PERIOD_MS);
  }
}

void app_main()
{
  dac_output_enable(DAC_CHANNEL_1);
  while (true)
  {
    dac_output_voltage(DAC_CHANNEL_1, 75);
    printf("tesr1\n");
    delays(100);
    dac_output_voltage(DAC_CHANNEL_1, 150);
    printf("tesr2\n");
    delays(100);
    dac_output_voltage(DAC_CHANNEL_1, 255);
    printf("tesr3\n");
    delays(100);
  }
}