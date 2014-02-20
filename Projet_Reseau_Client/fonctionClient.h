/* 
 * File:   fonctionClient.h
 * Author: NNJeremy
 *
 * Created on 6 décembre 2011, 10:13
 */

#ifndef FONCTIONCLIENT_H
#define	FONCTIONCLIENT_H

#ifdef	__cplusplus
extern "C" {
#endif


    //afficher en-tete
    void afficherEnTete();
    
    //lance le menu
    void menu(int sock);
    
    //affiche le resultat de la requete
    void lecteurResultat(int sock);


#ifdef	__cplusplus
}
#endif

#endif	/* FONCTIONCLIENT_H */

