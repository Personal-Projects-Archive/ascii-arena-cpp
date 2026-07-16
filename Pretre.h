#ifndef PRETRE_H
#define PRETRE_H

#include "Character.h"

/*
 Classe Prêtre
 Peut soigner les personnages
*/
class Pretre : public Character {
public:
    Pretre(std::string n);

    void soigner(Character& cible);
};

#endif