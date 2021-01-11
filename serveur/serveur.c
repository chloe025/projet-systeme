#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#define BUFFER_SIZE 512
#define PORT 6000

typedef struct ticket {
  char nodossier[12];
  char nom[30];
  char prenom[30];
  int dispo;
} ticket ;

//table des tickets
ticket table[100];

void traite_connection(int sock, ticket T[], FILE *fichier);
int affiche_adresse_socket(int sock);
int cree_socket_tcp_ip();
void lire(ticket table[], FILE* fichier);
void inserFichier(ticket table[], FILE* fichier);

int main(int argc, char const *argv[]) {
  int sock_contact;
  int sock_connectee;
  struct sockaddr_in adresse;
  socklen_t longueur;
  pid_t pid_fils;
  srand((unsigned int)time(NULL));
  FILE* fichier = NULL;
    fichier = fopen("ticket.txt", "r+");
    // si le fichier est vide, on l'initialise
  if (fichier != NULL){
    int caractereLu = 0;
    caractereLu = fgetc(fichier); // On lit le caractère
    // si le tableau est vide
    if (caractereLu==EOF) {
      printf("fichier vide, on le remplit \n");
      for (int i = 0; i<100; i++){
        fprintf(fichier, "-1 NULL NULL 0\n");
      }
    }
    fclose(fichier);
  }
  // on a fermé le fichier, on lit les infos.
  lire(table,fichier);

  sock_contact = cree_socket_tcp_ip();
  if (sock_contact < 0)
    return -1;

  listen(sock_contact, 5);
  printf("Mon adresse (sock contact) -> ");
  affiche_adresse_socket(sock_contact);

  while (1) {
    longueur = sizeof(struct sockaddr_in);
    sock_connectee = accept(sock_contact,
    (struct sockaddr*)&adresse,
    &longueur);
    if (sock_connectee < 0) {
      fprintf(stderr, "Erreur accept\n");
      return -1;
    }
    pid_fils = fork();
    if (pid_fils == -1) {
      fprintf(stderr, "Erreur fork\n");
      return -1;
    }
    if (pid_fils == 0) { //fils
      close(sock_contact);
      traite_connection(sock_connectee, table, fichier);
      exit(0);
    }
    else
      close(sock_connectee);
    }
    return 0;
}
int affiche_adresse_socket(int sock) {
  struct sockaddr_in adresse;
  socklen_t longueur;
  longueur = sizeof(struct sockaddr_in);
  if (getsockname(sock, (struct sockaddr*)&adresse, &longueur) < 0) {
    fprintf(stderr, "Erreur getsockname \n");
    return -1;
  }
  printf("IP = %s, Port = %u\n", inet_ntoa(adresse.sin_addr), ntohs(adresse.sin_port));
  return 0;
}

void traite_connection(int sock, ticket T[], FILE* fichier) {
  struct sockaddr_in adresse;
  socklen_t longueur;
  char bufferR[BUFFER_SIZE];
  char bufferW[BUFFER_SIZE];
  int nb, t, i, place;
  srand(time(NULL));
  // pour inserer nos donnees
  char fname[30];
  char name[30];
  char doss[12];
  // connexion au serveur
  longueur = sizeof(struct sockaddr_in);
  if (getpeername(sock, (struct sockaddr*) &adresse, &longueur) < 0) {
    fprintf(stderr, "Erreur getpeername\n");
    return;
  }
  printf("Connexion : locale (sock_connectee) ");
  affiche_adresse_socket(sock);
  // tant que l'user veut continuer, on récup la saisie
  do {
    lire(table,fichier);
    // on lit la reponse de l'user
    nb= read(sock, bufferR, BUFFER_SIZE);
    if (strcmp(bufferR,"r")==0) {
      char tempo[BUFFER_SIZE];
      bufferR[0]='\0';
      int dispo=0;
      // on regarde si il reste des places de libre
      for (i = 0; i < 100; i++) {
        if (T[i].dispo == 0) {
          sprintf(bufferR,"%d ",i);
          dispo=1;
        }
        strcat(tempo,bufferR);

        bufferR[0]='\0';
      }
      // si il reste des places de libre
      if (dispo ==0) {
        strcat(bufferW, "il n'y a plus de place");
        write(sock, bufferW, BUFFER_SIZE);
      }
      else{
        write(sock, tempo, BUFFER_SIZE);
        tempo[0]='\0';
        // recuperation du numero de place
        read(sock, bufferR, BUFFER_SIZE);
        // pour passer de la chaine au int
        place=atoi(bufferR);
        // si la place est déjà prise
        if (T[place].dispo==1){
          write(sock, "Place déjà prise", BUFFER_SIZE);
        }
        else{
          write(sock,"1",BUFFER_SIZE);
          printf("Place : %d\n", place);
          bufferR[0]='\0';
          //recuperation du nom du client
          read(sock, name, BUFFER_SIZE);
          bufferR[0]='\0';
          //recuperation du prenom du client
          read(sock, fname, BUFFER_SIZE);
          bufferR[0]='\0';
          //creation du numero de dossier
          int t=0;
          while (t < 10) {
            doss[t] = '0'+(rand()%10);
            t++;
          }
          // insertion des données dans le tableau
          strcpy(T[place].nom, name);
          strcpy(T[place].prenom, fname);
          strcpy(T[place].nodossier, doss);
          T[place].dispo=1;
          printf("Recap : place %d, %s %s %s %d\n",place,T[place].nodossier, T[place].nom, T[place].prenom, T[place].dispo );
          bufferW[0]='\0';
          // envoi du n° de dossier au client
          strcat(bufferW, T[place].nodossier);
          write(sock, bufferW, BUFFER_SIZE);
          bufferW[0]='\0';
          inserFichier(T, fichier);
        }
      }

    bufferW[0]='\0';
    }

    else if (strcmp(bufferR,"a")==0) {
      int trouve = 0;
      bufferR[0]='\0';
      //recuperation du nom du client
      read(sock, bufferR, BUFFER_SIZE);
      strcpy(name,bufferR);
      bufferR[0]='\0';
      printf("Nom du client qui souhaite annuler : [%s]\n", name);
      //recuperation du n° de dossier du client
      read(sock, bufferR, BUFFER_SIZE);
      strcpy(doss,bufferR);
      bufferR[0]='\0';
      printf("N° de dossier : [%s]\n", doss);
      // on cherche si un ticket correspond aux infos données
      for (i=0; i<100; i++){
        if ((strcmp(T[i].nom, name)==0)&&(strcmp(T[i].nodossier, doss)==0)){
          trouve =1;
          break;
        }
      }
      if (trouve==0) {
        strcat(bufferR,"Le nom et le n° de dossier ne correspondent pas.\n");
        write(sock, bufferR, BUFFER_SIZE);
        bufferR[0]='\0';
      }
      else {
        strcpy(T[i].nom, "NULL");
        strcpy(T[i].prenom, "NULL");
        strcpy(T[i].nodossier, "-1");
        T[i].dispo=0;
        strcat(bufferR,"Annulation réalisée avec succès!\n");
        write(sock, bufferR, BUFFER_SIZE);
        bufferR[0]='\0';
        inserFichier(T, fichier);
      }
    }
  } while(strcmp(bufferR,"q")!=0);
}



int cree_socket_tcp_ip() {
  int sock;
  struct sockaddr_in adresse;
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "Erreur socket\n");
    return -1;
  }
  memset(&adresse, 0, sizeof(struct sockaddr_in));
  adresse.sin_family = AF_INET;
  // donner un numro de port disponible quelconque
  adresse.sin_port = htons(PORT);
  // aucun contrle sur l'adresse IP :
  adresse.sin_addr.s_addr = htons(INADDR_ANY);
  // Autre exemple :
  // connexion sur le port 33016 fix
  // adresse.sin_port = htons(33016);
  // depuis localhost seulement :
  // inet_aton("127.0.0.1", &adresse.sin_addr);
  if (bind(sock, (struct sockaddr*) &adresse, sizeof(struct sockaddr_in)) < 0) {
    close(sock);
    fprintf(stderr, "Erreur bind\n");
    return -1;
  }
  return sock;
}

void lire(ticket table[], FILE* fichier){
  fichier = fopen("ticket.txt", "r");
  if (fichier != NULL){
    for(int i=0;i<100;i++){
      fscanf(fichier, "%s %s %s %d", table[i].nodossier, table[i].nom, table[i].prenom, &table[i].dispo);
    }
    fclose(fichier);
  }
}
void inserFichier(ticket table[], FILE* fichier){
  fichier =fopen("ticket.txt","w");  // on ferme le fichier et on le réouvre en -w
  for (int i = 0; i<100; i++){
    fprintf(fichier, "%s %s %s %d \n", table[i].nodossier, table[i].nom, table[i].prenom, table[i].dispo);
  }
  fclose(fichier);
}
