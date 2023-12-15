#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdatomic.h>
#include <unistd.h>
#include <signal.h>

#include "async_client.h"
#include "client_message_transmission.h"

atomic_int is_connected = 1;
atomic_int images_sent = 0;
atomic_int images_received = 0;

volatile sig_atomic_t signal_received = 0;

void * sending_thread(void * arg){
    int sock = *(int*)arg;
    char buffer[1024];
    while(signal_received == 0 && fgets(buffer, 1024, stdin) != NULL){
        if(!send_image(sock, buffer)){
            printf("No similar image found (no comparison could be performed successfully).");
        }
        else{
            images_sent++;
        }
        while(images_sent - images_received > 4){
            usleep(300000);
        }
    }
    send_annonce(sock, "exit");
    is_connected = 0;
    return NULL;
}

void * receiving_thread(void * arg){
    int sock = *(int*)arg;
    char buffer[1024];
    while((is_connected == 1 || images_received < images_sent) && signal_received == 0){
        //printf("images_received: %d, images_sent: %d\n", images_received, images_sent);
        receive_message(sock, buffer);
        // if (strcmp(buffer, "end") == 0){
        //     break;
        // }
        printf("%s\n", buffer);
        images_received++;
    }
    //send_annonce(sock, "exit");
    return NULL;
}