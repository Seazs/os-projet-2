#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>

#include "../commun/commun.h"
#include "../serveur/imgdist.h"
#include "connect_server.h"
#include "client_message_transmission.h"


int main(int argc, char* argv[]) {
   // attendre que le serveur soit prêt
   sleep(1);
   signal(SIGPIPE, SIG_IGN);

   int sock = checked(socket(AF_INET, SOCK_STREAM, 0));
   connect_to_server(sock);

   char buffer[1024];
   char closest_image_path[999];
   int *distance = malloc(sizeof(int));

   //printf("Entrez un message ou un chemin d'image: \n");
   while (fgets(buffer, 1024, stdin) != NULL) {
      if(send_image(sock, buffer)){
         //printf("Image envoyée\n");
         receive_result(sock, closest_image_path, distance);
         printf("Most similar image found: '%s' with a distance of %d.\n", closest_image_path, *distance);
      }
      // else if (send_message(sock, buffer)) {
      //    printf("Message envoyé\n");
      //    receive_message(sock, buffer);
      // }
      else{
         printf("No similar image found (no comparison could be performed successfully).");
      }
      //printf("Entrez un message ou un chemin d'image: \n");
   }
   send_annonce(sock, "exit");
   close(sock);
   free(distance);
   return 0;
}


