#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "functions.h"

extern char forest[30][30];
extern pthread_mutex_t forest_mutex;
MessageQueue control_queue = { .front = 0, .rear = 0, .count = 0 }; // Inicializa a fila de mensagens

// Função para inicializar a floresta
void initialize_forest(char forest[30][30]) {
    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 30; j++) {
            // Colocamos sensores em 70% das células e áreas livres em 30%
            int random_value = rand() % 10;
            if (random_value < 7) {
                forest[i][j] = 'T'; // Sensor ativo
            } else {
                forest[i][j] = '-'; // Área livre
            }
        }
    }
}

// Função da thread dos sensores
void* sensor_thread_function(void* arg) {
    SensorNode* node = (SensorNode*) arg;

    while (1) {
        pthread_mutex_lock(&node->lock);
        // Se o sensor estiver em chamas, detecta o incêndio e combate
        if (forest[node->x][node->y] == '@') {
            printf("Sensor em (%d, %d) detectou incêndio! Notificando...\n", node->x, node->y);
            
            // Notifica a central de controle (se for borda) ou combate diretamente
            if (node->x == 0 || node->x == 29 || node->y == 0 || node->y == 29) {
                enqueue_message(node->x, node->y, &control_queue);  // Notifica a central
            } else {
                // Sensor tenta combater o fogo
                fight_fire(node->x, node->y, forest);
            }
        }
        pthread_mutex_unlock(&node->lock);
        sleep(2); // Simula o tempo entre verificações
    }
    return NULL;
}

// Função de combate ao incêndio
void fight_fire(int x, int y, char forest[30][30]) {
    pthread_mutex_lock(&forest_mutex);
    if (forest[x][y] == '@') {
        printf("Combate ao fogo em (%d, %d), célula queimada!\n", x, y);
        forest[x][y] = '/'; // Fogo extinto, célula queimada
    }
    pthread_mutex_unlock(&forest_mutex);
}

// Função da thread geradora de incêndios
void* fire_generator(void* arg) {
    while (1) {
        int x = rand() % 30;
        int y = rand() % 30;

        pthread_mutex_lock(&forest_mutex);
        if (forest[x][y] == 'T') {
            forest[x][y] = '@'; // Inicia incêndio na célula
            printf("\n*** Incêndio iniciado em (%d, %d) ***\n", x, y);
        }
        pthread_mutex_unlock(&forest_mutex);

        sleep(5); // Gera incêndios a cada 5 segundos
    }
    return NULL;
}

// Função da central de controle
void* control_center(void* arg) {
    int x, y;

    while (1) {
        if (dequeue_message(&x, &y, &control_queue) == 1) { // Se houver uma mensagem na fila
            printf("Central de controle combatendo fogo em (%d, %d)\n", x, y);
            fight_fire(x, y, forest); // Ação de combate ao incêndio
        }
        sleep(1); // Espera 1 segundo entre verificações da fila
    }
    return NULL;
}

// Função para imprimir a floresta em intervalos de tempo
void print_forest(char forest[30][30]) {
    printf("\nEstado Atual da Floresta:\n");
    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 30; j++) {
            printf("%c ", forest[i][j]);
        }
        printf("\n");
    }
}

// Função para atualizar a visualização a cada intervalo
void* forest_visualizer(void* arg) {
    while (1) {
        print_forest(forest);  // Atualiza a matriz a cada 5 segundos
        sleep(5);  // Intervalo de tempo para atualização da visualização
    }
    return NULL;
}

// Função para adicionar mensagens à fila
void enqueue_message(int x, int y, MessageQueue* queue) {
    pthread_mutex_lock(&forest_mutex);
    if (queue->count < QUEUE_CAPACITY) {
        queue->x[queue->rear] = x;
        queue->y[queue->rear] = y;
        queue->rear = (queue->rear + 1) % QUEUE_CAPACITY;
        queue->count++;
    }
    pthread_mutex_unlock(&forest_mutex);
}

// Função para retirar mensagens da fila
int dequeue_message(int* x, int* y, MessageQueue* queue) {
    pthread_mutex_lock(&forest_mutex);
    if (queue->count > 0) {
        *x = queue->x[queue->front];
        *y = queue->y[queue->front];
        queue->front = (queue->front + 1) % QUEUE_CAPACITY;
        queue->count--;
        pthread_mutex_unlock(&forest_mutex);
        return 1;
    }
    pthread_mutex_unlock(&forest_mutex);
    return 0;
}
