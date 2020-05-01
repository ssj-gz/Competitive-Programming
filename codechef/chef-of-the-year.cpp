// Simon St James (ssjgz) - 2020-05-01
// 
// Solution to: https://www.codechef.com/problems/CVOTE
//
//#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#else
#define _GLIBCXX_DEBUG       // Iterator safety; out-of-bounds access for Containers, etc.
#pragma GCC optimize "trapv" // abort() on (signed) integer overflow.
#define BRUTE_FORCE
#endif
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int numChefs = 1 + rand() % 5;
        const int numEmails = 1 + rand() % 5;
        const int numCountries = 1 + rand() % 5;

        auto randomName = []()
        {
            const int maxLetterIndex = 1 + rand() % 26;
            string name;
            const auto nameLen = 1 + rand() % 10;
            for (int j = 0; j < nameLen; j++)
            {
                if (rand() % 2 == 0)
                {
                    name += 'a' + rand() % maxLetterIndex;
                }
                else
                {
                    name += 'A' + rand() % maxLetterIndex;
                }
            }
            return name;
        };

        vector<string> chefNames;
        for (int i = 0; i < numChefs; i++)
        {
            chefNames.push_back(randomName());
        }
        vector<string> countryNames;
        for (int i = 0; i < numCountries; i++)
        {
            countryNames.push_back(randomName());
        }

        cout << numChefs << " " << numEmails << endl;
        for (int i = 0; i < numChefs; i++)
        {
            cout << chefNames[i] << " " << countryNames[rand() % numCountries] << endl;
        }
        for (int i = 0; i < numEmails; i++)
        {
            cout << chefNames[rand() % numChefs] << endl;
        }


        return 0;
    }
    
    const int numChefs = read<int>();
    const int numEmails = read<int>();

    map<string, string> countryForChef;
    for (int i = 0; i < numChefs; i++)
    {
        const auto chefName = read<string>();
        const auto chefsCountry = read<string>();

        countryForChef[chefName] = chefsCountry;
    }
    
    map<string, int> numVotesForChef;
    map<string, int> numVotesForCountry;

    for (int i = 0; i < numEmails; i++)
    {
        const auto emailSubject = read<string>();
        numVotesForChef[emailSubject]++;
        numVotesForCountry[countryForChef[emailSubject]]++;
    }

    const auto winningCountryIter = max_element(numVotesForCountry.begin(), numVotesForCountry.end(), [](const auto& lhs, const auto& rhs)
            {
                if (lhs.second != rhs.second)
                    return lhs.second < rhs.second;
                return lhs.first > rhs.first;
            });
    const auto winningChefIter = max_element(numVotesForChef.begin(), numVotesForChef.end(), [](const auto& lhs, const auto& rhs)
            {
                if (lhs.second != rhs.second)
                    return lhs.second < rhs.second;
                return lhs.first > rhs.first;
            });

    cout << winningCountryIter->first << endl;
    cout << winningChefIter->first << endl;

    assert(cin);
}
