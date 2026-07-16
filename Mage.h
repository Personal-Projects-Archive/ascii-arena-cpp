#ifndef MAGE_H
#define MAGE_H

#include "Character.h"

/*
 Classe Mage
 Peut lancer un sort pour doubler ses dégâts
*/
class Mage : public Character {
private:
    bool sortActif;

public:
    Mage(std::string n);

    void lancerSort();
    void attaquer(Character& cible) override;
};

#endif