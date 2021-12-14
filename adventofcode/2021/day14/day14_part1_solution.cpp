#include <iostream>
#include <regex>
#include <vector>
#include <map>

using namespace std;

/*
    struct PairInsertion
    {
        string toMatch;
        char toInsert = '\0';
    };
    */

string getInteratedPolymer(const string& originalPolymer, map<string, char>& pairInsertions)
{
    string result;
    const int n = static_cast<int>(originalPolymer.size());
    cout << "originalPolymer: " << originalPolymer << endl;
    result += originalPolymer.front();
    for (int i = 0; i + 1 < n; i++)
    {
        const string adjacentPair = originalPolymer.substr(i, 2);

        cout << "i: " << i << " adjacentPair: " << adjacentPair << endl;

        if (const char charToInsert = pairInsertions[adjacentPair]; charToInsert != '\0')
        {
            cout << " inserting: " << charToInsert << endl;
            result += charToInsert;
        }
        result += originalPolymer[i + 1];

    }
    return result;
}

int main()
{
    string initialPolymer;
    cin >> initialPolymer;

    string pairInsertionLine;
    //vector<PairInsertion> pairInsertions;
    map<string, char> pairInsertions;
    while (getline(cin, pairInsertionLine))
    {
        static regex pairInsertionRegex(R"((\w\w)\s*->\s*(\w)\s*)");
        std::smatch pairInsertionMatch;
        if (regex_match(pairInsertionLine, pairInsertionMatch, pairInsertionRegex))
        {
            //pairInsertions.push_back({toMatch, toInsert});
            pairInsertions[pairInsertionMatch[1]] = string(pairInsertionMatch[2]).front();
        }
    }

    string polymer = initialPolymer;
    for (int iter = 1; iter <= 10; iter++)
    {
        polymer = getInteratedPolymer(polymer, pairInsertions);
        cout << "After iteration #" << iter << " : " << polymer << " (len: " << polymer.size() << ")" << endl;
    }

    int letterHistogram[26] = {};
    for (const auto letter : polymer)
    {
        letterHistogram[letter - 'A']++;
    }
    const int numMostCommon = *max_element(begin(letterHistogram), end(letterHistogram));
    int numLeastCommon = numeric_limits<int>::max();
    for (int numOccurences : letterHistogram)
    {
        if (numOccurences > 0)
            numLeastCommon = min(numLeastCommon, numOccurences);
    }

    cout << (numMostCommon - numLeastCommon) << endl;


}
