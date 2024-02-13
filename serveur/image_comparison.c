#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>

#include "image_comparison.h"
#include "imgdist.h"
#include "database.h"
#include "message_transmission.h"

#define NB_THREADS 3
pthread_t comparing_threads[NB_THREADS]; // Array to store thread IDs
pthread_key_t keys[NB_THREADS]; // Array to store thread-specific data keys

/**
 * @struct Thread_data
 * @brief A struct to hold data for each thread.
 */
typedef struct {
    int thread_id; // The ID of the thread
    Image *image; // The image to compare
    int images_per_thread; // The number of images to compare for this thread
    unsigned int best_distance; // The best distance found by this thread
    char *best_image_path; // The path of the best image found by this thread
    pthread_mutex_t mutex; // The mutex for this thread
    Client *client; // The client
} Thread_data;

/**
 * @brief Compares an image with a database image.
 * 
 * @param image The image to compare.
 * @param db_image The path of the database image to compare with.
 * @return The distance between the two images.
 */
unsigned int compare_image(Image *image, char *db_image){
    uint64_t hash1, hash2;
    unsigned int size_raw_image = image->taille;

    // Calculate the perceptual hash of the database image
    if(!PHash(db_image, &hash2)){
        printf("Error calculating the hash of the database image %s\n", db_image);
        return 100;
    }

    // Calculate the perceptual hash of the received image
    if (!PHashRaw(image->raw_image, size_raw_image, &hash1)) {
         printf("Error calculating the hash of the received image\n");
         return 100;
    }   

    // Calculate the distance between the two hashes
    unsigned int distance = DistancePHash(hash1, hash2);
    return distance;
}

/**
 * @brief Function for each thread to compare images.
 * 
 * @param arg The thread data.
 * @return NULL.
 */
void *compare_images_thread(void *arg){

    //set_comparing_threads_signal_handler(); // Set the signal handler for the comparing threads
    Thread_data *thread_data = (Thread_data *)arg;


    // pthread_key_t key;
    // if(pthread_key_create(&key, NULL) != 0){
    //     perror("pthread_key_create()");
    //     exit(1);
    // }
    // keys[thread_data->thread_id] = key;
    // pthread_setspecific(key, client); // Set the client struct as thread-specific data
    
    // Determine the range of images for this thread to compare
    int start_index = thread_data->thread_id * thread_data->images_per_thread;
    int end_index = (thread_data->thread_id + 1) * thread_data->images_per_thread;
    if(thread_data->thread_id == NB_THREADS - 1){
        end_index = db_images_path.file_count;
    }

    // Compare each image in the range
    for(int i = start_index; i < end_index; i++){

        // If the client has to terminate, set the best distance to 100 and return
        if(thread_data->client->has_to_terminate){
            thread_data->best_distance = 100;
            thread_data->best_image_path = NULL;
            return NULL;
        }

        // Compare the image with the database image
        unsigned int distance = compare_image(thread_data->image, db_images_path.images_path[i]);
        if(distance < thread_data->best_distance){
            thread_data->best_distance = distance;
            thread_data->best_image_path = db_images_path.images_path[i];
        }
    }
    return NULL;
}

// Function to handle threads for image comparison
void handle_threads(Image *image, Client *client){
    
    Thread_data thread_data[NB_THREADS];

    int client_socket = client->socket;

    // Start the threads
    for (int i = 0; i<NB_THREADS; i++){
        thread_data[i].thread_id = i;
        thread_data[i].image = image;
        thread_data[i].images_per_thread = db_images_path.file_count / NB_THREADS ;
        thread_data[i].best_distance = 100; // 100 is the maximum distance between two images
        thread_data[i].best_image_path = NULL;
        thread_data[i].client = client;

        // Initialize the mutex for the thread
        if(pthread_mutex_init(&thread_data[i].mutex, NULL) != 0){
            perror("pthread_mutex_init()");
            exit(1);
        }

        // Create the thread
        if(pthread_create(&comparing_threads[i], NULL, compare_images_thread, (void *)&thread_data[i]) != 0){
            perror("pthread_create()");
            exit(1);
        }
    }

    // Wait for the threads to finish
    for(int i = 0; i<NB_THREADS; i++){
        if(pthread_join(comparing_threads[i], NULL) != 0){
            perror("pthread_join()");
            exit(1);
        }
    }

    // Get the best result
    unsigned int best_distance = 100;
    char *best_image_path = NULL;
    for(int i = 0; i<NB_THREADS; i++){
        if(thread_data[i].best_distance < best_distance){
            best_distance = thread_data[i].best_distance;
            best_image_path = thread_data[i].best_image_path;
        }
    }

    // Prepare the result message
    char *result = (char *)malloc(100);
    if(best_image_path == NULL){
        sprintf(result, "No similar image found (no comparison could be performed successfully).\n");
        printf("No similar image found.\n");
    }else{
        sprintf(result, "Most similar image found: '%s' with a distance of %d.", best_image_path, best_distance);
        printf("Most similar image found: '%s' with a distance of %d.\n", best_image_path, best_distance);
    }

    // Send the result to the client
    send_message(client_socket, result);

    free(result);
}

/**
 * Sets the signal handler for the comparing threads.
 */
// void set_comparing_threads_signal_handler(){
//     struct sigaction sa;
//     sa.sa_handler = comparing_theads_signal_handler;
//     sigemptyset(&sa.sa_mask);
//     sa.sa_flags = 0;
//     sigaction(SIGPIPE, &sa, NULL);

//     sigset_t set;
//     sigemptyset(&set);
//     sigaddset(&set, SIGPIPE);
//     if (pthread_sigmask(SIG_UNBLOCK, &set, NULL) != 0) {
//         perror("pthread_sigmask");
//         exit(1);
//     }
// }

/**
 * Signal handler for the comparing threads.
 * @param signal The signal received.
 */
// void comparing_theads_signal_handler(int signal){
//     if(signal == SIGPIPE){
//         printf("Thread %ld : SIGPIPE\n", pthread_self());
//         pthread_key_t key;
//         for(int i = 0; i<NB_THREADS; i++){
//             printf("Thread %ld : comparing_threads[%d] = %ld\n", pthread_self(), i, comparing_threads[i]);
//             if(pthread_equal(pthread_self(), comparing_threads[i])){
//                 printf("key = %d\n", keys[i]);
//                 pthread_key_t key = keys[i];
//                 break;
//             }
//         }
        
//         Client *client = pthread_getspecific(key); // Get the client struct from thread-specific data
//         printf("lala\n");
//         int number = client->client_number;
//         printf("lolo\n");
//         printf("Client %d disconnected\n", number);
//         for (int i = 0; i < NB_THREADS; i++) {
//             pthread_kill(comparing_threads[i], SIGPIPE);
//         }
//         pthread_kill(client->thread_id, SIGPIPE);

//     }
// }