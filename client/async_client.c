#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdatomic.h>
#include <unistd.h>
#include <signal.h>

#include "async_client.h"
#include "client_message_transmission.h"
#include "io.h"

#define MAX_IMAGE_PATH_LENGTH 999

atomic_int is_connected = 1;
atomic_int images_sent = 0;
atomic_int images_received = 0;

volatile sig_atomic_t signal_received = 0;
volatile sig_atomic_t sigpipe_receive = 0;



void * sending_thread(void * arg){

    struct sigaction action;   
    action.sa_handler = Thread_Signal_Handler;
    sigemptyset(&action.sa_mask);
    if(sigaction(SIGPIPE, &action, NULL) != 0) {
        perror("sigaction()");
        return NULL;
    }
    if(sigaction(SIGUSR1, &action, NULL) != 0) {
        perror("sigaction()");
        return NULL;
    }
    

    int sock = *(int*)arg;

    char buffer[MAX_IMAGE_PATH_LENGTH + 1];
    unsigned int i = 0;
    read_state_t state;

    do {
        state = ReadLine(STDIN_FILENO, buffer, MAX_IMAGE_PATH_LENGTH + 1, &i);
        
        if(signal_received == 1){
            break;
        }

        switch (state){
        case READ_EOF:
            break;
        case READ_OK:
            if(!send_image(sock, buffer)){
                printf("No similar image found (no comparison could be performed successfully).\n");
            }
            else{
                images_sent++;
            }
            while(images_sent - images_received > 4){
                usleep(300000);
            }
            i = 0;
            break;
        case READ_INTERRUPTED:
            printf("Reading interrupted.\n");
            sleep(3);
            break;
        case READ_ERROR:
            fprintf(stderr, "An error occured while reading the input.\n");
        case READ_TOO_LARGE:
            fprintf(stderr, "Data is too large to be stored in the buffer.\n");
            break;
        default:
            break;
        }

        if(signal_received == 1){
            break;
        }
        
    } while (state != READ_EOF);
    if (sigpipe_receive == 1){
        printf("Connection closed by the server.\n");
    }
    else{
        printf("Connection closed.\n");
        send_annonce(sock, "exit");
    }
    // faut pas envoyer si c'est le serveur qui a fermé la connexion 
    is_connected = 0;
    return NULL;
}

void * receiving_thread(void * arg){

    struct sigaction action;
    action.sa_handler = SIG_IGN;
    sigemptyset(&action.sa_mask);
    if(sigaction(SIGUSR1, &action, NULL) != 0) {
        perror("sigaction()");
        return NULL;
    }
    action.sa_handler = Thread_Signal_Handler;
    sigemptyset(&action.sa_mask);
    if(sigaction(SIGPIPE, &action, NULL) != 0) {
        perror("sigaction()");
        return NULL;
    }
    
    
    
    int sock = *(int*)arg;
    char buffer[1024];
    while((is_connected == 1 || images_received < images_sent) && signal_received == 0){
        if(signal_received == 1){
            break;
        }
        if(receive_message(sock, buffer)){
            break;
        }
        if(signal_received == 1){
            break;
        }
        
        printf("%s\n", buffer);
        images_received++;
    }
    //send_annonce(sock, "exit");
    return NULL;
}

void Thread_Signal_Handler(int signal) {
   switch (signal) {
    case SIGPIPE:
        printf("SIGPIPE\n");
        signal_received = 1;
        sigpipe_receive = 1;
        break;
    case SIGUSR1:
        printf("SIGUSR1\n");
        signal_received = 1;
        break;
    default:
        printf("Unknown signal\n");
        break;
   }
}