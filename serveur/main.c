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
   

   while(1){

      

      receive_message(serveur_socket, buffer);
      printf("Annonce Recu du cote serveur : %s\n", buffer);
      clean_str(buffer);

      if(strcmp(buffer, "img") == 0){
         char raw_image[20000];
         printf("ouverture du fichier image_recue.bmp\n");
         FILE *image_file = fopen("image_recue.bmp", "wb");
         if(!receive_image(serveur_socket, raw_image, image_file)){
            fclose(image_file);
            
            send_message(serveur_socket, "Image reçue avec succès");
         }
         else{
            printf("Erreur lors de la réception de l'image\n");
         }
      }
      else if(strcmp(buffer, "message") == 0){
         printf("Server:  Message reçu \n");
         if(!receive_message(serveur_socket, buffer)){
            printf("Recu du cote serveur : %s\n", buffer);
            if (strcmp(buffer, "exit") == 0) {
               printf("Serveur déconnecté\n");
               send_message(serveur_socket, "serveur déconnecté");
               break;
            }
            send_message(serveur_socket, buffer);
         }
         printf("lala\n");
      }
      else{
         printf("Erreur, pas de signal d'annonce reçu\n");
      }
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