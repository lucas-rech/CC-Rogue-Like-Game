#include "utils.h"

#include <cstdlib>
#include <ctime>

using namespace std;

void inicializarAleatorio() {
    srand(static_cast<unsigned int>(time(nullptr)));
}
