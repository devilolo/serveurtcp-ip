/**
 * \file processusServeur.c
 * \brief Les fonctions du serveur
 * \date 3 décembre 2018
 * \author Loïc Lemaire, Loïc Jovanovic, Robinson Lestani, Quentin Meliard
 */


#include "processusServeur.h"

/**
 * \fn int processusFils(int SocketListener, int SocketFils)
 * \brief 
 * \param SocketListener, le numéro de descripteur fichier de notre socket listener 
 * \return fils
 */

int processusFils(int SocketListener,int Socketfils){
	pid_t fils;
	switch(fils=fork()){
		case -1:
			return -1;
		case 0:
			close(SocketListener);
			fprintf(stderr, "Un client se connecte\n");
			int value=-1,infinie = 1;
			while(infinie){
				value=-1;
				while(value != EXPORT && value != IMPORT && value != CONSULT && value != QUIT){
					value=readSocketEntier(Socketfils);
				}

				//Execution des commandes en fonction de l'indication venue du Serveur
				switch(value){
					case EXPORT:
						processusExport(Socketfils);
						break;
					case IMPORT:
						processusConsult(Socketfils,1);
						break;
					case CONSULT:
						processusConsult(Socketfils,0);
						break;
					case QUIT:
						infinie = 0;
						break;											
				}
			}
			exit(-1);
		default:
			close(Socketfils);
			return fils;
	}
}

 /**
 * \fn void processImport(int sock)
 * \brief Permet l'importation des images
 * \param sock Sock de communication
 *
 * Dans un premier temps elle récupère les fichiers, 
 les transforme sous forme de liste, 
 et pour tous les fichiers de la liste correspondant, 
 créée un nouveau fichier sur le serveur ayant le même nom que celui du client et copie le contenu du fichier du client dans le nouveau fichier du serveur.
 */
void processImport(int sock){


	if(readSocketEntier(sock)){


		char * lesimport = NULL;
		lesimport = lectureSocket(sock);
		char **lesfichiers = NULL;
		int nbfich = 0;



		//On lance le hashage des données
		lesfichiers = decoupage(lesimport,&nbfich);

		//On va envoyer tous les fichiers que l'on nous a demandé 
		for (int i = 0; i < nbfich; ++i)
		{

			if(write(sock,lesfichiers[i],strlen(lesfichiers[i])+1 * sizeof(char))==-1){
				fprintf(stderr,"Erreur de communication avec le client\n");
				exit(-1);
			}

			//On reconcatène avec le dossier de destination
			char *prefix = NULL;
			prefix = (char *)malloc((strlen("./imageServeur/")+strlen(lesfichiers[i])+1)*sizeof(char));
			strcpy(prefix,"./imageServeur/");
			strcat(prefix,lesfichiers[i]);
			//On envoie au client le nom
			

			//On envoie au client le contenu des fichiers
			FILE *in =NULL;
			if((in=fopen(prefix,"rb"))==NULL){
				fprintf(stderr,"Echec d'ouverture du fichier \n");
				writeSocketEntier(sock,0);
			}else{
				writeSocketEntier(sock,1);
				//On vérifie que le fichier destination est ouvrable
				if (readSocketEntier(sock)){
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
				}


				fclose(in);
				free(prefix);
			}
		}

		free(lesimport);
		for (int i = 0; i < nbfich; ++i)
			free(lesfichiers[i]);
		free(lesfichiers);

	}
}


 /**
 * \fn void processusConsult(int sock,int commande)
 * \brief 
 * \param sock Sock dans la quelle on écrit 
 * \param commande 0 pour la consultation, 1 pour l'export
 * \return 0 en cas de succès sinon -1
 *
 * Dans un premier temps si l'utilisateur demande a consulter le répertoire du serveur on affiche le contenu du répertoire imageServeur.
 Dans un second temps si l'utilisateur demande l'export on affiche le contenu du répertoire imageServeur et on procède à l'exportation des fichiers voulus par l'utilisateur
 */
void processusConsult(int sock,int commande){

	DIR *dossier; //On ouvre nos dossiers

	if((dossier  = opendir("./imageServeur"))==NULL){
		writeSocketEntier(sock,0);
	}else{
		writeSocketEntier(sock,1);
		struct dirent *files; //Initilaisation de la structure pour récuperer les fichiers

		char *liste = NULL; //Notre liste d'éléments 
		liste = malloc(sizeof(char)); //On la malloc
		liste[0]='\0';//On met le premier en tant que fin de chaine pour éviter les erreurs de cat
	
		//Boucle de récupération des fichiers concatenés les uns après les autres
		int nbfich=0;
		while((files = readdir(dossier))){
			liste = realloc(liste,(strlen(liste)+files->d_reclen+1)*sizeof(char));
			strcat(liste,files->d_name);
			strcat(liste,"|\0");
			nbfich++;
		}

		//Envoie la chaine créée précedemment au client
		if((write(sock,liste,(strlen(liste)+1)*sizeof(char)))==-1){
			fprintf(stderr,"Erreur d'envoie\n");
			exit(-1);
		}


		//On exécute cette partie uniquement si le client a demandé
		if(commande){
			processImport(sock);
		}

		//On libère tout 
		free(liste);
		closedir(dossier);

	}
		
}

 /**
 * \fn void finProcessusFils()
 * \brief Attend la mort du processus fils
 */
void finProcessusFils(){
	fprintf(stderr, "Un processus fils est terminé\n");
	wait(NULL);
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
 * \fn void processusExport(int sock)
 * \brief Fonction qui gère l'arrivée du fichier depuis le client
 * \param arc nombre de fichiers (limité à deux)
 * \param argv, en premier nom du port (sup à 3000) et ensuite le nom de serveur.
 * \return 0 en cas de succès sinon -1
 */

void processusExport(int sock){	
	
	if(readSocketEntier(sock) && readSocketEntier(sock)){
		char *lesimport = NULL;
		lesimport = lectureSocket(sock);

		int nbfich = 0 ;
		char **listefichier = NULL;
		listefichier = decoupage(lesimport,&nbfich);

		free(lesimport);

		for (int i = 0; i < nbfich; ++i)
		{
			

			if(readSocketEntier(sock)){
				FILE *out=NULL;
				//On utilise getpid pour créer un fichier temporaire unique au processeur
				char tmp[30]="./imageServeur/tmp";
				char str[10];
				itoa(getpid(),str);
				strcat(tmp,str);
				if((out=fopen(tmp,"wb"))==NULL){
					fprintf(stderr,"Erreur d'ouverture de fichier\n");
					exit(-1);
				}
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

				

				if(verifMimeType(tmp)){
					char *prefix = NULL;
					prefix = (char *)malloc((strlen("./imageServeur/")+strlen(listefichier[i])+1)*sizeof(char));
					strcpy(prefix,"./imageServeur/");
					strcat(prefix,listefichier[i]);
					rename(tmp,prefix);
					writeSocketEntier(sock,1);
				}else{
					//suppression du fichier
					if(unlink(tmp)==-1){
						fprintf(stderr,"Impossible de le supprimer\n");
					}
					writeSocketEntier(sock,0);
				}

				fclose(out);
			}
			
		}

		for (int i = 0; i < nbfich; ++i)
			free(listefichier[i]);
		free(listefichier);
	
	}
}

 /**
 * \fn int verifMimeType(char tmp[30])
 * \brief Vérification Des extensions des fichiers
 * \param tmp[30] Chemin du fichier temporaire 
 * \return 1 en cas de succes ou 1 en cas d'echec
 */
int verifMimeType(char tmp[30]){
	int tunnel[3][2];
	int pid1;

	for (int i = 0; i < 3; ++i)
	{
		if(pipe(tunnel[i])!=0){
			fprintf(stderr,"Erreur d'ouverture du pipe\n");
			return 0;
		}
	}
	

	pid_t fils;
	switch(fils=fork()){
		case -1:
			fprintf(stderr,"Erreur du fork\n");
			exit(-1);
		case 0:
			close(1);
			dup(tunnel[1][1]);
			closePipe(tunnel[2]);
			closePipe(tunnel[1]);
			closePipe(tunnel[0]);
			execlp("file","file","-i",tmp,(char *)NULL);
		default:
			switch(pid1 = fork()){
				case -1:
					fprintf(stderr,"Erreur du fork\n");
					exit(-1);
				case 0:
					close(0);
					dup(tunnel[1][0]);
					closePipe(tunnel[1]);
					close(1);
					dup(tunnel[2][1]);
					closePipe(tunnel[2]);
					closePipe(tunnel[0]);
					execlp("grep","grep","-f","doc/MimeTypes.txt",(char *)NULL);
				default:
					closePipe(tunnel[1]);
					switch(pid1=fork()){
						case -1:
							fprintf(stderr,"Erreur du fork\n");
							exit(-1);
						case 0:
							close(0);
							dup(tunnel[2][0]);
							closePipe(tunnel[2]);
							close(1);
							dup(tunnel[0][1]);
							closePipe(tunnel[0]);

							execlp("wc","wc","-l",(char *)NULL);
						default:
							closePipe(tunnel[2]);
							close(tunnel[0][1]);
							char value;
							if(read(tunnel[0][0],&value,sizeof(char))==-1){
								fprintf(stderr,"Erreur\n");
							}
							close(tunnel[0][0]);
							printf("La valeur de fin des processus sont : %c\n",value);
							if(value=='0'){
								return 0;
							}else{
								return 1;
							}
										
					}
			}

	}

	return 0;
}


 /**
 * \fn int closePipe(int tunnel[2])
 * \brief Ferme le pipe passer en argument
 * \param tunnel[2] Pipe 
 * \return 0  
 */
int closePipe(int tunnel[2]){
	close(tunnel[0]);
	close(tunnel[1]);
	return 0;
}






