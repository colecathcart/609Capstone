
#include <iostream>
#include "entropycalculator.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Error: must specify a filepath" << endl;
        return -1;
    }
    EntropyCalculator ec;
    cout << "File entropy: " << ec.get_entropy(argv[1]) << endl;
    cout << "Magic Bytes entropy: " << ec.get_magic_bytes_entropy(argv[1]) << endl;
    bool is_random = ec.monobit_test(argv[1]);
    cout << "Monobit result: " << is_random << endl;
}