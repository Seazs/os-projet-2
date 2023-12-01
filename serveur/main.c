/**
 * Un main avec quelques exemples pour vous aider. Vous pouvez
 * bien entendu modifier ce fichier comme bon vous semble.
 **/

#include <stdio.h>
#include <inttypes.h>
#include <signal.h>
#include <sys/socket.h>
#include <string.h>

#include "imgdist.h"
#include "../commun/commun.h"
#include "create_server.h"

void ExempleSignaux(void);


int main(int argc, char* argv[]) {
   // Permet que write() retourne 0 en cas de réception
   // du signal SIGPIPE.
   signal(SIGPIPE, SIG_IGN);
   
   // /// Exemple d'utilisation de la biliothèque img-dist ///
   // uint64_t hash1, hash2;
   
   // // Calcule du code de hachage perceptif de l'image "img/1.bmp" et
   // // conservation de celui-ci dans hash1.
   // if (!PHash("img/1.bmp", &hash1))
   //    return 1; // Échec dans le chargement de l'image (message sur stderr automatique)
   
   // // Idem pour "img/2.bmp".
   // if (!PHash("img/2.bmp", &hash2))
   //    return 1;
   
   // // Calculer la distance entre hash1 et hash2
   // unsigned int distance = DistancePHash(hash1, hash2);
   
   // // Afficher la distance entre les deux images (valeur de retour d'img-dist dans le projet 1
   // // quand il n'y avait pas d'erreur).
   // printf("Distance entre les images 'img/1.bmp' et 'img/2.bmp' : %d\n", distance);
   
   // /// Fin de l'exemple d'utilisation de la biliothèque img-dist ///
   
   // /// Exemple gestion de signaux (cf Annexe de l'énoncé & corrigé du projet 1) ///
   
   // //ExempleSignaux();
   
   int server_fd =  checked(socket(AF_INET, SOCK_STREAM, 0));
   int serveur_socket = create_socket(server_fd);

   
   char buffer[1024];
   uint32_t longueur;
   
   // while(lire_exactement(serveur_socket, (char*)&longueur, sizeof(longueur))
   //       && lire_exactement(serveur_socket, buffer, ntohl(longueur))){
   //    printf("Recu du cote serveur : %s\n", buffer);
   //    send_message(serveur_socket, buffer);
   // }

   while(1){
      printf("1\n");
      if(!lire_exactement(serveur_socket, (char*)&longueur, sizeof(longueur))){
         break;
      }
      printf("2\n");
      if(!lire_exactement(serveur_socket, buffer, ntohl(longueur))){
         break;
      }
      printf("3\n");
      printf("Recu du cote serveur : %s\n", buffer);
      send_message(serveur_socket, buffer);
   }
   
   
   /// ///
   
   
   close(server_fd);
   close(serveur_socket);
   
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