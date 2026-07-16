#include "Character.h"

// Initialise le nom, les PV et l'attaque
Character::Character(std::string n, int pv, int atk)
    : nom(n), pv(pv), attaque(atk) {
}

// Attaque un autre personnage avec les degats de base
void Character::attaquer(Character& cible) {
    cible.recevoirDegats(attaque);
}

// Reception de degats (une valeur negative sert a soigner)
void Character::recevoirDegats(int degats) {
    pv -= degats;
    if (pv < 0)
        pv = 0;
}

// Vrai si le personnage a encore des PV
bool Character::estVivant() const {
    return pv > 0;
}

int Character::getPV() const {
    return pv;
}

std::string Character::getNom() const {
    return nom;
}
