#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "functions.h"

char forest[30][30];
pthread_mutex_t forest_mutex = PTHREAD_MUTEX_INITIALIZER;

int main() {
    // Inicializa a floresta
    initialize_forest(forest);

    // Cria threads para cada nó sensor
    pthread_t sensor_threads[30][30];
    SensorNode nodes[30][30];

    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 30; j++) {
            nodes[i][j].x = i;
            nodes[i][j].y = j;
            pthread_mutex_init(&nodes[i][j].lock, NULL);
            pthread_cond_init(&nodes[i][j].cond, NULL);
            pthread_create(&sensor_threads[i][j], NULL, sensor_thread_function, &nodes[i][j]);
        }
    }

    // Cria a thread geradora de incêndios
    pthread_t fire_thread;
    pthread_create(&fire_thread, NULL, fire_generator, NULL);

    // Cria a central de controle
    pthread_t control_thread;
    pthread_create(&control_thread, NULL, control_center, NULL);

    // Cria a thread visualizadora da floresta
    pthread_t visualizer_thread;
    pthread_create(&visualizer_thread, NULL, forest_visualizer, NULL);

    // Junta as threads principais
    pthread_join(fire_thread, NULL);
    pthread_join(control_thread, NULL);
    pthread_join(visualizer_thread, NULL);

    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 30; j++) {
            pthread_join(sensor_threads[i][j], NULL);
        }
    }

    return 0;
}
