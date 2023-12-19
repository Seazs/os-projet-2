/**
 * Un main avec quelques exemples pour vous aider. Vous pouvez
 * bien entendu modifier ce fichier comme bon vous semble.
 **/

#include <stdio.h>
#include <inttypes.h>
#include <signal.h>
#include <sys/socket.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>

#include "imgdist.h"
#include "../commun/commun.h"
#include "create_server.h"
#include "handle_client.h"
#include "database.h"

void ExempleSignaux(void);



int main() {
   // du signal SIGPIPE.
   signal(SIGPIPE, SIG_IGN);
   

   // /// Exemple gestion de signaux (cf Annexe de l'énoncé & corrigé du projet 1) ///
   
   //ExempleSignaux();
   
   int server_fd =  checked(socket(AF_INET, SOCK_STREAM, 0));
   create_socket(server_fd);
   
   init_images_path();
   //printf("Serveur en écoute\n");

   accept_connections(server_fd);
   
   
   
   close(server_fd);
   clean_images_path();
   
   return 0;
}

static volatile sig_atomic_t signalRecu = 0;
void SignalHandler(int sig) {
   signalRecu = 1;
}

void ExempleSignaux(void) {
   /// Exemple gestion de signaux (cf Annexe de l'énoncé & corrigé du projet 1) ///
   
   // Forcer l'interruption des appels systèmes lors de la réception de SIGINT
   struct sigaction action;
   action.sa_handler = SignalHandler;
   sigemptyset(&action.sa_mask);

   if (sigaction(SIGINT, &action, NULL) < 0) {
      perror("sigaction()");
      return;
   }
   
   
   // Gestion idéale (court et sans risque d'accès concurrents) d'un signal
   // (cf SignalHandler() également).
   printf("Signal recu : %d.\n", signalRecu);
   raise(SIGINT);
   printf("Signal recu : %d.\n", signalRecu);
   
   
   // Bloquer des signaux pour le thread courant
   sigset_t set;
    
   sigemptyset(&set);        // Ensemble vide de signaux
   sigaddset(&set, SIGINT);  // Ajouter le signal SIGINT
   sigaddset(&set, SIGUSR1); // Ajouter le signal SIGUSR1
    
   if (pthread_sigmask(SIG_BLOCK, &set, NULL) != 0) {
      perror("pthread_sigmask()");
      return;
   }
   
   /// ///
}


