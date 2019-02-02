/**
 * \file mainClient.c
 * \brief Le main du Client
 * \date 6 décembre 2018
 * \author Loïc Lemaire, Loïc Jovanovic, Robinson Lestani, Quentin Meliard
 * Programme qui lance l'application client
 *
 */
 
#include "mainClient.h"


 /**
 * \fn int main (int argc, char *argv[])
 * \brief Programme qui lance l'application client 
 * \param arc nombre de fichiers (limité à deux)
 * \param argv, en premier nom du port (sup à 3000) et ensuite le nom de serveur.
 * \return 0 en cas de succès sinon -1 ou 1
 */
int main (int argc, char *argv[]){
	
	//On veifie qu'il y a bien 3 argument (1.commande 2.port 3.adresse du serveur)
	if(argc !=3){
		fprintf(stderr,"Deux arguments demandés : numéro de port puis adresse serveur \n");
		return -1;
	}

	unsigned int noPort;
	//On verifie que le port est bien un entier superieur a 0
	if((atoi(argv[1]))<=0){
		fprintf(stderr,"Le numéro de port doit etre un entier positif supérieur à zéro\n");
		exit(-1);
	}else{
		noPort = atoi(argv[1]);
	}

	// Récupération du port entré en parametre mieux gérer l'argument
	printf("%d\n",noPort );

	struct sockaddr_in s;
	struct hostent *h;
	
	int sock;

	//Création de l'handler pour la fin des fils
	struct sigaction a;
	a.sa_handler = finProcessusFils;
	a.sa_flags = SA_RESTART;
	sigaction(SIGCHLD, &a, NULL);

	// Création de socket (primitive socket)

	if((sock=socket(AF_INET,SOCK_STREAM,0))==-1){
		fprintf(stderr,"Probleme d'ouverture de socket\n");
		return 1;
	}

	//On récupere l'adresse du serveur
	if((h = gethostbyname(argv[2]))==NULL){
		fprintf(stderr,"Probleme de récuperation hostname\n");
		return -1;
	}

	// Préparation des champs sin_family, sin_port et sin_addr.s_addr de la  variable S

	s.sin_family = AF_INET;
	memcpy(&s.sin_addr.s_addr,h->h_addr,h->h_length);
	s.sin_port = htons(noPort); 

	fprintf(stderr, "%x\n", s.sin_addr.s_addr);

	//On se connecte avec le serveur

	if((connect(sock,(struct sockaddr *)&s,sizeof(s)))==-1){
		fprintf(stderr,"Problème de connection au serveur\n");
		return -1;
	}

	processusClient(sock);

	//Début des dialogues avec le serveur




	return 0;

	



}


/**
 * \fn int void signalHandler()
 * \brief Fonction qui vas kill les processus fils stockés dans pid[] 
 */
void signalHandler(){
		printf("Killing process %d\n",getpid() );
		kill(getpid(),SIGKILL);
}

 /**
 * \fn void finProcessusFils()
 * \brief Attend la mort du processus fils
 */
void finProcessusFils(){
	fprintf(stderr, "Un processus fils est terminé\n");
	wait(NULL);
}
