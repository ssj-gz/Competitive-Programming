// Simon St James (ssjgz) - 2020-05-01
// 
// Solution to: https://www.codechef.com/problems/CVOTE
//
#include <iostream>
#include <map>
#include <algorithm>

#include <cassert>

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

int main()
{
    ios::sync_with_stdio(false);
    
    const auto numChefs = read<int>();
    const auto numEmails = read<int>();

    map<string, string> countryForChef;
    for (int i = 0; i < numChefs; i++)
    {
        const auto chefName = read<string>();
        const auto chefsCountry = read<string>();

        countryForChef[chefName] = chefsCountry;
    }
    
    map<string, int> numVotesForChef;
    map<string, int> numVotesForCountry;

    for (auto i = 0; i < numEmails; i++)
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
