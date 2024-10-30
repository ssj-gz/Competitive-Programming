#include <iostream>
#include <set>
#include <sstream>
#include <algorithm>

#include <cassert>

using namespace std;

int main()
{
    string passphrase;
    int numValid = 0;
    while (getline(cin, passphrase))
    {
        std::cout << "passphrase: " << passphrase << std::endl;
        bool valid = true;
        set<string> normalisedWordsAlreadyUsed;
        istringstream wordsStream(passphrase);
        string word;
        while (wordsStream >> word)
        {
            string normalisedWord = word;
            sort(normalisedWord.begin(), normalisedWord.end());
            if (normalisedWordsAlreadyUsed.contains(normalisedWord))
            {
                valid = false;
                break;
            }
            normalisedWordsAlreadyUsed.insert(normalisedWord);
        }
        if (valid)
            numValid++;
        std::cout << " valid? " << valid << std::endl;
    }
    std::cout << "numValid: " << numValid << std::endl;
    return 0;
}
