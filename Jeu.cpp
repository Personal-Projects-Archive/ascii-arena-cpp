#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <chrono>
#include <limits>
#include <cstdlib>
#include "Jeu.h"
#include "Character.h"
#include "Guerrier.h"
#include "Mage.h"
#include "Pretre.h"

// Represente la classe d'un personnage (heros ou ennemi), utile pour savoir
// quelle action speciale proposer et comment convertir le pointeur au bon type
enum class TypeClasse { GUERRIER, MAGE, PRETRE };

// Resultat possible d'un combat contre un ennemi
enum class ResultatCombat { VICTOIRE, DEFAITE, ABANDON };

// Nombre de soins autorises par combat pour un Pretre (joueur ou ennemi),
// pour eviter qu'un combat ne s'eternise indefiniment
const int NB_SOINS_MAX = 3;

// Regroupe le heros du joueur avec ses infos utiles pendant toute la partie
struct Heros {
    std::shared_ptr<Character> perso; // shared_ptr : le heros est detruit automatiquement a la fin
    TypeClasse classe;
    int pvMax;
    int soinsRestants; // nombre de soins encore disponibles ce combat (seul le Pretre en a besoin)
};

// Regroupe un ennemi avec ses infos utiles le temps d'un combat
struct Ennemi {
    Character* perso; // pointeur brut : cree avec new, devra etre libere avec delete
    TypeClasse classe;
    int pvMax;
    int soinsRestants; // nombre de soins encore disponibles (seul le Pretre en a besoin)
};

// Met le programme en pause quelques instants, pour donner un rythme aux actions
void pause(int millisecondes) {
    std::this_thread::sleep_for(std::chrono::milliseconds(millisecondes));
}

// Efface le contenu de la console (commande Windows)
void viderEcran() {
    system("cls");
}

// Lit un choix entre min et max au clavier, redemande tant que la saisie n'est pas valide
int lireChoix(int min, int max) {
    int choix;
    while (true) {
        std::cin >> choix;

        if (std::cin.fail() || choix < min || choix > max) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Choix invalide, reessaie : ";
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return choix;
        }
    }
}

// Affiche une barre de vie du style "Nom [########------] 8/20"
void afficherBarreVie(const std::string& nom, int pv, int pvMax) {
    const int nbCases = 20;
    int rempli = (pv * nbCases) / pvMax;

    if (rempli < 0) rempli = 0;
    if (rempli > nbCases) rempli = nbCases; // securite si jamais un soin depasse le max

    std::cout << nom << " [";
    for (int i = 0; i < nbCases; i++) {
        std::cout << (i < rempli ? '#' : '-');
    }
    std::cout << "] " << pv << "/" << pvMax << "\n";
}

// Empeche un soin de faire depasser le maximum de PV
// (Character ne connait pas son propre maximum, donc on gere ca ici)
void plafonnerPV(Character* perso, int pvMax) {
    int surplus = perso->getPV() - pvMax;
    if (surplus > 0)
        perso->recevoirDegats(surplus);
}

// Ecran de bienvenue
void afficherIntro() {
    viderEcran();
    std::cout << "========================================\n";
    std::cout << "            A S C I I   A R E N A\n";
    std::cout << "========================================\n\n";
    std::cout << "Un mal s'est reveille dans l'Arene.\n";
    std::cout << "Trois vagues d'ennemis se dressent entre toi et la sortie.\n";
    std::cout << "Choisis ton heros et survis... si tu peux.\n\n";
    std::cout << "Appuie sur Entree pour commencer...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Demande le nom et la classe du heros, puis le cree
Heros choisirHeros() {
    std::string nom;
    std::cout << "\nQuel est le nom de ton heros ? ";
    std::getline(std::cin, nom);
    if (nom.empty())
        nom = "Sans-Nom";

    std::cout << "\nChoisis ta classe :\n";
    std::cout << "1. Guerrier - 18 PV, 8 ATQ, resiste aux degats\n";
    std::cout << "2. Mage     - 16 PV, 11 ATQ, peut doubler ses degats avec un sort\n";
    std::cout << "3. Pretre   - 22 PV, 8 ATQ, peut se soigner (3 fois par combat)\n";
    std::cout << "Ton choix : ";
    int choix = lireChoix(1, 3);

    Heros heros;
    heros.soinsRestants = 0;
    switch (choix) {
    case 1:
        heros.perso = std::make_shared<Guerrier>(nom);
        heros.classe = TypeClasse::GUERRIER;
        heros.pvMax = 18;
        break;
    case 2:
        heros.perso = std::make_shared<Mage>(nom);
        heros.classe = TypeClasse::MAGE;
        heros.pvMax = 16;
        break;
    default:
        heros.perso = std::make_shared<Pretre>(nom);
        heros.classe = TypeClasse::PRETRE;
        heros.pvMax = 22;
        break;
    }

    return heros;
}

// Cree l'ennemi correspondant a la vague donnee (0, 1 ou 2)
Ennemi creerEnnemi(int vague) {
    if (vague == 0)
        return { new Guerrier("Gobelin"), TypeClasse::GUERRIER, 18, 0 };
    if (vague == 1)
        return { new Mage("Sorcier des Ombres"), TypeClasse::MAGE, 16, 0 };
    return { new Pretre("Pretre Dechu"), TypeClasse::PRETRE, 22, NB_SOINS_MAX };
}

// Fait jouer un tour a l'ennemi selon une IA simple propre a sa classe
void jouerTourEnnemi(Ennemi& ennemi, Character& joueur) {
    Character* perso = ennemi.perso;

    if (ennemi.classe == TypeClasse::MAGE) {
        Mage* ennemiMage = static_cast<Mage*>(perso);
        if (rand() % 2 == 0) {
            ennemiMage->lancerSort();
            std::cout << perso->getNom() << " se prepare a lancer un sort !\n";
        } else {
            std::cout << perso->getNom() << " attaque " << joueur.getNom() << " !\n";
            perso->attaquer(joueur);
        }
    }
    else if (ennemi.classe == TypeClasse::PRETRE) {
        // Le nombre de soins est limite, sinon le combat pourrait durer tres longtemps
        bool peutSoigner = ennemi.soinsRestants > 0 && perso->getPV() < ennemi.pvMax / 2;
        if (peutSoigner) {
            static_cast<Pretre*>(perso)->soigner(*perso);
            plafonnerPV(perso, ennemi.pvMax);
            ennemi.soinsRestants--;
            std::cout << perso->getNom() << " se soigne ! (" << ennemi.soinsRestants << " soin(s) restant(s))\n";
        } else {
            std::cout << perso->getNom() << " attaque " << joueur.getNom() << " !\n";
            perso->attaquer(joueur);
        }
    }
    else {
        std::cout << perso->getNom() << " attaque " << joueur.getNom() << " !\n";
        perso->attaquer(joueur);
    }
}

// Deroule un combat complet entre le heros et un ennemi, tour par tour
ResultatCombat combattre(Heros& heros, Ennemi& ennemi) {
    while (heros.perso->estVivant() && ennemi.perso->estVivant()) {
        viderEcran();
        std::cout << "--- Combat ---\n\n";
        afficherBarreVie(heros.perso->getNom(), heros.perso->getPV(), heros.pvMax);
        afficherBarreVie(ennemi.perso->getNom(), ennemi.perso->getPV(), ennemi.pvMax);
        std::cout << "\n";

        // Le menu propose une action speciale differente selon la classe du heros
        std::cout << "1. Attaquer\n";
        int optionSpeciale = 0;
        if (heros.classe == TypeClasse::MAGE) {
            std::cout << "2. Lancer un sort (double les degats du prochain coup, coute 3 PV)\n";
            optionSpeciale = 2;
        } else if (heros.classe == TypeClasse::PRETRE && heros.soinsRestants > 0) {
            std::cout << "2. Se soigner (+8 PV, " << heros.soinsRestants << " soin(s) restant(s))\n";
            optionSpeciale = 2;
        }
        int optionAbandon = (optionSpeciale != 0) ? 3 : 2;
        std::cout << optionAbandon << ". Abandonner le combat\n";
        std::cout << "Ton choix : ";

        int choix = lireChoix(1, optionAbandon);

        if (choix == optionAbandon) {
            return ResultatCombat::ABANDON;
        }
        else if (choix == 1) {
            std::cout << "\n" << heros.perso->getNom() << " attaque " << ennemi.perso->getNom() << " !\n";
            heros.perso->attaquer(*ennemi.perso);
        }
        else if (choix == optionSpeciale) {
            if (heros.classe == TypeClasse::MAGE) {
                // On sait avec certitude que heros.perso pointe vers un Mage :
                // static_pointer_cast permet d'appeler lancerSort(), propre a cette classe
                std::static_pointer_cast<Mage>(heros.perso)->lancerSort();
                std::cout << "\n" << heros.perso->getNom() << " se prepare a lancer un sort puissant !\n";
            } else if (heros.classe == TypeClasse::PRETRE) {
                std::static_pointer_cast<Pretre>(heros.perso)->soigner(*heros.perso);
                plafonnerPV(heros.perso.get(), heros.pvMax);
                heros.soinsRestants--;
                std::cout << "\n" << heros.perso->getNom() << " se soigne !\n";
            }
        }

        pause(700);

        if (!ennemi.perso->estVivant())
            break;

        jouerTourEnnemi(ennemi, *heros.perso);
        pause(1000);
    }

    return heros.perso->estVivant() ? ResultatCombat::VICTOIRE : ResultatCombat::DEFAITE;
}

// Affiche l'ecran de fin correspondant au resultat de la partie
void afficherFin(const Heros& heros, ResultatCombat resultat) {
    viderEcran();
    if (resultat == ResultatCombat::VICTOIRE)
        std::cout << "Felicitations " << heros.perso->getNom() << ", tu as survecu aux 3 vagues !\n";
    else if (resultat == ResultatCombat::ABANDON)
        std::cout << heros.perso->getNom() << " a abandonne le combat...\n";
    else
        std::cout << heros.perso->getNom() << " est tombe au combat...\n";
}

void lancerJeu() {
    afficherIntro();

    Heros heros = choisirHeros();
    ResultatCombat resultat = ResultatCombat::VICTOIRE;

    for (int vague = 0; vague < 3; vague++) {
        Ennemi ennemi = creerEnnemi(vague);

        // Le heros se repose completement avant chaque nouvelle vague :
        // chaque combat est un defi independant, pas un cumul de fatigue
        int pvManquants = heros.pvMax - heros.perso->getPV();
        if (pvManquants > 0)
            heros.perso->recevoirDegats(-pvManquants);
        heros.soinsRestants = NB_SOINS_MAX;

        viderEcran();
        std::cout << "--- Vague " << (vague + 1) << " / 3 ---\n";
        std::cout << heros.perso->getNom() << " est repose et pret au combat.\n";
        std::cout << ennemi.perso->getNom() << " apparait !\n";
        pause(1200);

        resultat = combattre(heros, ennemi);

        if (resultat == ResultatCombat::VICTOIRE) {
            viderEcran();
            std::cout << ennemi.perso->getNom() << " est vaincu !\n";
            pause(1200);
        }

        delete ennemi.perso; // liberation manuelle : l'ennemi a ete cree avec new
        ennemi.perso = nullptr;

        if (resultat != ResultatCombat::VICTOIRE)
            break;
    }

    afficherFin(heros, resultat);
    // heros.perso est un shared_ptr : il sera automatiquement detruit ici, pas besoin de delete

    std::cout << "\nRejouer ? (o/n) : ";
    char rejouer;
    std::cin >> rejouer;
    if (rejouer == 'o' || rejouer == 'O') {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        lancerJeu();
    }
}
