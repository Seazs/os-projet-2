#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdatomic.h>

#include "async_client.h"
#include "client_message_transmission.h"

int is_connected = 1;

void * sending_thread(void * arg){
    int sock = *(int*)arg;
    char buffer[1024];
    while(fgets(buffer, 1024, stdin) != NULL){
        if(!send_image(sock, buffer)){
            printf("No similar image found (no comparison could be performed successfully).");
        }
    }
    send_annonce(sock, "exit");
    is_connected = 0;
}

void * receiving_thread(void * arg){
    int sock = *(int*)arg;
    char buffer[1024];
    while(is_connected){
        receive_message(sock, buffer);
        printf("%s", buffer);
    }
    receive_message(sock, buffer);
}