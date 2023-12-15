#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>
#include <pthread.h>

#include "../commun/commun.h"
#include "../serveur/imgdist.h"
#include "connect_server.h"
#include "client_message_transmission.h"
#include "async_client.h"

#define DEFAULT_SERVER_IP "127.0.0.1"


int main(int argc, char* argv[]) {

   char* server_ip;

   if (argc > 2) {
      printf("Usage: %s [server_ip]\n", argv[0]);
      return 1;
   }
   else if (argc == 2) {
      server_ip = argv[1];
   }
   else {
      server_ip = DEFAULT_SERVER_IP;
   }

   // attendre que le serveur soit prêt
   //sleep(1);
   signal(SIGPIPE, SIG_IGN);

   int sock = checked(socket(AF_INET, SOCK_STREAM, 0));
   connect_to_server(sock, server_ip);

   //printf("Entrez un message ou un chemin d'image: \n");
   pthread_t thread_sending, thread_receiving;

   pthread_create(&thread_sending, NULL, sending_thread, &sock);
   pthread_create(&thread_receiving, NULL, receiving_thread, &sock);

   pthread_join(thread_sending, NULL);
   pthread_join(thread_receiving, NULL);
   
   close(sock);
   
   return 0;
}


