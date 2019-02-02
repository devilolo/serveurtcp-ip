/**
 * \file Write.c
 * \brief Write
 * \date 3 décembre 2018
 * \author Loïc Lemaire, Loïc Jovanovic, Robinson Lestani, Quentin Meliard
 */

#include "Write.h"

 /**
 * \fn char * lectureSocket(int sock)
 * \brief Lecture du contenue d'une socket
 * \param sock Socket dans laquelle on lira son contenu
 * \return 0 fichier Contenue de la socket
 */
char * lectureSocket(int sock){
	int i=0;
	char c;
	char *fichier = NULL;
	fichier = (char *)malloc(sizeof(char));
	int ret;
	while((ret=read(sock,&c,sizeof(char)))>0){
		fichier = realloc(fichier, (i+1) * sizeof(char));
		fichier[i]=c;
		i++;
		if(c=='\0'){
			break;
		}
	}
	fflush(stdout);
	//fichier = realloc(fichier, (i+1) * sizeof(char));
	fichier[i]='\0';
	return fichier;
}

 /**
 * \fn char **decoupage(char *liste,int *i)
 * \brief Découpe la chaine de caractère des differents fichiers séparer par des "|" pour créer une liste de fichier
 * \param liste Chaine de caractère comportant les fichiers séparer par des "|"
 * \param i pointeur determinant le nombre de ligne du tableau / fichier changant a chaque nouveau fichier introduit dans le tableau
 * \return tab Tableau contenant les differents fichiers saisie par l'utilisateur
 */
char **decoupage(char *liste,int *i){
    char **tab = NULL;
    char *token;
    token=strtok(liste,"|");
    while(token != NULL){
    	tab = (char **)realloc(tab, (*i+1) * sizeof(*tab));
        tab[*i]=(char *)malloc((strlen(token)+1)*sizeof(char));
        strcpy(tab[*i], token);       
        printf("%d. %s\n",*i,tab[*i]);   
        token=strtok(NULL,"|");
        *i+=1;
        
    }
    return tab;
}


 /**
 * \fn char *chaineChar()
 * \brief	Skocke dans une chaines caractère les fichiers saisie par l'utilisateur séparer par des "|"
 * \return lesfichiers Liste des fichiers séparer par des "|"
 */
char *chaineChar(){
	char *lesfichiers= NULL;
	lesfichiers = (char *)malloc(sizeof(char));
	char c;
	int cmpt = 0;
	lesfichiers[cmpt]='\0';
	scanf(" %c",&c);
	while(c!='\n'){
		lesfichiers=realloc(lesfichiers,(strlen(lesfichiers)+1) * sizeof(char) );

		if(c==' '){
			lesfichiers[cmpt]='|';
		}else{
			lesfichiers[cmpt]=c;
		}
		scanf("%c",&c);
		cmpt++;
	}
	lesfichiers=realloc(lesfichiers,(strlen(lesfichiers)+1) * sizeof(char) );
	lesfichiers[cmpt]='|';
	lesfichiers[cmpt+1]='\0';
	return lesfichiers;
}

 /**
 * \fn int readSocketEntier(int sock)
 * \brief Lire un entier dans une socket et l'écrit dans une variable
 * \param sock Socket ou l'on ecrira un entier
 * \return L'entier lu dans la socket
 */

int readSocketEntier(int sock){
	int retour = 0;
	if(read(sock,&retour,sizeof(int))==-1){
		printf("Erreur de lecture sur la socket\n");
		exit(-1);
	}
	printf("valeur reçu %d \n", retour );
	retour = ntohl(retour);
	printf("valeur apres transformation %d\n",retour );
	return retour;
}

 /**
 * \fn void writeSocketEntier(int sock,int value)
 * \brief Ecrit un entier dans une socket
 * \param sock Socket ou l'on ecrira
 * \param value Valeur entiere que l'on veut envoyer dans la socket
 */
void writeSocketEntier(int sock,int value){
	int valeur = htonl(value);
	printf("valeur envoyer 1 = %d\n",valeur );
	if(write(sock,&valeur,sizeof(int))==-1){
		printf("Erreur d'ecriture sur la socket\n");
		exit(-1);
	}	
}


 /**
 * \fn int tailleFichier(char *f)
 * \brief Recupere la taille d'un fichier f
 * \param f nom du fichier
 * \return La taille du fichier
 */
int tailleFichier(char *f){
	struct stat filestat;
    if(stat(f,&filestat) < 0){
    	fprintf(stderr, "Erreur avec la fonction stat\n");
    	return 0; 
    }    
    return filestat.st_size;
        
}

 /**
 * \fn char* itoa(int valeur, char* str)
 * \brief Transforme un entier en chaine de caractère (on inverse pas la chaine car inutile)
 * \param valeur Entier que nous voulant convertir
 * \param str Chaine de caractère qui récupera la transformation de l'entier en caractère
 * \return 0 str Chaine de caractère contenant l'entier transformer en caractère
 */
char* itoa(int valeur, char* str){ 
    int i = 0; 
  
    // On gère le cas où on a zero
    if (valeur == 0) 
    { 
        str[i++] = '0'; 
        str[i] = '\0'; 
        return str; 
    } 
  
    //Si on a un nombre négatif on renvoie 0
    if (valeur < 0) 
    { 
        return 0;
    } 
  
    //On change les valeurs une par une
    while (valeur != 0) 
    { 
        int rem = valeur % 10; 
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0'; 
        valeur = valeur/10; 
    }
  
    str[i] = '\0'; //On met le symbole de fin d'une chaine de caractère
  
    return str; 
}


/* 
//lecture d'une ligne ne pas oublier a free la chaine dans la fonction un fois sont utilisation terminé
char ** getNomFichiers(int *value){
	char **fichernom = (char **)malloc(sizeof(char*));
	char c;
	
	scanf(" %c",&c);
	while(c != '\n'){
		int i=0;
		if(*value!=0){
			scanf("%c",&c);
		}
		fichernom[*value]= (char *)malloc(sizeof(char));
		while(c != ' ' && c != '\n'){
			fichernom[*value][i]=c;
			fichernom[*value] = realloc(fichernom[*value], sizeof(c) + sizeof *fichernom[*value]);
			i++;
			scanf("%c",&c);
		}
		fichernom[*value][i]='\0';	
		*value+=1;
		fichernom = realloc(fichernom,(*value) + sizeof *fichernom);	
	}
	return fichernom;
}
*/
