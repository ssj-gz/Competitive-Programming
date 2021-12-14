#include <iostream>
#include <regex>
#include <vector>
#include <map>
#include <cassert>

using namespace std;

/*
    struct PairInsertion
    {
        string toMatch;
        char toInsert = '\0';
    };
    */

string getInteratedPolymerBruteForce(const string& originalPolymer, const map<string, char>& pairInsertions)
{
    string result;
    const int n = static_cast<int>(originalPolymer.size());
    result += originalPolymer.front();
    for (int i = 0; i + 1 < n; i++)
    {
        const string adjacentPair = originalPolymer.substr(i, 2);

        if (const auto pairIter =  pairInsertions.find(adjacentPair); pairIter != pairInsertions.end() )
        {
            result += pairIter->second;
        }
        result += originalPolymer[i + 1];

    }
    return result;
}

constexpr auto alphabetSize = 26;

map<string, int64_t> getNumSpecialPairs(const string& polymer, const map<string, char>& pairInsertions)
{
    map<string, int64_t> result;
    for (int i = 0; i + 1 < static_cast<int>(polymer.size()); i++)
    {
        const string adjacentPair = polymer.substr(i, 2);
        if (pairInsertions.find(adjacentPair) != pairInsertions.end())
            result[adjacentPair]++;
    }
    return result;
}


array<int64_t, alphabetSize> getLetterHistogram(const string& polymer)
{
    array<int64_t, alphabetSize> result = {};
    for (const auto letter : polymer)
        result[letter - 'A']++;

    return result;
}

struct PolymerExpansionRule
{
    vector<pair<string, int>> specialDeltas;
    char newChar = '\0';
};



std::pair<array<int64_t, alphabetSize>, map<string, int64_t>> getInteratedPolymerInfo(const array<int64_t, alphabetSize>& letterHistogram, const map<string, int64_t>& numSpecialPairs,     const map<string, PolymerExpansionRule>& expansionRuleForPair)
{
    array<int64_t, alphabetSize> nextLetterHistogram = letterHistogram;
    map<string, int64_t> nextNumSpecialPairs = numSpecialPairs;

    for (const auto& [pair, expansionRule] : expansionRuleForPair)
    {
        int64_t numOfPair = 0;
        if (numSpecialPairs.find(pair) != numSpecialPairs.end())
            numOfPair = numSpecialPairs.find(pair)->second;
        nextLetterHistogram[expansionRule.newChar - 'A'] += numOfPair;

        for (const auto& [pairToChange, delta] : expansionRule.specialDeltas)
        {
            nextNumSpecialPairs[pairToChange] += delta * numOfPair;
        }
    }

    return {nextLetterHistogram, nextNumSpecialPairs};
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

    map<string, PolymerExpansionRule> expansionRuleForPair;
    for (const auto& [pair, charToInsert] : pairInsertions)
    {
        PolymerExpansionRule& expansionRule = expansionRuleForPair[pair];
        expansionRule.newChar = charToInsert;
        expansionRule.specialDeltas.push_back({pair, -1});
        const string firstNewPair = string() + pair.front() + charToInsert;
        if (pairInsertions.find(firstNewPair) != pairInsertions.end())
            expansionRule.specialDeltas.push_back({firstNewPair, +1});
        const string secondNewPair = string() + charToInsert + pair.back();
        if (pairInsertions.find(secondNewPair) != pairInsertions.end())
            expansionRule.specialDeltas.push_back({secondNewPair, +1});
    }

    for (const auto& [pair, expansionRule] : expansionRuleForPair)
    {
        cout << "pair: " << pair << " newChar: " << expansionRule.newChar << " special deltas: " << endl;
        for (const auto& specialDelta : expansionRule.specialDeltas)
        {
            cout << " " << specialDelta.first << " " << specialDelta.second << endl;
        }
    }

    string debugPolymer = initialPolymer;
    array<int64_t, alphabetSize> letterHistogram = getLetterHistogram(initialPolymer);
    map<string, int64_t> numSpecialPairs = getNumSpecialPairs(initialPolymer, pairInsertions);


    for (int iter = 1; iter <= 40; iter++)
    {
#ifdef BRUTE_FORCE
        debugPolymer = getInteratedPolymerBruteForce(debugPolymer, pairInsertions);
        cout << "After iteration #" << iter << " : " << debugPolymer << " (len: " << debugPolymer.size() << ")" << endl;
#endif

        const auto& blah = getInteratedPolymerInfo(letterHistogram, numSpecialPairs, expansionRuleForPair);

        letterHistogram = blah.first;
        numSpecialPairs = blah.second;

#ifdef BRUTE_FORCE
        {
            const auto letterHistogramDebug = getLetterHistogram(debugPolymer);
            for (int letterIndex = 0; letterIndex < alphabetSize; letterIndex++)
            {
                cout << "letter: " << static_cast<char>(letterIndex + 'A') << " debug: " << letterHistogramDebug[letterIndex] << " opt: " << letterHistogram[letterIndex] << endl;
                assert(letterHistogramDebug[letterIndex] == letterHistogram[letterIndex]);
            }
        }
#endif
    }

#if 0
    int letterHistogram[26] = {};
    for (const auto letter : polymer)
    {
        letterHistogram[letter - 'A']++;
    }
#endif
    const int64_t numMostCommon = *max_element(begin(letterHistogram), end(letterHistogram));
    int64_t numLeastCommon = numeric_limits<int64_t>::max();
    for (int64_t numOccurences : letterHistogram)
    {
        if (numOccurences > 0)
            numLeastCommon = min(numLeastCommon, numOccurences);
    }

    cout << (numMostCommon - numLeastCommon) << endl;


}
