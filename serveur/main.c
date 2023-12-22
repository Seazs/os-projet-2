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



int main() {
   
   int server_fd =  checked(socket(AF_INET, SOCK_STREAM, 0));

   create_socket(server_fd);
   
   init_images_path();

   accept_connections(server_fd);
   
   close(server_fd);

   clean_images_path();
   
   return 0;
}





