//Ciências da Computação
//Giuseppe Bruno Ferreira Filippin
//Felipe Augusto Baleche 
#include "contiki.h" //biblioteca conitki
#include <stdio.h> //biblioteca padrao
#include <stdlib.h> // biblioteca para numeros aleatorios

//processos para cada dado 
PROCESS(coracao, "Monitoramento de Batimentos Cardíacos");
PROCESS(oxygen_saturation_process, "Monitoramento de Saturação de Oxigênio");
PROCESS(temperature_process, "Monitoramento de Temperatura");
PROCESS(alert_process, "Processo de Alerta");
//comecar automaticamnete no comeco 
AUTOSTART_PROCESSES(&coracao, &oxygen_saturation_process, &temperature_process, &alert_process);

//variavel para armazenar os dados 
static int heart_rate = 0;
static int oxygen_saturation = 0;
static int temperature = 0;

// Função de thread para monitorar batimentos cardíacos
PROCESS_THREAD(coracao, ev, data) {
  static struct etimer et;
  PROCESS_BEGIN();
  while (1) {
    heart_rate = (rand() % 121) + 20; // Valores entre 20 e 140
    printf("Batimentos cardíacos: %d bpm\n", heart_rate);
    if (heart_rate < 50) {
      process_post(&alert_process, PROCESS_EVENT_MSG, "Batimento cardíaco baixo!");
    } else if (heart_rate > 90) {
      process_post(&alert_process, PROCESS_EVENT_MSG, "Batimento cardíaco alto!");
    }
    etimer_set(&et, CLOCK_SECOND * 3); // Intervalo de 3 segundos
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  }
  PROCESS_END();
}

// Função de thread para monitorar saturação de oxigênio
PROCESS_THREAD(oxygen_saturation_process, ev, data) {
  static struct etimer et;
  PROCESS_BEGIN();
  while (1) {
    oxygen_saturation = (rand() % 21) + 80; // Valores entre 80% e 100%
    printf("Saturação de oxigênio: %d%%\n", oxygen_saturation);
    if (oxygen_saturation < 90) {
      process_post(&alert_process, PROCESS_EVENT_MSG, "Saturação baixa!");
    }
    etimer_set(&et, CLOCK_SECOND * 3); // Intervalo de 3 segundos
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  }
  PROCESS_END();
}

// Função de thread para monitorar temperatura
PROCESS_THREAD(temperature_process, ev, data) {
  static struct etimer et;
  PROCESS_BEGIN();
  while (1) {
    temperature = (rand() % 8) + 34; // Valores entre 34°C e 41°C
    printf("Temperatura: %d°C\n", temperature);
    if (temperature < 35) {
      process_post(&alert_process, PROCESS_EVENT_MSG, "Hipotermia detectada!");
    } else if (temperature > 37) {
      process_post(&alert_process, PROCESS_EVENT_MSG, "Febre detectada!");
    }
    etimer_set(&et, CLOCK_SECOND * 3); // Intervalo de 3 segundos
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  }
  PROCESS_END();
}

// Processo de alerta
PROCESS_THREAD(alert_process, ev, data) {
  PROCESS_BEGIN();
  while (1) {
    PROCESS_WAIT_EVENT();
    if (ev == PROCESS_EVENT_MSG) {
      printf("ALERTA: %s\n", (char *)data); // Imprime a mensagem de alerta
    }
  }
  PROCESS_END();
}
