/* 
 * File:   main.c
 * Author: vaschalr
 *
 * Created on 5 décembre 2011, 10:58
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "fonctionClient.h"

/*
 * 
 */
int main(int argc, char** argv) {
    int sock;
    struct sockaddr_in sin;
        
    //on demande l'addresse et le port du serveur
    afficherEnTete();
    char addr_serveur[15];
    int port_serveur;
    
    //on regarde si on a des param sinon on les demandes
    if(argc > 2) {
        sprintf(addr_serveur, "%s", argv[1]);
        port_serveur = atoi(argv[2]);
    } else {
        printf("Adresse IP du serveur : ");
        gets(addr_serveur);
        printf("Port du serveur : ");
        scanf("%d", & port_serveur);
    }
    
    
    //creation de la socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    
    //configuration de la socket
    sin.sin_addr.s_addr = inet_addr(addr_serveur);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port_serveur);
    
    //le client ce connect au serveur
    if(connect(sock, (struct sockaddr*)&sin, sizeof(sin)) != -1) {
        printf("Connexion à %s sur le port %d\n", inet_ntoa(sin.sin_addr), htons(sin.sin_port));
        menu(sock);
    }
    else
        perror("Impossible de se connecter\n");
    
    /* TEST ENVOI MSG CLIENT -> SERVER OK
    write(sock,"test",4);
     */
    
    //A la fin on ferme la socket
    close(sock);
    
    return (EXIT_SUCCESS);
}