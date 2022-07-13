/* Programa que funciona de modo que sua saída é de
valores inteiros entre 0 e 9 apenas.
*/
#include <stdio.h>
#include "esp_system.h"
#include "math.h"

void app_main(void)
{
  int i = 0;
  int rnd = 0;
  while (i < 10)
  {
    rnd = esp_random();
    printf("rnd = %d \n", rnd % 10);
    i++;
  }
}