#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#define BUFFER_SIZE 512
#define PORT 6000

int cree_socket_tcp_client() {
  struct sockaddr_in adresse;
  int sock;

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "Erreur socket\n");
    return -1;
  }
  memset(&adresse, 0, sizeof(struct sockaddr_in));
  adresse.sin_family = AF_INET;
  adresse.sin_port = htons(PORT);
  adresse.sin_addr.s_addr = htons(INADDR_ANY);
  if (connect(sock, (struct sockaddr*) &adresse, sizeof(struct sockaddr_in)) < 0) {
    close(sock);
    fprintf(stderr, "Erreur connect\n");
    return -1;
  }
  return sock;
}

int affiche_adresse_socket(int sock) {
  struct sockaddr_in adresse;
  socklen_t longueur;
  longueur = sizeof(struct sockaddr_in);
  if (getsockname(sock, (struct sockaddr*)&adresse, &longueur) < 0) {
    fprintf(stderr, "Erreur getsockname\n");
    return -1;
  }
  printf("IP = %c, Port = %u\n", inet_ntoa(adresse.sin_addr), ntohs(adresse.sin_port));
  return 0;
}

int main(int argc, char**argv) {
  char *choix;
  int sock;
  char buffer[BUFFER_SIZE];
  sock = cree_socket_tcp_client();
  if (sock < 0) {
    puts("Erreur connection socket client");
    exit(1);
  }
   printf("\nBienvenue dans l'application d'achat de billets!\n");
   // tant que l'on ne quitte pas l'app, on fait la boucle
   do {
     printf("Que voulez-vous faire ?\n (r) Réserver un billet\n (a) Annuler une réservation\n (q) Quitter\n");
     scanf("%s", choix);
     // si on réserve
     if (strcmp(choix, "r")==0){
       int place;
       write(sock, "r", BUFFER_SIZE);
       printf("Vous avez choisi de réserver une place, voici la liste des places disponibles : \n");
       // on affiche le message du serveur
       read(sock, buffer, BUFFER_SIZE);
       printf("%s\n", buffer);
       buffer[0]='\0';
       printf("Quelle place souhaitez-vous réserver? \n");
       do {
         scanf("%d", &place);
       } while(place < 0 || place > 99);
       // on envoie la place choisie
       sprintf(choix,"%d",place); //pour passer de int à char
       strcat(buffer,choix);
       write(sock, buffer, BUFFER_SIZE);
       // on lit si la place est dispo
       read(sock, buffer, BUFFER_SIZE);
       if (strcmp(buffer,"1")==0) {
         //  si la place est libre
         buffer[0]='\0';
         printf("Entrez votre nom: ");
         scanf("%s", choix);
         strcat(buffer,choix);
         write(sock, buffer, BUFFER_SIZE);
         buffer[0]='\0';
         printf("Entrez votre prenom: ");
         scanf("%s", choix);
         strcat(buffer,choix);
         write(sock, buffer, BUFFER_SIZE);
         buffer[0]='\0';
         read(sock, buffer, BUFFER_SIZE);
         printf("Place attribuée avec succès!\nVotre n° de dossier est le %s.\n", buffer);
         buffer[0]='\0';
       }
       else printf("place déjà attribuée, veuillez ré-essayer\n" );
       buffer[0]='\0';
     }

     else if (strcmp(choix, "a")==0){
       printf("Vous avez choisi d'annuler une réservation.\n");
       write(sock, "a", BUFFER_SIZE);
       printf("Entrez votre nom: ");
       scanf("%s", choix);
       strcat(buffer,choix);
       write(sock, buffer, BUFFER_SIZE);
       buffer[0]='\0';
       printf("Entrez votre n° de dossier: ");
       scanf("%s", choix);
       strcat(buffer,choix);
       write(sock, buffer, BUFFER_SIZE);
       buffer[0]='\0';
       read(sock, buffer, BUFFER_SIZE);
       printf("%s\n", buffer);
       buffer[0]='\0';
      }
      else if (strcmp(choix, "q")==0){
        printf("Vous quittez l'application\n");
        write(sock,"q",BUFFER_SIZE);
      }
   } while(strcmp(choix, "q")!=0);
  return 0;
}
