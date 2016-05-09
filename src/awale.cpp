#include <iostream>
#include <string.h>
#include <limits.h>
#include <algorithm>
#include <ctime>

#define SIZE 6
#define T 100

struct Position {
  int plateau[2 * SIZE]; // Le plateau du jeu est mis dans un seul tableau
  int ordi_joue; // boolean vrai si c'est à l'ordi de jouer et faux sinon
  int pions_pris_joueur; // pions pris par le joueur
  int pions_pris_ordi; // pions pris par l'ordi
};

int evaluation(Position * pos, int ordi_joue, int profondeur) {
  /* Fonction d'evaluation de la partie en cours, elle est basique,
  elle compare le nombre de pions pris par l'ordi et par le joueur */
  return pos->pions_pris_ordi - pos->pions_pris_joueur;
}

int positionFinale(Position * pos, int ordi_joue, int profondeur) {
  /* Fonction qui determine si la partie est terminé ou non,
  soit en regardant le nombre de pions pris, soit en regardant si
  on a une situation de famine */
  int somme = 0;
  if (ordi_joue) {
    for (int i = 0; i < SIZE; i++) {
      somme += pos->plateau[i];
    }
  }
  else {
    for (int i = SIZE; i < 2 * SIZE; i++) {
      somme += pos->plateau[i];
    }
  }
  return ((pos->pions_pris_ordi >= (SIZE * 4) + 1) | (pos->pions_pris_joueur >= (SIZE * 4) + 1) | !somme);
}

int coupValide(Position * pos, int ordi_joue, int coup) {
  /* Fonction qui  determine si le coup est valide ou non dans la position pos,
  il suffit que des pions soit dans la case pour que le coup soit valide*/
  if (ordi_joue) {
    return pos->plateau[coup];
  }
  else {
    return pos->plateau[SIZE + coup];
  }
}

int jouerCoup(Position * prochaine, Position * courante, int ordi_joue, int coup) {
  /* Fonction qui joue le coup dans la position courante et met tout a jour dans
  la position prochaine, et la fonction renvoie 1 si le coup est valide (ne
  crée pas de situation de famine) et 0 sinon */

  // Copie de la position courante dans prochaine
  prochaine->pions_pris_ordi = courante->pions_pris_ordi;
  prochaine->pions_pris_joueur = courante->pions_pris_joueur;
  prochaine->ordi_joue = !courante->ordi_joue;
  for (int i = 0; i < 2 * SIZE; i++) {
    prochaine->plateau[i] = courante->plateau[i];
  }

  int cote_ordi = ordi_joue;
  int caillou;
  int pointeur;
  int initial;

  // On regarde combien de pions nous allons distribué
  if (cote_ordi) {
    caillou = prochaine->plateau[coup];
    pointeur = coup;
  }
  else {
    caillou = prochaine->plateau[SIZE + coup];
    pointeur = coup + SIZE;
  }
  initial = pointeur;
  prochaine->plateau[pointeur] = 0;

  // On distribue les pions autour du plateau
  for (int i = 0; i < caillou;) {
    if (pointeur != initial) {
      prochaine->plateau[pointeur] += 1;
      caillou--;
    }
    pointeur++;
    if (pointeur == 2 * SIZE) {
      pointeur = 0;
    }
  }

  // On prends les pions si les conditions sont bonnes
  pointeur--;
  if (ordi_joue & (pointeur >= SIZE)) {
    while ((prochaine->plateau[pointeur] == 3 | prochaine->plateau[pointeur] == 2) & (pointeur >= SIZE)) {
      prochaine->pions_pris_ordi += prochaine->plateau[pointeur];
      prochaine->plateau[pointeur] = 0;
      pointeur--;
    }
  }
  if (!ordi_joue & (pointeur < SIZE)) {
    while ((prochaine->plateau[pointeur] == 3 | prochaine->plateau[pointeur] == 2) & (pointeur < SIZE) & (pointeur >= 0)) {
      prochaine->pions_pris_joueur += prochaine->plateau[pointeur];
      prochaine->plateau[pointeur] = 0;
      pointeur--;
    }
  }

  if (prochaine->pions_pris_ordi > 48) {
    return 1;
  }

  // On regarde que le coup ne crée pas de situation de famine
  int somme = 0;
  if (ordi_joue) {
    for (int i = SIZE; i < 2 * SIZE; i++) {
      somme += prochaine->plateau[i];
    }
  }
  else {
    for (int i = 0; i < SIZE; i++) {
      somme += prochaine->plateau[i];
    }
  }
  return somme;
}

void affichage(Position * courante) {
  /* Fonction qui affiche la position courante*/
  std::cout << std::endl;
  std::cout << "\t\t";
  for (int i = SIZE; i > 0; i--) {
      std::cout << courante->plateau[SIZE + i - 1] << " ";
  }
  std::cout << "\nPions ordi: " << courante->pions_pris_ordi << "\t\t\t\tPions joueur: " << courante->pions_pris_joueur << std::endl;
  std::cout << "\t\t";
  for (int i = 0; i < SIZE; i++) {
      std::cout << courante->plateau[i] << " ";
  }
  int somme = 0;
  for (int i = 0; i < 2 * SIZE; i++) {
    somme += courante->plateau[i];
  }
  std::cout << std::endl << "Pions sur le plateau: " << somme << std::endl;
  std::cout << std::endl << std::endl;
}

int valeurMinMax(Position* pos_courante, int ordi_joue, int prof, int profMax, int alpha, int beta){
  /* Fonction qui evalue tous les coups possible a partir de pos_courante
  jusqu'a une profondeur profMax et retourne le coup qui amene vers le
  meilleur chemin*/

  int alpha2 = alpha;
  int beta2 = beta;
  int tab_valeurs[SIZE];
  if (ordi_joue) {
    for (int i = 0; i < SIZE; i++) {
      tab_valeurs[i] = INT_MIN;
    }
  }
  else {
    for (int i = 0; i < SIZE; i++) {
      tab_valeurs[i] = INT_MAX;
    }
  }
  Position pos_next; // En C on crée dans la pile = TRES rapide

  if (positionFinale(pos_courante, ordi_joue, prof)){
    // Si la partie est fini on met 48 a l'ordi et -48 au joueur
    // le resultat est changer en fonction de la profondeur pour
    // que plus on finit tot plus la partie est bien evalué

    if (pos_courante->pions_pris_ordi > SIZE * 4 + 1) {
      return SIZE * 8 + profMax - prof;
    }
    else {
      if (pos_courante->pions_pris_joueur > SIZE * 4 + 1) {
        return -SIZE * 8;
      }
      else {
        if (pos_courante->pions_pris_ordi == SIZE * 4 & pos_courante->pions_pris_joueur == 4 * SIZE) {
          return 0;
        }
      }
    }
  }

  if (prof == profMax) {
    // Si c'est une feuille on renvoie l'evaluation de la partie courante
    return evaluation(pos_courante, ordi_joue, prof);
  }

  int valeur;
  if (ordi_joue) {
    valeur = INT_MIN;
  }
  else {
    valeur = INT_MAX;
  }
  if (prof == 0) {
    int late = 0;
    int new_prof = profMax;
    double temps = (double)T / SIZE;
    double temps_tot = 0;
    double temps_int = 0;
    for(int i = 0; i < SIZE; i++) {
      // on joue le coup i
      if (coupValide(pos_courante, ordi_joue, i)) {
        // si le coup est valide
        int val;
        val = jouerCoup(&pos_next, pos_courante, ordi_joue, i);
        if (val) {
          // si le coup ne crée pas de famine
          // pos_next devient la position courante, et on change le joueur
          clock_t begin = std::clock();
          tab_valeurs[i] = valeurMinMax(&pos_next, !ordi_joue, prof + 1, new_prof, alpha2, beta2);
          clock_t end = std::clock();
          // On crée un systeme de timeout si une branche prends trop de temps
          // sur la suivante on descend sur une profondeur diminuer de 1
          // et si on a depasser 6 secondes on regarde toutes les branches avec
          // profondeur tres faible
          temps_int = double(end - begin) / CLOCKS_PER_SEC;
          temps_tot += temps_int;
          if ((temps_int >= temps) & (!late)) {
            new_prof -= 1;
          }
          else {
            if ((temps_tot > 5) & (!late)) {
              new_prof = 7;
              late = 1;
            }
          }
          valeur = std::max(valeur, tab_valeurs[i]);
           if (valeur >= beta2) {
             return valeur;
           }
           alpha2 = std::max(alpha2, valeur);
        }
        else {
          // si le coup crée une situation de famine
          if (ordi_joue) {
            tab_valeurs[i] = -200;
          }
          else {
            tab_valeurs[i] = +200;
          }
        }
      }
      else {
        // si le coup n'est pas valide
        if (ordi_joue) {
          tab_valeurs[i] = -200;
        }
        else {
          tab_valeurs[i] = +200;
        }
      }
    }
  }
  else {
    for(int i = 0; i < SIZE; i++) {
      // on joue le coup i
      if (coupValide(pos_courante, ordi_joue, i)) {
        // si le coup est valide
        int val;
        val = jouerCoup(&pos_next, pos_courante, ordi_joue, i);
        if (val) {
          // si le coup ne crée pas de famine
          // pos_next devient la position courante, et on change le joueur
          if (ordi_joue) {
            tab_valeurs[i] = valeurMinMax(&pos_next, !ordi_joue, prof + 1, profMax, alpha2, beta2);
            valeur = std::max(valeur, tab_valeurs[i]);
            if (valeur >= beta2) {
              return valeur;
            }
            alpha2 = std::max(alpha2, valeur);
          }
          else {
            tab_valeurs[i] = valeurMinMax(&pos_next, !ordi_joue, prof + 1, profMax, alpha2, beta2);
            valeur = std::min(valeur, tab_valeurs[i]);
            if (alpha2 >= valeur) {
              return valeur;
            }
            beta2 = std::min(beta2, valeur);
          }
        }
        else {
          // si le coup crée une situation de famine
          if (ordi_joue) {
            tab_valeurs[i] = -200;
          }
          else {
            tab_valeurs[i] = +200;
          }
        }
      }
      else {
        // si le coup n'est pas valide
        if (ordi_joue) {
          tab_valeurs[i] = -200;
        }
        else {
          tab_valeurs[i] = +200;
        }
      }
    }
  }

  if (prof == 0) {
    // On cherche l'indice maximum ou le minimum en fonction de qui joue
    int res_int = 0;
    if (ordi_joue) {
      for (int i = 1; i < SIZE; i++) {
        if (tab_valeurs[i] > tab_valeurs[res_int]) {
          res_int = i;
        }
      }
    }
    else {
      for (int i = 1; i < SIZE; i++) {
        if (tab_valeurs[i] < tab_valeurs[res_int]) {
          res_int = i;
        }
      }
    }
    for (int i = 0; i < SIZE; i++) {
      std::cout  << tab_valeurs[i] << " ";
    }
    std::cout << std::endl;
    return res_int;
  }
  else {
    return valeur;
  }
}

void joueurJoue(Position * courante, Position * suivante) {
  int valide = 0;
  int coup;
  while (!valide) {
    std::cout << std::endl << "A toi de jouer: ";
    std::cin >> coup;
    valide = coupValide(courante, courante->ordi_joue, coup);
  }
  jouerCoup(suivante, courante, courante->ordi_joue, coup);
  affichage(suivante);
}

void ordiJoue(Position * courante, Position * suivante) {
  //TODO: mettre une profondeur variable en focntion du nombre de trou utilisé !!! et !!! du nombre de pions restant
  int profondeur = 11;
  int somme = 0;
  int trous = 0;
  // for (int i = 0; i < 2 * SIZE; i++) {
  //   somme += courante->plateau[i];
  //   trous += courante->plateau[i] > 0;
  // }
  // if (somme < 40) {
  //   profondeur = 14;
  // }
  // else {
  //   if ((somme < 60)) {
  //     profondeur = 13;
  //   }
  //   else {
  //     if (somme < 70) {
  //       profondeur = 13;
  //     }
  //     else {
  //       if ((somme < 90)) {
  //         profondeur = 12;
  //       }
  //       else {
  //         if ((somme < 96) /*& (trous < 14)*/) {
  //           profondeur = 12;
  //         }
  //       }
  //     }
  //   }
  // }



  std::cout << "Profondeur: " << profondeur << std::endl;
  clock_t begin = std::clock();
  int max = valeurMinMax(courante, courante->ordi_joue, 0, profondeur, INT_MIN, INT_MAX);
  clock_t end = std::clock();
  std::cout << "Temps: " << double(end - begin) / CLOCKS_PER_SEC << std::endl;
  std::cout << "==============> L'ordi joue " << max << " <==================" << std::endl;
  jouerCoup(suivante, courante, courante->ordi_joue, max);
  affichage(suivante);
}

int main() {
  Position pos;
  Position new_pos;
  std::cout << "Je joue en premier ? ";
  std::cin >> pos.ordi_joue;
  std::cout << std::endl;
  pos.pions_pris_ordi = 0;
  pos.pions_pris_joueur = 0;
  for (int i = 0; i < 2 * SIZE; i++) {
      pos.plateau[i] = 4;

  }
  if (pos.ordi_joue) {
    while (!positionFinale(&pos, pos.ordi_joue, 0)) {
      ordiJoue(&pos, &new_pos);
      if (positionFinale(&new_pos, new_pos.ordi_joue, 0)) {
        break;
      }
      joueurJoue(&new_pos, &pos);
    }
  }
  else {
    while (!positionFinale(&pos, pos.ordi_joue, 0)) {
      joueurJoue(&pos, &new_pos);
      if (positionFinale(&new_pos, new_pos.ordi_joue, 0)) {
        break;
      }
      ordiJoue(&new_pos, &pos);
    }
  }

  return 0;
}
