#include <iostream>
#include <set>
#include <sstream>

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
        set<string> wordsAlreadyUsed;
        istringstream wordsStream(passphrase);
        string word;
        while (wordsStream >> word)
        {
            if (wordsAlreadyUsed.contains(word))
            {
                valid = false;
                break;
            }
            wordsAlreadyUsed.insert(word);
        }
        if (valid)
            numValid++;
        std::cout << " valid? " << valid << std::endl;
    }
    std::cout << "numValid: " << numValid << std::endl;
    return 0;
}
