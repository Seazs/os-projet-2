#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "image_comparison.h"
#include "imgdist.h"
#include "database.h"


#define NB_THREADS 3


typedef struct {
    int thread_id;
    char *raw_image;
    int images_per_thread;
    int best_distance;
    char *best_image_path;
    pthread_mutex_t mutex
} Thread_data;

unsigned int compare_image(char *raw_image, char *db_image){
    uint64_t hash1, hash2;
    unsigned int size_raw_image = sizeof(raw_image);
    if (!PHashRaw(raw_image, size_raw_image, &hash1)) {
        printf("Erreur lors du calcul du hash de l'image reçue\n");
        return 1;
    }

    if(!PHash(db_image, &hash2)){
        printf("Erreur lors du calcul du hash de l'image de la base de données\n");
        return 1;
    }

    unsigned int distance = DistancePHash(hash1, hash2);
    printf("Distance entre les deux images : %d\n", distance);
    return distance;

}

void *compare_images_thread(void *arg){
   Thread_data *thread_data = (Thread_data *)arg;
   printf("Thread %d\n", thread_data->thread_id);

    for(int i = thread_data->thread_id * thread_data->images_per_thread; i < (thread_data->thread_id + 1) * thread_data->images_per_thread; i++){
        printf("Thread %d : %s\n", thread_data->thread_id, db_images_path.images_path[i]);
        unsigned int distance = compare_image(thread_data->raw_image, db_images_path.images_path[i]);
        if(distance < thread_data->best_distance){
            pthread_mutex_lock(&thread_data->mutex);
            thread_data->best_distance = distance;
            thread_data->best_image_path = db_images_path.images_path[i];
            pthread_mutex_unlock(&thread_data->mutex);
        }
    }
}

void handle_threads(char *raw_image, int client_socket){
    pthread_t threads[NB_THREADS];
    Thread_data thread_data[NB_THREADS];

    // lance les threads
    for (int i = 0; i<NB_THREADS; i++){
        thread_data[i].thread_id = i;
        thread_data[i].raw_image = raw_image;
        thread_data[i].images_per_thread = db_images_path.file_count / NB_THREADS ;
        thread_data[i].best_distance = 100; // 100 est la distance maximale entre deux images
        thread_data[i].best_image_path = NULL;

        if(pthread_mutex_init(&thread_data[i].mutex, NULL) != 0){
            perror("pthread_mutex_init()");
            exit(1);
        }
        if(pthread_create(&threads[i], NULL, compare_images_thread, (void *)&thread_data[i]) != 0){
            perror("pthread_create()");
            exit(1);
        }
    }
    // attend la fin des threads
    for(int i = 0; i<NB_THREADS; i++){
        if(pthread_join(threads[i], NULL) != 0){
            perror("pthread_join()");
            exit(1);
        }
    }
}



