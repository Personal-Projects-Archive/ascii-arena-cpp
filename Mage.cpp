#include "Mage.h"

// Le mage a :
// - 16 points de vie
// - 11 points d'attaque
Mage::Mage(std::string n)
    : Character(n, 16, 11), sortActif(false) {
}

// Le sort coute 3 points de vie
// Il double les degats de la prochaine attaque
void Mage::lancerSort() {
    if (pv > 3) {
        pv -= 3;
        sortActif = true;
    }
}

// Attaque du mage
// Si le sort est actif, les degats sont doubles
void Mage::attaquer(Character& cible) {
    int degats = attaque;

    if (sortActif) {
        degats *= 2;
        sortActif = false;
    }

    cible.recevoirDegats(degats);
}
