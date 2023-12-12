// Inclusion des bibliothèques nécessaires
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>

#include "create_server.h"  // Inclusion du fichier d'en-tête pour la création du serveur
#include "../commun/commun.h"  // Inclusion du fichier d'en-tête commun
#define MAX_CONNEXION 1000  // Définition du nombre maximum de connexions

/**
 * Crée un socket et retourne le nouveau descripteur de socket.
 * @param server_fd Le descripteur de fichier pour le socket du serveur.
 * @return Le nouveau descripteur de socket.
 */
void create_socket(int server_fd){
    // Création d'un descripteur de fichier pour le socket
    int opt = 1;
    // Configuration du socket pour réutiliser l'adresse et le port
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    // Configuration de l'adresse du serveur
    struct sockaddr_in address;
    address.sin_family = AF_INET;  // Famille d'adresses Internet
    address.sin_addr.s_addr = INADDR_ANY;  // Autorise le socket à accepter les connexions de n'importe quelle adresse IP
    address.sin_port = htons(PORT);  // Conversion du numéro de port en format réseau

    // Liaison du socket à l'adresse et au port spécifiés
    checked(bind(server_fd, (struct sockaddr *)&address, sizeof(address)));
    // Mise en écoute du socket pour les connexions entrantes
    checked(listen(server_fd, MAX_CONNEXION));
}