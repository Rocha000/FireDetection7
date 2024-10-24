#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <pthread.h>

#define QUEUE_CAPACITY 100 // Capacidade da fila de mensagens

typedef struct {
    int x, y;               // Posições do sensor
    pthread_mutex_t lock;   // Mutex para exclusão mútua
    pthread_cond_t cond;    // Variável de condição
} SensorNode;

void initialize_forest(char forest[30][30]);
void* sensor_thread_function(void* arg);
void* fire_generator(void* arg);
void* control_center(void* arg);
void fight_fire(int x, int y, char forest[30][30]);
void print_forest(char forest[30][30]);
void* forest_visualizer(void* arg);

// Fila de mensagens para o Control Center
typedef struct {
    int x[QUEUE_CAPACITY];
    int y[QUEUE_CAPACITY];
    int front;
    int rear;
    int count;
} MessageQueue;

void enqueue_message(int x, int y, MessageQueue* queue);
int dequeue_message(int* x, int* y, MessageQueue* queue);

#endif
