# serveurtcp-ip

﻿# README -> Projet Système/Réseau

## Compilation
Afin de compiler notre application il  suffit d’ouvrir un terminal dans le répertoire “Projet_SR”, et d'exécuter la commande make, ce qui va créer les deux exécutables (client et serveur) dans le répertoire ‘bin’.

## Exécution
Voici les étapes nécessaires au lancement de l’application (à réaliser après la compilation) :
* Lancer deux terminaux dans le répertoire Projet_SR.
* Lancer d’abord le serveur, en écrivant dans le premier terminal la commande :
```sh
	$ ./bin/serveur [numeroPort] 
```
*numeroPort est le numéro du port désiré, par exemple 11300*
* Puis, lancer le client, en écrivant dans le deuxième terminal la commande :
```sh
	$ ./bin/client [numeroPort] [adresseServeur] 
```
*numeroPort est le numéro du port désiré, par exemple 11300. L’adresse serveur est l’adresse du serveur lancé*

Si le serveur et le client sont lancés sur la même machine, l’adresse du serveur sera “localhost”. 
En revanche, si l’utilisateur souhaite se connecter de manière distante au serveur, il y a plusieurs étapes à suivre :
* Sur la machine qui accueille le serveur:
	Récupérer l’adresse IP de la machine avec la commande 
```sh
	$ ifconfig -a
```
* Sur la machine qui accueille le client:
Lancer la commande
```sh
	$ nslookup
```
* Dans le menu de nslookup, tapez l’adresse IP précédemment récupérée
