/*
  O programa permite o controle da intensidade
  luminosa do LED a partir do gerador de PWM presente no
  hardware da placa com freeRTOS.
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/ledc.h"
#include "esp_err.h"

#define PWMPIN 4                 // Pino cujo sinal controla o LED
#define MAX_PWM 1023             // Resolução do PWM
#define LEDC_TEST_FADE_TIME 3000 // tempo em milisegundos

void app_main(void)
{
  int delay_time = LEDC_TEST_FADE_TIME;

  // Struct de dados relacionados ao timer que alteram frequência e dutcycle do PWM
  ledc_timer_config_t ledc_timer = {
      .duty_resolution = LEDC_TIMER_10_BIT,
      .freq_hz = 5000,
      .speed_mode = LEDC_HIGH_SPEED_MODE,
      .timer_num = LEDC_TIMER_0,
      .clk_cfg = LEDC_AUTO_CLK,
  };

  // Define as configurações do timer
  ledc_timer_config(&ledc_timer);

  // Struct que relaciona o timer com o canal de GPIO para gerar o PWM
  ledc_channel_config_t ledc_channel = {
      .channel = LEDC_CHANNEL_0,
      .duty = 0,
      .gpio_num = PWMPIN,
      .speed_mode = LEDC_HIGH_SPEED_MODE,
      .hpoint = 0,
      .timer_sel = LEDC_TIMER_0,
      .flags.output_invert = 0};

  // Define as configurações do canal
  ledc_channel_config(&ledc_channel);

  // Permite o uso de fading para transicionar entre dutycycles
  ledc_fade_func_install(0);

  while (true)
  {
    ledc_set_fade_with_time(ledc_channel.speed_mode,
                            ledc_channel.channel, MAX_PWM, LEDC_TEST_FADE_TIME);
    ledc_fade_start(ledc_channel.speed_mode,
                    ledc_channel.channel, LEDC_FADE_NO_WAIT);

    vTaskDelay(delay_time / portTICK_PERIOD_MS);

    ledc_set_fade_with_time(ledc_channel.speed_mode,
                            ledc_channel.channel, 0, LEDC_TEST_FADE_TIME);
    ledc_fade_start(ledc_channel.speed_mode,
                    ledc_channel.channel, LEDC_FADE_NO_WAIT);

    vTaskDelay(delay_time / portTICK_PERIOD_MS);
  }
}