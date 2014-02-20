/* 
 * File:   fonctionTrain.h
 * Author: remimichel
 *
 * Created on 5 décembre 2011, 13:58
 */

#ifndef FONCTIONTRAIN_H
#define	FONCTIONTRAIN_H

//On définit une structure destinée à recevoir les caractéristiques d'un train
typedef struct Train Train;
struct Train
{
    int numero;
    char villeDepart[50];
    char villeArrivee[50];
    char heureDepart[6];
    char heureArrivee[6];
    float prix;
    char promotion[6];
};

//On définit une structure de liste destinée à recevoir l'ensemble de nos trains
typedef struct element element;
struct element{
    Train train;
    struct element * elementSuivant;
};
typedef element* llist;


//Fonction permettant de construire notre liste de train
llist constructListeTrain(char * nomFichier);

//Fonction permettant de construire un nouvel element train a partir d'une ligne lue dans le fichier
Train recupElementTrain(char * ligneFichier);

//Fonction permettant d'ajouter un élement en fin de liste
llist ajouterFin(llist liste, Train train);

//Fonction permettant d'afficher la liste
void afficherListe(llist liste);

//Retourne le prix après réduction du train
float getPrixApresReduction(Train train);

//Retourne la durée du trajet du train
void getDureeTrajet(char * duree, Train train);

//Retourne vrai ssi l'horaire est dans la tranche horaire du train
int estDansLaTrancheHoraire(char * horaire1, char * horaire2, Train train);

//Retourne le nombre de seconde entre deux temps
int getNombreSeconde(char * horaire1, char * horaire2);

//Retourne une liste de trains en fonction de la ville de départ, de la ville d'arrivée et de l'horaire
llist rechercherTrain1(llist listeTrain, char villeDepart[50], char villeArrivee[50], char heureDepart[6]);

//Retourne une liste de trains en fonction de la ville de départ, de la ville d'arrivée et d'une tranche horaire de départ
llist rechercherTrain2(llist listeTrain, char villeDepart[50], char villeArrivee[50], char * horaire1, char * horaire2);

//Retourne une liste de trains en fonction de la ville de départ et de la ville d'arrivée
llist rechercherTrain3(llist listeTrain, char villeDepart[50], char villeArrivee[50]);

//Retourne le train ayant le meilleur tarif dans une liste de trains
Train rechercheMeilleurTarif(llist listeTrain);

//Retourne le train ayant la durée la plus courte dans une liste de trains
Train rechercheMeilleurDuree(llist listeTrain);


#endif	/* FONCTIONTRAIN_H */

