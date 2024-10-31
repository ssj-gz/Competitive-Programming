#include <iostream>
#include <regex>

#include <cassert>

using namespace std;

int main()
{
    std::regex generatorStartRegex(R"(^Generator [AB] starts with (\d+)$)");


    string generatorAStartDesc;
    std::getline(cin, generatorAStartDesc);
    std::smatch generatorAStartMatch;
    const bool generatorAMatched = std::regex_match(generatorAStartDesc, generatorAStartMatch, generatorStartRegex);
    assert(generatorAMatched);
    const int64_t generatorAStartVal = std::stoll(generatorAStartMatch[1]);

    string generatorBStartDesc;
    std::getline(cin, generatorBStartDesc);
    std::smatch generatorBStartMatch;
    const bool generatorBMatched = std::regex_match(generatorBStartDesc, generatorBStartMatch, generatorStartRegex);
    assert(generatorBMatched);
    const int64_t generatorBStartVal = std::stoll(generatorBStartMatch[1]);

    std::cout << "generatorAStartVal: " << generatorAStartVal << " generatorBStartVal: " << generatorBStartVal << std::endl;

    int64_t generatorA = generatorAStartVal;
    int64_t generatorB = generatorBStartVal;
    constexpr int64_t generatorAFactor = 16'807;
    constexpr int64_t generatorBFactor = 48'271;
    int64_t numMatches = 0;
    for (int64_t count = 1; count <= 5'000'000; count++)
    {
        do
        {
            generatorA = (generatorA * generatorAFactor) % 2'147'483'647;
        } while (generatorA % 4 != 0);
        do
        {
            generatorB = (generatorB * generatorBFactor) % 2'147'483'647;
        } while (generatorB % 8 != 0);
        if ((generatorA & 0xFFFF) == (generatorB & 0xFFFF))
        {
            numMatches++;
        }

    }
    std::cout << "numMatches: " << numMatches << std::endl;

    return 0;
}
