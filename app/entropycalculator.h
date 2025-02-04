#ifndef ENTROPYCALCULATOR
#define ENTROPYCALCULATOR

#include <string>

using namespace std;

class EntropyCalculator
{
    public:
        EntropyCalculator();

        double get_entropy(const string& filepath) const;

        bool is_encrypted(double e1, double e2) const;
    
    private:
        
};

#endif