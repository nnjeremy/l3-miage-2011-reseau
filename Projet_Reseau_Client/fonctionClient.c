#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define STX 0x02
#define ETX 0x03
#define EOT 0x04

#include "fonctionClient.h"

void afficherEnTete() {
   
    //on efface l'ecran
    printf("%c[2J", 0x1B);
    
    //on affiche le logo du logiciel
    printf("            .---- -  -\n");
    printf("           (   ,----- - -\n");
    printf("            \\_/      ___\n");
    printf("          c--U---^--'o  [_\n");
    printf("          |------------'_|   \n");
    printf("         /_(o)(o)--(o)(o)\n");
    printf("   ~ ~~~~~~~~~~~~~~~~~~~~~~~~ ~\n\n");
                    
}

void menu(int sock) {
    char ville1[100];
    char ville2[100];
    char tram[100];
    char heure1[100];
    char heure2[100];
    char stx = STX;
    char etx = ETX;
    int choix = 1;
    int cpt = 0;
    
    //on affiche le logo
    afficherEnTete();
    
    //on affiche le menu a l'utilisateur
    while (choix != 0) {
        tram[0] = '\0';
        printf("Menu (0 pour quitter) :\n");
        printf("    -> 1 : Recherche du 1er train disponible\n");
        printf("    -> 2 : Recherche des trains disponibles entre 2 horraires\n");
        printf("    -> 3 : Recherche des trains disponibles pour un trajet\n");
        //si ce n'est pas ca 1er recherche on affiche les autres options
            if (cpt > 0) {
        printf("    -> 4 : Trouver le meilleur tarif sur la derniere recherche\n");
        printf("    -> 5 : Trouver la meilleure duree sur la derniere recherche\n");
            }
        
        scanf("%d", & choix);
        
        //on affiche le logo
        afficherEnTete();
        
        //on lance la bonne action en fonction du choix
            if (choix == 1) {
                printf("Recherche du 1er train disponible : \n");
                printf("-----------------------------------------------------\n");
                //on demande les options a l'utilisateur
                printf("Ville de depart : ");
                        scanf("%s", ville1);
                printf("Ville de d'arrivee : ");
                        scanf("%s", ville2);
                printf("Heure de depart (hh:mm) : ");
                        scanf("%s", heure1);
                        
                        //on construit la chaine
                        sprintf(tram, "%c1#%s#%s#%s%c", 0x02, ville1, ville2, heure1, 0x03);
                                
                        //on envoi la chaine
                        write(sock,tram,strlen(tram));
                             
                        //on affiche le résultat
                        lecteurResultat(sock);
                                
            } else if (choix == 2) {
                printf("Recherche des trains disponibles entre 2 horraires :\n");
                printf("-----------------------------------------------------\n");
                //on demande les options a l'utilisateur
                printf("Ville de depart : ");
                        scanf("%s", ville1);
                printf("Ville de d'arrivee : ");
                        scanf("%s", ville2);
                printf("Heure min (hh:mm) : ");
                        scanf("%s", heure1);
                printf("Heure max (hh:mm) : ");
                        scanf("%s", heure2);
                        
                                //on construit la chaine
                                sprintf(tram, "%c2#%s#%s#%s#%s%c", 0x02, ville1, ville2, heure1, heure2, 0x03);
                                                      
                                //on envoi la chaine
                                write(sock,tram,strlen(tram));
                                
                                //on affiche le résultat
                                lecteurResultat(sock);
                                
            } else if (choix == 3) {
                printf("Recherche des trains disponibles pour un trajet\n");
                printf("-----------------------------------------------------\n");
                //on demande les options a l'utilisateur
                printf("Ville de depart : ");
                        scanf("%s", ville1);
                printf("Ville de d'arrivee : ");
                        scanf("%s", ville2);
                        
                                //on construit la chaine
                                sprintf(tram, "%c3#%s#%s%c", 0x02, ville1, ville2, 0x03);
                                
                                //on envoi la chaine
                                write(sock,tram,strlen(tram));
                                
                                //on affiche le résultat
                                lecteurResultat(sock);
                                
            } else if (choix == 4) {
                
                //on construit la chaine
               sprintf(tram, "%c%c#%c", 0x02, '4', 0x03);
                         
              //on envoi la chaine
               write(sock,tram,strlen(tram));
                
               //on affiche le résultat
                lecteurResultat(sock);
            } else if (choix == 5) {
                
                //on construit la chaine
               sprintf(tram, "%c%c#%c", 0x02, '5', 0x03);
                         
               //on envoi la chaine
               write(sock,tram,strlen(tram));
                
               //on affiche le résultat
                lecteurResultat(sock);
            } else if (choix == 0) {
                
                //on construit la chaine
               sprintf(tram, "%c%c#%c", 0x02, '0', 0x03);
                             
               //on envoi la chaine
               write(sock,tram,strlen(tram));
                
               //on affiche le résultat
                printf("         A BIENTOT");
            }      
        
        //on regarde combien de recherche l'utilisateur a fait
        cpt++;
        
    }
}

void lecteurResultat(int sock) {
    char buffer;
    char message[100];
    char stx = STX;
    int fin = 0;
    
    //on affiche le logo
    afficherEnTete();
    printf("------------------------------------\n");
    printf("LISTE DES TRAINS :\n");
    
    do {
    
        do {
            read(sock,& buffer,1);
        } while(buffer != (char)STX && buffer != (char)EOT && buffer != '\0');
        //on va au 1er caractere du la chaine
        
        //si ce n'est pas un message de fin de transmission on affiche le train
        if (buffer != (char)EOT) {
            
            //On recupere le reste de la trame dans message
            read(sock, &buffer, 1);
            sprintf(message, "%c", buffer);
            while(buffer != (char)ETX && buffer != '\0'){
                read(sock, &buffer, 1);
                sprintf(message, "%s%c", message, buffer);
            }
            
            printf("------------------------------------\n");
            //on utilise strtok pour couper la chaine
            strtok( message, "#");
            printf("Ville de Depart : %s\n", strtok( NULL, "#"));
            printf("Ville D'arrivee : %s\n", strtok( NULL, "#"));
            printf("Heure de Depart : %s\n", strtok( NULL, "#"));
            printf("Heure D'arrivee : %s\n", strtok( NULL, "#"));
            printf("Prix : %s\n", strtok( NULL, "#"));
            printf("------------------------------------\n\n");            
            
        } else
            fin = 1;
    
        //si fin de transmission on arrete d'afficher
    } while (fin != 1);
}