/*
  Exemplo de uso do light-sleep, tratando do problema de atraso e congelamento com clock gating.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "esp32/rom/uart.h"

void app_main()
{
  // define o tempo em que o modo sleep ficará ligado após seu inicio
  // o RTC continua ligado no modo light-sleep e o valor é dado em microssegundos, porém é influenciado pelo clock RTC SLOW_CLK
  esp_sleep_enable_timer_wakeup(5000000); // configura o timer

  printf("Iniciando modo sleep... \n"); // mensagem informativa

  vTaskDelay(100 / portTICK_PERIOD_MS); // delay adicionado para correção de erro

  int64_t t_inicio = esp_timer_get_time(); // time stamp 1

  // insere o ESP32 no modo light-sleep
  esp_light_sleep_start(); // iniciando modo sleep

  int64_t t_fim = esp_timer_get_time(); // time stamp 2

  printf("Modo sleep durou %lld milisegundos.\n", (t_fim - t_inicio) / 1000);
}

// o comportamento inesperado no código envolve a mensagem inicial "iniciano modo sleep" aparecendo apenas no retorno do proprio modo sleep do esp32. O que pode estar ocorrendo envolve o código não dar tempo para o esp realizar o envio da mensagem antes de seu processamento e envio para o computador, afinal, durante o modo sleep, a própria ram e os periférios sofrem clock gating (o clock é desligado) e seu conteúdo é preservado para o quando o modo normal retornar. A minha solução envolveu a adição de um delay previo ao início do sleep. Ademais também foi corrigida a subtração na linha final, de modo que antes os valores eram  negativos.