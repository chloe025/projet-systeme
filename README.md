# projet-systeme : Gestion des tickets pour un concert

### Nos choix :
#### Le stockage des informations
Pour stocker nos tickets, nous avons décidé d'utiliser un fichier .txt. En effet, nous voulions que l'application soit la plus réaliste possible, donc nous voulions pouvoir avoir des données sauvegardées même en redemarrant le serveur. Par conséquent, pour pouvoir stocker des informations sans passer par une base de données, nous avons essayé de stocker les informations dans un fichier texte. Au démarrage du serveur, si le fichier de 
tickets est vide, nous l'initialisons avec des données vides. Puis, on se sert de ces données pour les sauvergarder dans un tableau 
grâce à la fonction lire(). Pour chaque réservation ou annulation, ce tableau sera mis à jour avec les nouvelles données, et actualisera par la même 
occasion le fichier des tickets.

#### Création de plusieurs connexions
Pour permettre à notre serveur de supporter plusieurs connexions, nous avons décidé de créer des processus fils à l'aide de fork.

#### Terminer le programme
Une des fonctionnalités que l'on trouvait important était de pouvoir permettre à l'utilisateur de quitter l'application quand il le souhaite. Nous avons alors mis
en place des boucles qui se répète tant que l'utilisateur n'a pas demandé à quitter, permettant ainsi de faire plusieurs demandes de réservations ou d'annulation d'affilé. 


#### Ce que l'on peut faire :
Il est possible de réserver une ou plusieurs places au choix, sauf si celle-ci est déjà réservée. A la fin de la réservation, le client reçoit son numéro de dossier qui lui permettra d'annuler sa réservation si il fournit son nom et ce numéro.

### Compiler :
se placer dans les bons dossiers puis : 
 - serveur : 	gcc serveur.c -o serveur
 - client : 	gcc client.c -o client
