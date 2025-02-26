
#include "analyzer.h"
#include "event.h"
#include "entropy_calculator.h"
#include "file_extension_checker.h"

Analyzer::Analyzer()
    : calculator(EntropyCalculator()), file_checker(FileExtensionChecker())
{
}

void Analyzer::analyze(Event& event)
{

}