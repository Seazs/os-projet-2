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
   signal(SIGPIPE, SIG_IGN);

   int sock = checked(socket(AF_INET, SOCK_STREAM, 0));
   connect_to_server(sock);

   char buffer[1024];
   uint32_t longueur;

   while (fgets(buffer, 1024, stdin) != NULL) {
      // longueur = strlen(buffer) + 1;
      // longueur = htonl(longueur);
      // checked_wr(write(sock, &longueur, sizeof(longueur)));
      // longueur = ntohl(longueur);
      // checked_wr(write(sock, buffer, longueur));

      if(!send_image(sock, buffer)){
         printf("Image envoyée\n");
         receive_message(sock, buffer);
      }
      else if (!send_message(sock, buffer)) {
         
         receive_message(sock, buffer);
      }
      else{
         printf("Erreur lors de l'envoi\n");
      }

   }

   close(sock);
   return 0;
}

