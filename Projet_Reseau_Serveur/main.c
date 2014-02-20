/* 
 * File:   main.c
 * Author: vaschalr
 *
 * Created on 5 décembre 2011, 10:55
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include "fonctionTrain.h"

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define PORT 7777
#define MAX_CLIENT 10
#define STX 0x02
#define ETX 0x03
#define EOT 0x04

//La liste des trains construite a partir du fichier
llist listeTrains = NULL;

//Transforme une structure de train en une chaine de caractere formatee
char* trainToString(Train train){
    //Allocation de la memoire pour la chaine de caracteres
    char* strtrain = (char*)malloc(sizeof(train)+5);
    //Concatenation et formalisation de la chaine de caracteres: attribut1#attribut2#att...
    sprintf(strtrain,"%d#%s#%s#%s#%s#%f#%s",train.numero, train.villeDepart, train.villeArrivee, train.heureDepart, train.heureArrivee, train.prix, train.promotion);
    
    return strtrain;
}

int LireTrame(int socket, char* message){
    //Variable servant à lire caractere par caractere dans le buffer de la socket
    char buff;
    //le choix, fait par l'utilisateur, que l'on va retourner
    char choix;
    int i = 0;
    //Tant que l'on ne trouve pas de debut de trame
    do{
        read(socket, &buff, 1);
    }while(buff != (char)STX);
    //On recupere l'identifiant de la demande
    read(socket, &choix, 1);
    //on skip le #
    read(socket, &buff, 1);

    //On recupere le reste de la trame dans message
    //On boucle jusqu'a ce que l'on tombe sur le caractere de fin de trame
    read(socket, &buff, 1);
    while(buff != (char)ETX && buff != NULL){
        message[i] = buff;
        read(socket, &buff, 1);
        i++;
    }
    //On termine la chaine par le caractere de fin
    message[i] = '\0';
    //On retourne le choixx de l'utilisateur sous forme d'entier
    return atoi(& choix);
}

void envoyerTrame(int socket, char* message){
    //On encapsule le message sous la forme <STX>message<ETX>
    char* trame = (char*)malloc(sizeof(message)+2);
    sprintf(trame, "%c%s%c", STX, message, ETX);
    //On envoi la trame
    write(socket, trame, strlen(trame));
}


void envoyerReponse(int socket, llist reponse){
    //On utilise une variable intermediaire pour parcourir la liste
    llist tmp = reponse;
    //chaine de caractere formalisee representant un train
    char* strtrain;
    // Tant que l'on n'est pas au bout de la liste
    while(tmp != NULL){
        //On transforme une structure train en chaine
        strtrain = trainToString(tmp->train);
        //On envoit la chaine
        envoyerTrame(socket, strtrain);
        //On passe a l'element suivant de la liste
        tmp = tmp->elementSuivant;
    }
    //Tous les trains ont été transmits, on envoi le caractere de fin de transmission
    char eot = EOT;
    write(socket, &eot, 1);
}

//Thread client
void * gestionClient(void* arg){
    //Variable servant a stocker les parametres pour les fonctions de recherches
    char* param1;
    char* param2;
    char* param3;
    char* param4;
    //Emulation d'un booleen pour controler le cycle de vie du client
    int boucle = 1;
    //Recuperation de la socket client
    int sock_client = (int)arg;
    //Liste des trains retournés par les fonctions de recherches
    llist listeReponse = NULL;
    //Variable temporaire servant a stocker le train renvoyé par les fonction de recherches meilleur tarif et meilleur duree
    element listetmp;
    //Chaine de caractere representant la demande du client
    char* message = (char*)malloc(sizeof(char)*100);
    while(boucle){
        //On recupere la demande client
        int choix = LireTrame(sock_client, message);
        printf("demande recu\n");
        switch(choix){
            case 0:
                //Le client souhaite se deconnecter
                boucle = 0;
                break;
            case 1:
                //fonction recherche 1, retourne une liste de train
                param1 = strtok(message, "#");
                param2 = strtok(NULL, "#");
                param3 = strtok(NULL, "#");
                listeReponse = rechercherTrain1(listeTrains, param1, param2, param3);
                break;
            case 2:
                //fonction recherche 2, retourne une liste de train
                param1 = strtok(message, "#");
                param2 = strtok(NULL, "#");
                param3 = strtok(NULL, "#");
                param4 = strtok(NULL, "#");
                listeReponse = rechercherTrain2(listeTrains, param1, param2, param3, param4);
                break;
            case 3:
                //fonction recherche 3, retourne une liste de train
                param1 = strtok(message, "#");
                param2 = strtok(NULL, "#");
                listeReponse = rechercherTrain3(listeTrains, param1, param2);
                break;
            case 4:
                //fonction recherche meileur tarif
                //On utilise une liste temporaire pour stocker le train et on réutilise la liste reponse que l'on a fourni à la demande precedante
                //On recherche parmis les reponses precedantes
                listetmp.train = rechercheMeilleurTarif(listeReponse);
                //Liste a un seul element donc le suivant est null
                listetmp.elementSuivant = NULL;
                //On met a jour listeReponse pour l'envoi
                listeReponse = &listetmp;
                break;
            case 5:
                //fonction recherche meilleur duree
                //On utilise une liste temporaire pour stocker le train et on réutilise la liste reponse que l'on a fourni à la demande precedante
                //On recherche parmis les reponses precedantes
                listetmp.train = rechercheMeilleurDuree(listeReponse);
                //Liste a un seul element donc le suivant est null
                listetmp.elementSuivant = NULL;
                //On met a jour listeReponse pour l'envoi
                listeReponse = &listetmp;
                break;
        }
        //Une fois la liste reponse mise a jour, on envoi au client
        envoyerReponse(sock_client, listeReponse);
        printf("reponse envoyee\n");
    }
    printf("Le client s'est deconnecte\n");
    close(sock_client);
    return NULL;
}


/*
 * 
 */
int main(int argc, char** argv) {

    //socket serveur
    int sock_ecoute;
    struct sockaddr_in sin;
    //On initialise le port a partir d'un argument, si pas d'argument valeur par defaut
    int port;
    if(argc > 1)
        port = atoi(argv[1]);
    else
        port = PORT;
    //On initialise la liste des Trains a partir du fichier
    listeTrains = constructListeTrain("./ListeTrain");
    //tableau des threads clients
    pthread_t tab_Threadclient[MAX_CLIENT];
    //socket client
    int sock_client;
    //client courant
    int numclient = 0;
    //On cree la socket serveur
    sock_ecoute = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_ecoute == INVALID_SOCKET) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }
    //On definit les parametres
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    
    if (bind(sock_ecoute, (struct sockaddr*) &sin, sizeof sin) == SOCKET_ERROR) {
        perror("bind()");
        exit(EXIT_FAILURE);
    }
    //On positionne la socket sur ecoute
    if (listen(sock_ecoute, MAX_CLIENT) == SOCKET_ERROR) {
        perror("listen()");
        exit(EXIT_FAILURE);
    }
    //Tant que le max de connection n'est pas atteint
    while (numclient < MAX_CLIENT) {
        struct sockaddr_in csin;
        int sinsize = sizeof csin;
        //on accepte la connection entrante
        sock_client = accept(sock_ecoute, (struct sockaddr*) &csin, &sinsize);
        if (sock_client == INVALID_SOCKET) {
            perror("accept()");
        }
        //lancement du thread client, le thread execute gestionClient avec la socket nouvellement créée en parametre
        pthread_create(&tab_Threadclient[numclient],NULL,gestionClient,(void*)sock_client);
        //On incremente le numero client
        numclient++;
    }
    //On attent la fin de tous les threads client
    for(numclient=0; numclient < MAX_CLIENT; numclient++){
        pthread_join(tab_Threadclient[numclient],NULL);
        printf("thread %d mort\n", numclient);
    }
    //On ferme la socket serveur
    close(sock_ecoute);

    return (EXIT_SUCCESS);
}

