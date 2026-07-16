#include "Guerrier.h"

// Le guerrier a :
// - 18 points de vie
// - 8 points d'attaque
Guerrier::Guerrier(std::string n)
    : Character(n, 18, 8) {
}

// Le guerrier reduit les degats recus de 2
void Guerrier::recevoirDegats(int degats) {
    // La reduction ne s'applique qu'aux degats reels (valeurs positives),
    // pas aux soins (valeurs negatives, cf. Pretre::soigner), sinon un soin
    // serait toujours annule par la resistance du Guerrier.
    // On garde toujours au moins 2 degats de passe, sinon un adversaire trop
    // faible ne pourrait jamais le blesser.
    if (degats > 0) {
        degats -= 2;
        if (degats < 2)
            degats = 2;
    }

    pv -= degats;
    if (pv < 0)
        pv = 0;
}
