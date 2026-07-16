#include "Pretre.h"

// Le pretre a :
// - 22 points de vie
// - 8 points d'attaque
Pretre::Pretre(std::string n)
    : Character(n, 22, 8) {
}

// Le pretre soigne un personnage de 8 PV
// (on ajoute des PV en envoyant des degats negatifs)
void Pretre::soigner(Character& cible) {
    cible.recevoirDegats(-8);
}
