#include <sys/socket.h>
#include <signal.h>
#include <pthread.h>

#include "../commun/commun.h"
#include "../serveur/imgdist.h"
#include "connect_server.h"
#include "client_message_transmission.h"
#include "async_client.h"


#define DEFAULT_SERVER_IP "127.0.0.1"

void handle_parameters(int argc, char* argv[], char** server_ip);

void SignalHandler(int signal);

pthread_t thread_sending, thread_receiving;

int main(int argc, char* argv[]) {

   

   char* server_ip;
   handle_parameters(argc, argv, &server_ip);


   int sock = checked(socket(AF_INET, SOCK_STREAM, 0));
   connect_to_server(sock, server_ip);

   sigset_t set;

   sigemptyset(&set) ; // Ensemble vide de signaux
   sigaddset(&set , SIGINT ) ; // Ajouter le signal SIGINT
   if(pthread_sigmask(SIG_BLOCK, &set , NULL) != 0) {
      perror("pthread_sigmask()") ;
      return 1;
   }

   pthread_create(&thread_receiving, NULL, receiving_thread, &sock);
   pthread_create(&thread_sending, NULL, sending_thread, &sock);

   if(pthread_sigmask (SIG_UNBLOCK, &set , NULL) != 0) {
      perror("pthread_sigmask()") ;
      return 1;
   }

   struct sigaction action;
   action.sa_handler = SignalHandler;
   sigemptyset(&action.sa_mask);
   action.sa_flags = 0;
   if(sigaction(SIGINT, &action, NULL) != 0) {
      perror("sigaction()");
      return 1;
   }

   

   

   pthread_join(thread_sending, NULL);
   pthread_join(thread_receiving, NULL);
   
   close(sock);
   
   return 0;
}


void handle_parameters(int argc, char* argv[], char** server_ip) {
   if (argc > 2) {
      printf("Usage: %s [server_ip]\n", argv[0]);
      exit(1);
   }
   else if (argc == 2) {
      *server_ip = argv[1];
   }
   else {
      *server_ip = DEFAULT_SERVER_IP;
   }
}


void SignalHandler(int signal) {
   switch(signal) {
      case SIGINT:
         printf("SIGINT received.\n");
         //signal_received = 1;
         pthread_kill(thread_sending, SIGUSR1);
         break;
      case SIGUSR1:
         printf("SIGUSR1 received.\n");
         break;
      default:
         printf("Signal %d received.\n", signal);
   }
}