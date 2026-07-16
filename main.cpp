#include <cstdlib>
#include <ctime>
#include "Jeu.h"

int main() {
    srand(static_cast<unsigned int>(time(nullptr))); 
    lancerJeu();
    return 0;
}
