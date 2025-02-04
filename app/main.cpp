
#include <iostream>
#include "entropycalculator.h"

using namespace std;

int main(int argc, char* argv[]) {
    EntropyCalculator ec;
    cout << "File entropy: " << ec.get_entropy("test/3point02206entropytest.txt") << endl;
    cout << "File entropy post-encryption: " << ec.get_entropy("test/lorem ipsum.zip") << endl;
}