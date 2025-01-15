//Giuseppe Bruno Ferreira Filippin
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "console.h"

// Variáveis globais para simular velocidade e manobras do quadricóptero
volatile long velocidadeMotor0 = 0;
volatile long velocidadeMotor1 = 0;
volatile long velocidadeMotor2 = 0;
volatile long velocidadeMotor3 = 0;

volatile char sentido[10];     // Variável para guinada (horário/anti-horário)
volatile char direcao[10];     // Variável para arfagem (frente/trás)
volatile char orientacao[10];  // Variável para rolagem (direita/esquerda)

// Semáforo binário
SemaphoreHandle_t xSemaphore;

// Funções para as manobras do quadricóptero
void guinada(void* pvParameters);
void arfagem(void* pvParameters);
void rolagem(void* pvParameters);
void radioFrequencia(void* pvParameters);  // Tarefa adicional de rádio frequência
void vPrintMotorSpeeds();
void vPrintString(const char *str);
int randn(int min, int max);

// Função principal
int main_(void) {
    // Criar semáforo binário
    xSemaphore = xSemaphoreCreateBinary();
    if (xSemaphore == NULL) {
        // Erro ao criar o semáforo
        printf("Erro ao criar o semáforo.\n");
        return -1;
    }

    // Inicializando o semáforo (liberar para uso)
    xSemaphoreGive(xSemaphore);

    // Inicializar variáveis globais com valores iniciais
    strcpy(sentido, "horario");
    strcpy(direcao, "frente");
    strcpy(orientacao, "direita");

    // Criando tarefas com prioridade 1
    xTaskCreate(guinada, "Guinada", 4096, NULL, 1, NULL);
    xTaskCreate(arfagem, "Arfagem", 4096, NULL, 1, NULL);
    xTaskCreate(rolagem, "Rolagem", 4096, NULL, 1, NULL);
    
    // Criando a nova tarefa de rádio frequência com prioridade inferior
    xTaskCreate(radioFrequencia, "RadioFreq", 4096, NULL, 0, NULL);
    
    // Iniciando o escalonador
    vTaskStartScheduler();

    // Loop infinito
    for(;;);
}

// Função guinada
void guinada(void* pvParameters) {
    vPrintString("Iniciando manobra de guinada\n");
    int count = 0;

    // Loop para executar a tarefa 10 vezes
    for(;;) {
        if (count >= 10) {
            vPrintString("Encerrando manobra de guinada\n");
            vTaskDelete(NULL);  // Excluir a tarefa após 10 iterações
        }

        // Verifica se conseguiu pegar o semáforo
        if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) {
            // Se o sentido for horário ou anti-horário
            if (strcmp(sentido, "horario") == 0) {
                velocidadeMotor0 += 100;
                velocidadeMotor2 += 100;
                velocidadeMotor1 -= 100;
                velocidadeMotor3 -= 100;
            } else {
                velocidadeMotor0 -= 100;
                velocidadeMotor2 -= 100;
                velocidadeMotor1 += 100;
                velocidadeMotor3 += 100;
            }
            vPrintMotorSpeeds();
            // Libera o semáforo após as operações
            xSemaphoreGive(xSemaphore);  
        }

        vTaskDelay(portTICK_RATE_MS * 10);  // Atraso de 10ms
        count++;  // Incrementar o contador após cada execução
    }
}

// Função arfagem
void arfagem(void* pvParameters) {
    vPrintString("Iniciando manobra de arfagem\n");
    int count = 0;

    for(;;) {
        if (count >= 10) {
            vPrintString("Encerrando manobra de arfagem\n");
            vTaskDelete(NULL);  // Excluir a tarefa após 10 iterações
        }

        // Verifica se conseguiu pegar o semáforo
        if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) {
            // Se a direção for frente ou trás
            if (strcmp(direcao, "frente") == 0) {
                velocidadeMotor2 += 25;
                velocidadeMotor3 += 25;
                velocidadeMotor0 -= 25;
                velocidadeMotor1 -= 25;
            } else {
                velocidadeMotor2 -= 25;
                velocidadeMotor3 -= 25;
                velocidadeMotor0 += 25;
                velocidadeMotor1 += 25;
            }
            vPrintMotorSpeeds();
            // Libera o semáforo após as operações
            xSemaphoreGive(xSemaphore);  
        }

        vTaskDelay(portTICK_RATE_MS * 40);  // Atraso de 40ms
        count++;  // Incrementar o contador após cada execução
    }
}

// Função rolagem
void rolagem(void* pvParameters) {
    vPrintString("Iniciando manobra de rolagem\n");
    int count = 0;

    for(;;) {
        if (count >= 10) {
            vPrintString("Encerrando manobra de rolagem\n");
            vTaskDelete(NULL);  // Excluir a tarefa após 10 iterações
        }

        // Verifica se conseguiu pegar o semáforo
        if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) {
            // Se a orientação for direita ou esquerda
            if (strcmp(orientacao, "direita") == 0) {
                velocidadeMotor0 += 50;
                velocidadeMotor3 += 50;
                velocidadeMotor1 -= 50;
                velocidadeMotor2 -= 50;
            } else {
                velocidadeMotor0 -= 50;
                velocidadeMotor3 -= 50;
                velocidadeMotor1 += 50;
                velocidadeMotor2 += 50;
            }
            vPrintMotorSpeeds();
            // Libera o semáforo após as operações
            xSemaphoreGive(xSemaphore);  
        }

        vTaskDelay(portTICK_RATE_MS * 20);  // Atraso de 20ms
        count++;  // Incrementar o contador após cada execução
    }
}

// Função rádio frequência para alterar aleatoriamente sentido, direção e orientação
void radioFrequencia(void* pvParameters) {
    for(;;) {
        int x = rand() % 100;
        int y = rand() % 100;
        int z = rand() % 100;

        // Verifica se conseguiu pegar o semáforo
        if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) {
            // Atualizar "sentido", "direcao" e "orientacao" com base no sorteio
            if (x % 2 == 0) sprintf(sentido, "horario");
            else sprintf(sentido, "antihorario");

            if (y % 2 == 0) sprintf(direcao, "frente");
            else sprintf(direcao, "tras");

            if (z % 2 == 0) sprintf(orientacao, "direita");
            else sprintf(orientacao, "esquerda");

            printf("Atualizando variáveis: Sentido: %s, Direção: %s, Orientação: %s\n", sentido, direcao, orientacao);
            // Libera o semáforo após as operações
            xSemaphoreGive(xSemaphore);  
        }

        vTaskDelay(portTICK_RATE_MS * 100);  // Atraso de 100ms
    }
}

// Função para exibir as velocidades dos motores
void vPrintMotorSpeeds() {
    printf("Velocidades dos motores:\n");
    printf("Motor 0: %ld\n", velocidadeMotor0);
    printf("Motor 1: %ld\n", velocidadeMotor1);
    printf("Motor 2: %ld\n", velocidadeMotor2);
    printf("Motor 3: %ld\n", velocidadeMotor3);
}

// Função para imprimir strings
void vPrintString(const char *str) {
    printf("%s", str);
}
