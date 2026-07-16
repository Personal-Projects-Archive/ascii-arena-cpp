#ifndef GUERRIER_H
#define GUERRIER_H

#include "Character.h"

/*
 Classe Guerrier
 Hérite de Character
*/
class Guerrier : public Character {
public:
    Guerrier(std::string n);

    // Le guerrier est résistant
    void recevoirDegats(int degats) override;
};

#endif