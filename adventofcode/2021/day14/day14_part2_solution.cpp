#include <iostream>
#include <regex>
#include <vector>
#include <map>
#include <cassert>

using namespace std;

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
    vector<pair<string, int>> specialPairDeltas;
    char newChar = '\0';
};

std::pair<array<int64_t, alphabetSize>, map<string, int64_t>> getIteratedPolymerInfo(const array<int64_t, alphabetSize>& letterHistogram, const map<string, int64_t>& numSpecialPairs,     const map<string, PolymerExpansionRule>& expansionRuleForPair)
{
    array<int64_t, alphabetSize> nextLetterHistogram = letterHistogram;
    map<string, int64_t> nextNumSpecialPairs = numSpecialPairs;

    for (const auto& [pair, expansionRule] : expansionRuleForPair)
    {
        int64_t numOfPair = 0;
        if (numSpecialPairs.find(pair) != numSpecialPairs.end())
            numOfPair = numSpecialPairs.find(pair)->second;
        nextLetterHistogram[expansionRule.newChar - 'A'] += numOfPair;

        for (const auto& [pairToChange, delta] : expansionRule.specialPairDeltas)
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
    map<string, char> pairInsertions;
    while (getline(cin, pairInsertionLine))
    {
        static regex pairInsertionRegex(R"((\w\w)\s*->\s*(\w)\s*)");
        std::smatch pairInsertionMatch;
        if (regex_match(pairInsertionLine, pairInsertionMatch, pairInsertionRegex))
        {
            pairInsertions[pairInsertionMatch[1]] = string(pairInsertionMatch[2]).front();
        }
    }

    // Build list of "expansion rules": if the polymer contains "pair", how many of which new chars are created,
    // and what are the changes (deltas) to the number of other special pairs?
    map<string, PolymerExpansionRule> expansionRuleForPair;
    for (const auto& [pair, charToInsert] : pairInsertions)
    {
        PolymerExpansionRule& expansionRule = expansionRuleForPair[pair];
        expansionRule.newChar = charToInsert;
        // This pair gets removed.
        expansionRule.specialPairDeltas.push_back({pair, -1});
        // These two new pairs get added.
        const string firstNewPair = string() + pair.front() + charToInsert;
        if (pairInsertions.find(firstNewPair) != pairInsertions.end())
            expansionRule.specialPairDeltas.push_back({firstNewPair, +1});
        const string secondNewPair = string() + charToInsert + pair.back();
        if (pairInsertions.find(secondNewPair) != pairInsertions.end())
            expansionRule.specialPairDeltas.push_back({secondNewPair, +1});
    }

    array<int64_t, alphabetSize> letterHistogram = getLetterHistogram(initialPolymer);
    map<string, int64_t> numSpecialPairs = getNumSpecialPairs(initialPolymer, pairInsertions);

    for (int iter = 1; iter <= 40; iter++)
    {
        const auto& newPolymerInfo = getIteratedPolymerInfo(letterHistogram, numSpecialPairs, expansionRuleForPair);

        letterHistogram = newPolymerInfo.first;
        numSpecialPairs = newPolymerInfo.second;

    }

    const int64_t numMostCommon = *max_element(begin(letterHistogram), end(letterHistogram));
    int64_t numLeastCommon = numeric_limits<int64_t>::max();
    for (int64_t numOccurences : letterHistogram)
    {
        if (numOccurences > 0)
            numLeastCommon = min(numLeastCommon, numOccurences);
    }

    cout << (numMostCommon - numLeastCommon) << endl;


}
