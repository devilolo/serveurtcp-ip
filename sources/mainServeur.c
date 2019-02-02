/**
 * \file mainServeur.c
 * \brief Le main du Serveur
 * \date 3 décembre 2018
 * \author Loïc Lemaire, Loïc Jovanovic, Robinson Lestani, Quentin Meliard
 * Programme qui lance l'application serveur
 */
#include "mainServeur.h"

 /**
 * \fn int main (int argc, char *argv[])
 * \brief Programme qui lance l'application serveur 
 * \param arc nombre de fichiers (limité à 1)
 * \param argv, uniquement nom du port (sup à 3000)
 * \return 0 en cas de succès sinon -1 ou 1
 */
int main(int argc,char *argv[]){


	//Gestion du paramètre pour que l'on nous donne un port
	if(argc != 2){
		fprintf(stderr,"Probleme d'argument : numéro de port requis\n");
		return 1;
	}

	//Recuperation du port d'entrée en paramètre
	unsigned int noPort;
	if((atoi(argv[1]))<=0){
		fprintf(stderr,"Le numéro de port doit etre un entier positf supérieur a zéro\n");
		exit(-1);
	}else{
		noPort = atoi(argv[1]);
		char name[100];
		if(gethostname(name,100)==-1){
			fprintf(stderr, "Erreur de récuperation du nom du serveur\n");
			exit(-1);
		}
		printf("Le host name est :%s\n", name );
	}

	//Notre struct sockadress_in
	struct sockaddr_in s;

	//Le numéro de descripteur fichier de notre socket listener
	int SocketListener=0;

	if((SocketListener = socket(AF_INET,SOCK_STREAM,0))==-1){
		fprintf(stderr,"Problème d'ouverture du socket listener\n");
		return 1;
	}

	s.sin_family = AF_INET;
	s.sin_addr.s_addr =htonl(INADDR_ANY);
	s.sin_port = htons(noPort);


	//Attachement de la socket (binding de notre socket d'écoute)
	if((bind(SocketListener,(struct sockaddr *)&s,sizeof(s)))==-1){
		fprintf(stderr,"Problème d'ouverture de bind\n");
		return 1;
	}

	//Ouverture de service avec comme descripteur SocketListener, et le nombre d'itération de l'écoute défini 10 
	if((listen(SocketListener,10))==-1){
		fprintf(stderr,"Problème d'ouverture de service\n");
		return 1;
	}

	int infinie = 1;
	unsigned int lengths = sizeof(s);

	//Création de l'handler pour la fin des fils
	struct sigaction a;
	a.sa_handler = finProcessusFils;
	a.sa_flags = SA_RESTART;
	sigaction(SIGCHLD, &a, NULL);

	//Interception du signal (ctrl-c ou sigint)
	signal(SIGINT,signalHandler);

	//Initialisation du client boucle infinie d'écoute, et création de processus fils
	while(infinie){
		int Socketfils = 0;
		if((Socketfils = accept(SocketListener,(struct sockaddr *)&s,&lengths))==-1){
			fprintf(stderr,"Problème d'acceptation de connection\n");
			return 1;
		}


		if(processusFils(SocketListener,Socketfils)==-1){
			fprintf(stderr,"Problème d'ouverture de fils\n");
			return 1;
		}

	}

	return 0;
}




