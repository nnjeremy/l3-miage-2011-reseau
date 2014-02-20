#include <stdio.h>
#include <stdlib.h>
#include "fonctionTrain.h"
#include <string.h>
#define TAILLE_LIGNE 1000
#define REMISE 0.20
#define SUPPLEMENT 0.10

llist constructListeTrain(char * nomFichier) {
    //On commence par ouvrir notre fichier
    FILE * fichier = NULL; //Notre fichier contenant les infos sur les trains
    char ligneFichier[TAILLE_LIGNE]; //Variable destinée à recevoir une ligne du fichier
    fichier = fopen(nomFichier, "r"); //On ouvre notre fichier en mode lecture
    llist listeTrain = NULL;
    if (fichier != NULL) {
        while (fgets(ligneFichier, TAILLE_LIGNE, fichier) != NULL) {
            //On parcours tout notre fichier et on va construire notre liste de train
            Train train = recupElementTrain(ligneFichier);
            listeTrain = ajouterFin(listeTrain, train);
        }
        fclose(fichier);
    } else {
        //Si l'ouverture du fichier à échoué - errno positionné
        perror("Erreur lors de l'ouverture du fichier");
    }
    return listeTrain;
}

Train recupElementTrain(char * ligneFichier) {
    char * p;
    Train train;
    p = strtok(ligneFichier, "#");
    //On a récupérer le numéro du train
    train.numero = atoi(p);
    p = strtok(NULL, "#");
    //On a récupérer la ville de départ
    strcpy(train.villeDepart, p);
    p = strtok(NULL, "#");
    //On a récupérer la ville d'arrivée
    strcpy(train.villeArrivee, p);
    p = strtok(NULL, "#");
    //On a récupérer l'heure de départ
    strcpy(train.heureDepart, p);
    p = strtok(NULL, "#");
    //On a récupérer l'heure d'arrivée
    strcpy(train.heureArrivee, p);
    p = strtok(NULL, "#");
    //On a récupérer le prix
    train.prix = atof(p);
    p = strtok(NULL, "#");
    //On a récupérer la promotion 
    strcpy(train.promotion, p);
    return train;
}

llist ajouterFin(llist liste, Train train) {
    //On crée un nouvel élement
    element* nouvelElement = malloc(sizeof (element));
    //On assigne la valeur au nouvel élement
    nouvelElement->train = train;
    //On l'ajoute en fin de liste
    nouvelElement->elementSuivant = NULL;
    if (liste == NULL) {

        /* Si la liste est vide il suffit de renvoyer l'élément créé */
        return nouvelElement;
    } else {
        /* Sinon, on parcourt la liste à l'aide d'un pointeur temporaire et on
        indique que le dernier élément de la liste est relié au nouvel élément */
        element* temp = liste;
        while (temp->elementSuivant != NULL) {
            temp = temp->elementSuivant;
        }
        temp->elementSuivant = nouvelElement;
        return liste;
    }

}

void afficherListe(llist liste) {
    element *tmp = liste;
    char duree[6];
    /* Tant que l'on n'est pas au bout de la liste */
    while (tmp != NULL) {
        getDureeTrajet(duree, tmp->train);
        printf("\n");
        /* On affiche */
        printf("%d ", tmp->train.numero);
        printf("%s ", tmp->train.villeDepart);
        printf("%s ", tmp->train.villeArrivee);
        printf("%s ", tmp->train.heureDepart);
        printf("%s ", tmp->train.heureArrivee);
        printf("%.2f ", tmp->train.prix);
        printf("%s ", tmp->train.promotion);
        printf("--> Prix final: %.2f\n", getPrixApresReduction(tmp->train));
        printf("--> Duree: %s\n", duree);
        tmp = tmp->elementSuivant;
    }
}

int getNombreSeconde(char * horaire1, char * horaire2) {
    //On commence par allouer de la mémoire destinée à recevoir nos horaires
    char * heure1 = malloc(2 * sizeof (char));
    char * heure2 = malloc(2 * sizeof (char));
    char * minute1 = malloc(2 * sizeof (char));
    char * minute2 = malloc(2 * sizeof (char));
    //On formate correctement nos horaires en sachant que horaire[0] et horaire[1] contiennent les heures
    // horaire[3] et horaire[4] contiennent nos minutes
    sprintf(heure1, "%c%c", horaire1[0], horaire1[1]);
    sprintf(minute1, "%c%c", horaire1[3], horaire1[4]);
    sprintf(heure2, "%c%c", horaire2[0], horaire2[1]);
    sprintf(minute2, "%c%c", horaire2[3], horaire2[4]);
    //On calcule le nombre de secondes
    int nbSec = (atoi(heure2)*3600 + atoi(minute2)*60)-(atoi(heure1)*3600 + atoi(minute1)*60);
    //On libère la mémoire allouée
    free(heure1);
    free(minute1);
    free(heure2);
    free(minute2);
    //On retourne le nombre de secondes
    return nbSec;
}

llist rechercherTrain1(llist listeTrain, char villeDepart[50], char villeArrivee[50], char heureDepart[6]) {
    //Retourne une liste de trains en fonction de la ville de départ, de la ville d'arrivée et de l'horaire
    llist nouvelleListe = NULL;
    element * p = listeTrain; //On sauvegarde la tête de liste
    Train train = p->train;
    int nbSecMin = 100000000;
    while (p != NULL) {
        //Si un train correspond à nos critère, on l'ajoute dans la liste
        if ((strcmp(p->train.villeDepart, villeDepart) == 0) &&
                (strcmp(p->train.villeArrivee, villeArrivee) == 0) &&
                (strcmp(p->train.heureDepart, heureDepart) == 0)) {
            nouvelleListe = ajouterFin(nouvelleListe, p->train);
        }
        if ((strcmp(p->train.villeDepart, villeDepart) == 0) &&
                (strcmp(p->train.villeArrivee, villeArrivee) == 0) && getNombreSeconde(heureDepart, p->train.heureDepart) >= 0 && getNombreSeconde(heureDepart, p->train.heureDepart) < nbSecMin) {
            nbSecMin = getNombreSeconde(heureDepart, p->train.heureDepart);
            train = p->train;
        }
        p = p->elementSuivant; //On passe à l'élément suivant
    }
    if (nouvelleListe == NULL) {
        nouvelleListe = ajouterFin(nouvelleListe, train);
    }

    return nouvelleListe;
}

llist rechercherTrain2(llist listeTrain, char villeDepart[50], char villeArrivee[50], char * horaire1, char * horaire2) {
    //Retourne une liste de trains en fonction de la ville de départ, de la ville d'arrivée et d'une tranche horaire de départ
    element * p = listeTrain;
    llist nouvelleListe = NULL;
    while (p != NULL) {
        if (strcmp(villeDepart, p->train.villeDepart) == 0 &&
                strcmp(villeArrivee, p->train.villeArrivee) == 0 &&
                estDansLaTrancheHoraire(horaire1, horaire2, p->train)){
            nouvelleListe = ajouterFin(nouvelleListe, p->train);
        }
        p = p->elementSuivant;
    }
    return nouvelleListe;
}

llist rechercherTrain3(llist listeTrain, char villeDepart[50], char villeArrivee[50]) {
    //Retourne une liste de trains en fonction de la ville de départ et de la ville d'arrivée
    element * p = listeTrain;
    llist nouvelleListe = NULL;
    while (p != NULL) {
        if (strcmp(villeDepart, p->train.villeDepart) == 0 &&
                strcmp(villeArrivee, p->train.villeArrivee) == 0)
            nouvelleListe = ajouterFin(nouvelleListe, p->train);
        p = p->elementSuivant;
    }
    return nouvelleListe;
}

float getPrixApresReduction(Train train) {
    if (strcmp(train.promotion, "REDUC") == 0)
        return (1 - REMISE) * train.prix;
    else if (strcmp(train.promotion, "SUPPL") == 0)
        return (1 + SUPPLEMENT) * train.prix;
    else return train.prix;
}

void getDureeTrajet(char * duree, Train train) {
    //On alloue un espace mémoire pour chaque variable pour notre calcul de durée
    char * heureDepart = malloc(2 * sizeof (char));
    char * heureArrivee = malloc(2 * sizeof (char));
    char * minuteDepart = malloc(2 * sizeof (char));
    char * minuteArrivee = malloc(2 * sizeof (char));
    int resultatHeure, resultatMinute;
    
    //On formate correctement nos variables
    sprintf(heureDepart, "%c%c", train.heureDepart[0], train.heureDepart[1]);
    sprintf(minuteDepart, "%c%c", train.heureDepart[3], train.heureDepart[4]);
    sprintf(heureArrivee, "%c%c", train.heureArrivee[0], train.heureArrivee[1]);
    sprintf(minuteArrivee, "%c%c", train.heureArrivee[3], train.heureArrivee[4]);

    //On calcule notre durée
    if (atoi(minuteArrivee) >= atoi(minuteDepart)) {
        resultatHeure = atoi(heureArrivee) - atoi(heureDepart);
        resultatMinute = atoi(minuteArrivee) - atoi(minuteDepart);
    } else {
        resultatHeure = (atoi(heureArrivee) - 1)-(atoi(heureDepart));
        resultatMinute = (atoi(minuteArrivee) + 60) - atoi(minuteDepart);
    }

    //On reformate le résultat de notre calcul dans la varaibles durée
    if (resultatHeure < 10 && resultatMinute < 10) {
        sprintf(duree, "0%dh0%d", resultatHeure, resultatMinute);
    } else if (resultatHeure >= 10 && resultatMinute < 10)
        sprintf(duree, "%dh0%d", resultatHeure, resultatMinute);
    else if (resultatHeure < 10 && resultatMinute >= 10)
        sprintf(duree, "0%dh%d", resultatHeure, resultatMinute);
    else
        sprintf(duree, "%dh%d", resultatHeure, resultatMinute);
    //On libère la mémoire allouée
    free(heureDepart);
    free(minuteDepart);
    free(heureArrivee);
    free(minuteArrivee);
}

int estDansLaTrancheHoraire(char * horaire1, char * horaire2, Train train) {
    //On alloue la mémoire pour nos différentes variables qui seront destinées aux différents calculs
    char * heureDepart = malloc(2 * sizeof (char));
    char * minuteDepart = malloc(2 * sizeof (char));
    char * heureHoraire1 = malloc(2 * sizeof (char));
    char * heureHoraire2 = malloc(2 * sizeof (char));
    char * minuteHoraire1 = malloc(2 * sizeof (char));
    char * minuteHoraire2 = malloc(2 * sizeof (char));

    //On range correctement les valeurs dans nos variables
    sprintf(heureDepart, "%c%c", train.heureDepart[0], train.heureDepart[1]);
    sprintf(minuteDepart, "%c%c", train.heureDepart[3], train.heureDepart[4]);
    sprintf(heureHoraire1, "%c%c", horaire1[0], horaire1[1]);
    sprintf(minuteHoraire1, "%c%c", horaire1[3], horaire1[4]);
    sprintf(heureHoraire2, "%c%c", horaire2[0], horaire2[1]);
    sprintf(minuteHoraire2, "%c%c", horaire2[3], horaire2[4]);
    
    //On ramène toutes les valeurs en secondes pour pouvoir mieux les comparer
    int nbSecHoraire1 = atoi(heureHoraire1)*3600 + atoi(minuteHoraire1)*60;
    int nbSecHoraire2 = atoi(heureHoraire2)*3600 + atoi(minuteHoraire2)*60;
    int nbSecDepart = atoi(heureDepart)*3600 + atoi(minuteDepart)*60;
    //On peut libérer la mémoire allouée
    free(heureDepart);
    free(minuteDepart);
    free(heureHoraire1);
    free(minuteHoraire1);
    free(heureHoraire2);
    free(minuteHoraire2);
    //On vérifie si l'heure de départ du train se situe bien dans la tranche horaire
    if(nbSecDepart>=nbSecHoraire1 && nbSecDepart<= nbSecHoraire2)
        return 1;
    else
        return 0;
}

Train rechercheMeilleurTarif(llist listeTrain) {
    Train train;
    float prixMin = 1000000.0; //On initialise avec une valeur très grande
    element * p = listeTrain;
    while (p != NULL) {
        if (getPrixApresReduction(p->train) < prixMin) {
            train = p->train;
            prixMin = getPrixApresReduction(p->train);
        }
        p = p->elementSuivant;
    }
    return train;
}

Train rechercheMeilleurDuree(llist listeTrain) {
    Train train;
    //On alloue de la mémoire pour nos variables 
    char * heureDureeTrain = malloc(2 * sizeof (char));
    char * heureDureeMin = malloc(2 * sizeof (char));
    char * minuteDureeTrain = malloc(2 * sizeof (char));
    char * minuteDureeMin = malloc(2 * sizeof (char));
    char * dureeTrain = malloc(6 * sizeof (char));
    char dureeMin[] = "23h59"; //On initialise avec une valeur très grande
    int nbSecondeDureeMin, nbSecondeDureeTrain;
    element * p = listeTrain;
    while (p != NULL) {//On parcours la liste élément par élément
        getDureeTrajet(dureeTrain, p->train); //On récupère la durée du trajet de l'élément courant
        //On range cette durée dans nos différentes variables
        sprintf(heureDureeTrain, "%c%c", dureeTrain[0], dureeTrain[1]);
        sprintf(heureDureeMin, "%c%c", dureeMin[0], dureeMin[1]);
        sprintf(minuteDureeMin, "%c%c", dureeMin[3], dureeMin[4]);
        sprintf(minuteDureeTrain, "%c%c", dureeTrain[3], dureeTrain[4]);
        //On convertit tout en seconde pour pouvoir mieux faire nos comparaisons
        nbSecondeDureeMin = (atoi(heureDureeMin)*3600)+(atoi(minuteDureeMin)*60);
        nbSecondeDureeTrain = (atoi(heureDureeTrain)*3600)+(atoi(minuteDureeTrain)*60);
        //Si la durée trouvée est plus petite que précedemment
        if (nbSecondeDureeTrain < nbSecondeDureeMin) {
            train = p->train; //On sauvegarde ce train
            getDureeTrajet(dureeMin, p->train); //On sauvegarde la durée
        }
        p = p->elementSuivant; //On passe à l'élément suivant
    }
    //On oublie pas de libérer la mémoire précedemment allouée
    free(dureeTrain);
    free(heureDureeTrain);
    free(heureDureeMin);
    free(minuteDureeTrain);
    free(minuteDureeMin);
    //On retourne notre train
    return train;
}
