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

void ExempleSignaux(void);

char **images_path;

int compareStrings(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

void init_images_path(){
   DIR *d;
   struct dirent *dir;
   int file_count = 0;

   d = opendir("img/");
   if (d == NULL) {
      perror("erreur lors de l'ouverture du dossier img");
      exit(1);
   }

   
   while ((dir = readdir(d)) != NULL) {
      images_path = realloc(images_path, sizeof(char*) * (file_count + 1));
      images_path[file_count] = malloc(sizeof(char) * (strlen(dir->d_name) + 1));
      strcpy(images_path[file_count], dir->d_name);
      file_count++;
   }
   closedir(d);

   
   qsort(images_path, file_count, sizeof(char*), compareStrings);
}

int main(int argc, char* argv[]) {
   init_images_path();
   //imprime les chemin des images
   for(int i = 0; i < 100; i++){
      printf("%s\n", images_path[i]);
   }

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
   
   ExempleSignaux();
   
   int server_fd =  checked(socket(AF_INET, SOCK_STREAM, 0));
   int serveur_socket = create_socket(server_fd);

   
   char buffer[1024];
   

   while(1){
      handle_server_response(serveur_socket);
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