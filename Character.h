#ifndef CHARACTER_H
#define CHARACTER_H

#include <string>

// Classe de base : tous les personnages du jeu heritent de Character
class Character {
protected:
    std::string nom;   // Nom du personnage
    int pv;            // Points de vie
    int attaque;       // Attaque de base

public:
    // Constructeur
    Character(std::string n, int pv, int atk);

    // Destructeur virtuel : obligatoire des qu'une classe est destinee a etre heritee
    virtual ~Character() = default;

    // Methodes communes (virtual = chaque classe fille peut redefinir son propre comportement)
    virtual void attaquer(Character& cible);
    virtual void recevoirDegats(int degats);

    // Methodes utilitaires
    bool estVivant() const;
    int getPV() const;
    std::string getNom() const;
};

#endif
