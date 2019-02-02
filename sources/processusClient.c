/**
 * \file processusClient.c
 * \brief Les fonctions du client
 * \date 3 décembre 2018
 * \author Loïc Lemaire, Loïc Jovanovic, Robinson Lestani, Quentin Meliard
 */

#include "processusClient.h"

 /**
 * \fn processusClient(int sock)
 * \brief 
 * \param sock, la socket de communication
 * \return 0 en cas de succès sinon 1
 */
int processusClient(int sock){
	int infinie = 1;
	while(infinie){

		//Demande la commande voulue avec gestion de l'entrée
		printf("\n\nQue souhaitez vous ?\n Exporter des fichiers(1)\n Importer des fichiers(2)\n Consulter les fichiers(3)\n Quitter (4) \n");
		char choix;
		int value =-1;
		scanf(" %c",&choix);
		value = choix - '0';
		while((value < 1) || (value > 4)){
			scanf(" %c",&choix);
			value = choix - '0';
		}

		writeSocketEntier(sock,value);

		switch(value){
			case EXPORT:
				processExport(sock);
				break;
			case IMPORT:
				processConsult(sock,1);
				break;
			case CONSULT:
				processConsult(sock,0);
				break;
			case QUIT:				
				infinie = quitServeur();
				return 1;
		}
	}
	return 0;
}

 /**
 * \fn void processConsult(int sock,int commande)
 * \brief Permer la consultation et l'importation des images
 * \param sock, la socket de communication
 * \param commande, désigne l'import ou le consult (1 ou 0)
 * 
 * Cette fonction va dans un premier temps récuperer la liste des fichiers du serveur et la mettre sous la forme d'une liste et l'afficher au client
 Si on est passé par la commande IMPORT(2) alors on passe à la demande de récupération des fichiers
 L'utilisateur choisi grâce aux chiffres le fichier voulu. On vérifie qu'il ne donne pas de fichier inexistant.
 Les dossiers 0 et 1 ne sont pas copiables
 */
void processConsult(int sock,int commande){

	if(readSocketEntier(sock)){
		char * fichier = NULL;
		fichier = lectureSocket(sock);
		printf("\nVoici les fichiers du serveur \n");
		//Cette partie sert à séparer la chaine de caractère reçue et la transformer en tableau
		
	    char **tab = NULL;
	    int i=0;
	    //On lance le hashage de la table
	    tab = decoupage(fichier,&i);  

	    //A partir d'ici on exécute uniquement si il a demandé un import
	    if(commande){
	    	//Ici on demande la suite des éléments qu'il demande et on va les concaéner dans une chaîne de caractère
	    	//séparés par des | (pipe)
	    	char * lesimports = NULL;
	    	lesimports = (char *)malloc(sizeof(char));
	    	lesimports[0]='\0';
	    	char scan[256];
	    	int valeur =0,nbfic=0;
	    	printf("\nQuel(s) fichier(s) souhaitez vous recupérer ?\n Donnez le ou leurs numéro(s), puis appuyer sur entrée après chaque numéro, pour arreter tapez -1\n");
	    	while(valeur!=-1){  
	    	if(fgets(scan,256,stdin)==NULL){
	    		printf("Merci de ne pas mettre un nombre aussi grand cela me parait suspect\n");
	    	}		    	
	    	valeur = atoi(scan);
	    	//On vérifie que les fichiers existent bel et bien
		    	if(valeur>1 && valeur<i){
		    		lesimports = (char *)realloc(lesimports,(strlen(lesimports)+strlen(tab[valeur])+2)*sizeof(char));
		    		strcat(lesimports,tab[valeur]);
		    		strcat(lesimports,"|\0");
		    		nbfic++;
		    	}
	    	}

	    	if(lesimports[0]!='\0'){
	    		writeSocketEntier(sock,1);

	    		//Ici on enlève le dernière espace et on remet un indicateur de fin de chaine
		    	lesimports = (char *)realloc(lesimports,strlen(lesimports)*sizeof(char));
		    	lesimports[strlen(lesimports)]='\0';

		    	if((write(sock,lesimports,(strlen(lesimports)+1)*sizeof(char)))==-1){
		    		fprintf(stderr,"Erreur d'écriture vers le père\n");
		    		exit(-1);
		    	}

		    	free(lesimports);
		    	recuperationDesFichiers(sock,nbfic);
	    	}else{
	    		writeSocketEntier(sock,0);
	    		free(lesimports);
	    	}
	    	
	    }


	    //A partir d'ici on libère tout ce qui a pu etre alloc
	    for (int j = 0; j < i; ++j)
			free(tab[j]);
		free(tab);
		free(fichier);
	}
		
}

 /**
 * \fn void recuperationDesFichiers(int sock, int nbfic)
 * \brief S'occupe de la réception des fichiers ainsi que de leur insertion dans notre dossier
 * \param sock, la socket de communication
 * \param nbfic, nombre de fichiers
 */
void recuperationDesFichiers(int sock,int nbfic){

	for (int i = 0; i < nbfic; ++i)
	{
		
		char * nomfic = lectureSocket(sock);

		if(readSocketEntier(sock)){
			char *prefix = NULL;
			prefix = (char *)malloc((strlen("./imageClient/")+strlen(nomfic)+1)*sizeof(char));
			strcpy(prefix,"./imageClient/");
			strcat(prefix,nomfic);

			FILE *out=NULL;
			if((out=fopen(prefix,"wb"))==NULL){
				fprintf(stderr,"Erreur d'ouverture de fichier\n");
				writeSocketEntier(sock,0);
			}else{
				writeSocketEntier(sock,1);
				int taillefichier = readSocketEntier(sock);
				unsigned char c;
				int cmpt =0;
				while(cmpt<taillefichier){
					if(read(sock,&c,sizeof(unsigned char))==-1){
						fprintf(stderr,"Erreur de lecture\n");
					}
					fwrite(&c,sizeof(unsigned char),1,out);
					cmpt++;
				}
				fclose(out);
				printf("Fichier bien reçu\n");
				ouvertureFichier(prefix);
			}
			free(prefix);
		}else{
			fprintf(stderr, "Erreur d'ouverture du fichier sur le serveur\n");
		}
		free(nomfic);
	}

}




/**
 * \fn void ouvertureFichier(char *nom)
 * \brief 
 * \param chaine de character de la position du fichier
 */
void ouvertureFichier(char *nom){
	char c;
	printf("Souaitez vous afficher l'image ? y/n\n");
	scanf("%c",&c);
	if (c=='y')
	{
		pid_t fils;
		switch(fils=fork()){
			case -1:
				fprintf(stderr, "erreur d'ouverture du processeur\n");
				exit(-1);
			case 0:
				close(1);
				close(2);
				execlp("soffice","soffice","-writer",nom,(char *)NULL);
			default:;
		}
		
	}
	//on vide completement stdin au cas ou quelqu'un tape de trop long mot
	while(c!='\n'){
		scanf("%c",&c);
	}

}


 /**
 * \fn void processExport(int sock)
 * \brief Fonction d'exportation de fichier depuis le serveur
 * \param sock, la socket de communication
 */
void processExport(int sock){
	
	DIR *dossier;
	if((dossier = opendir("./imageClient"))==NULL){
		writeSocketEntier(sock,0);
		fprintf(stderr, "Pas d'accès au dossier imageClient\n");
	}else{
		writeSocketEntier(sock,1);
		struct dirent *files; //Initilaisation de la structure pour récupérer les fichiers

		//Boucle de récupération des fichiers concaténés les uns après les autres
		char **liste = NULL; //Notre liste d'éléments
		int nbfich=0;
		printf("\nVoici vos fichiers personnel \n");
		while((files = readdir(dossier))){
			liste = (char **)realloc(liste, (nbfich+1) * sizeof(*liste));
	        liste[nbfich]=(char *)malloc((files->d_reclen+1)*sizeof(char));
	        strcpy(liste[nbfich], files->d_name);       
	        printf("%d. %s\n",nbfich,liste[nbfich]); 
			nbfich++;
		}


		char * lesimports = NULL;
		lesimports = (char *)malloc(sizeof(char));
		lesimports[0]='\0';
		char scan[256];
		int valeur = 0,nbfic=0;
		printf("\nQuel(s) fichier(s) souhaitez vous recupérer ?\n Donnez le ou leurs numéro(s), puis appuyer sur entrée après chaque numéro, pour arreter tapez -1\n");
		while(valeur!=-1){  
			if(fgets(scan,256,stdin)==NULL){
				printf("Merci de ne pas mettre un nombre aussi grand cela me parait suspect\n");
			}		    	
			valeur = atoi(scan);
			//On vérifie que les fichiers existent bel et bien
			if(valeur > 1 && valeur < nbfich){
				lesimports = (char *)realloc(lesimports,(strlen(lesimports)+strlen(liste[valeur])+2)*sizeof(char));
				strcat(lesimports,liste[valeur]);
				strcat(lesimports,"|\0");
				nbfic++;
			}
		}

		if(lesimports[0]!='\0'){
			writeSocketEntier(sock,1);

			//On envoie la liste des fichiers que l'on veut envoyer
			if((write(sock,lesimports,(strlen(lesimports)+1)*sizeof(char)))==-1){
				fprintf(stderr,"Erreur d'ecriture vers le père\n");
	    		exit(-1);
			}
		
			envoieDesFichiers(sock,lesimports);
		}else{
			writeSocketEntier(sock,0);
		}
		for (int i = 0; i < nbfich; ++i)
			free(liste[i]);
		free(liste);
		free(lesimports);
		closedir(dossier);
	} //On ouvre nos dossiers
		
}


 /**
 * \fn void envoieDesFichiers(int sock, char *lesimports)
 * \brief Permet l'importation des fichiers
 * \param sock, la socket de communication
 * \param *lesimports, la liste des nom des fichiers que l'on souhaite importer
 */
void envoieDesFichiers(int sock,char *lesimports){
	char **lesfichiers = NULL;
	int nbfic=0;
	lesfichiers = decoupage(lesimports,&nbfic);

	for (int i = 0; i < nbfic; ++i)
	{
		FILE *in;
		char prefix[300]="./imageClient/";
		strcat(prefix,lesfichiers[i]);

		//On teste si le fichier a été ouvert et on l'indique au serveur
		if((in=fopen(prefix,"rb"))==NULL){
			fprintf(stderr,"Erreur d'ouverture fichier\n");
			writeSocketEntier(sock,0);
		}else{
			writeSocketEntier(sock,1); //write socket 1
			int taillefichier = tailleFichier(prefix);
			writeSocketEntier(sock,taillefichier);

			unsigned char c;
			int ret = fread(&c,sizeof(unsigned char),1,in);
			while(ret){
				if(write(sock,&c,sizeof(c))==-1){
					fprintf(stderr,"Erreur de communication avec le client\n");
					exit(-1);
				}
				ret = fread(&c,sizeof(unsigned char),1,in);		
			}
			fclose(in);
			if(readSocketEntier(sock)){
				printf("Fichier importé\n");
			}else{
				printf("Fichier interdit\n");
			}

		}	

	}

	for (int i = 0; i < nbfic; ++i)
		free(lesfichiers[i]);
	free(lesfichiers);
}


 /**
 * \fn int quitServeur()
 * \brief Signale la sortie du serveur
 * \return 0 dans tous les cas
 */
int quitServeur(){
	fprintf(stdout, "A bientot\n");
	return 0;
}
