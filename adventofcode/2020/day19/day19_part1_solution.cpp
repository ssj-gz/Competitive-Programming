#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <cassert>

using namespace std;

struct Rule
{
    bool matchSingleChar = false;
    char charToMatch = '\0';
    int number = -1;

    vector<vector<int>> otherRulesToMatch;
};

vector<vector<string>> getAllWaysToSplitString(const string& stringToSplit, int numPieces)
{
    if (numPieces > stringToSplit.size())
        return {};
    assert(!stringToSplit.empty());
    if (numPieces == 1)
        return {{stringToSplit}};
    vector<int> splitPositions;
    const int stringLen = stringToSplit.size();
    for(int i = 0; i < numPieces - 1; i++)
    {
        splitPositions.push_back(i);
    }
    splitPositions.push_back(stringLen - 1); // Sentinel, to make code slightly less hideous.

    vector<vector<string>> splitStrings;

    while (true)
    {
#if 0
        cout << " Split positions: " << endl;
        for (const auto x : splitPositions)
        {
            cout << x << " ";
        }
        cout << endl;
#endif

#if 1
        int lastSplitPos = 0;
        int blah = 0;
        vector<string> splitString;
        for (const auto x : splitPositions)
        {
            splitString.push_back(stringToSplit.substr(lastSplitPos, x - lastSplitPos + 1));
            lastSplitPos = x + 1;

            blah++;
        }
#if 0
        string blee;
        for(const auto& str : splitString)
        {
            blee += str + "|";
        }
        blee.pop_back();
        cout << blee << endl;
#endif
        splitStrings.push_back(splitString);
#endif
#if 0
        {
            int splitIndex = 0;
            for (int i = 0; i < stringLen; i++)
            {
                cout << stringToSplit[i];
                if (i == splitPositions[splitIndex])
                {
                    cout << " ";
                    splitIndex++;
                }
            }
            cout << endl;
        }
#endif

        int splitIndex = numPieces - 2;
        while (splitIndex >= 0 && splitPositions[splitIndex] == splitPositions[splitIndex + 1] - 1)
        {
            splitIndex--;
        }
        if (splitIndex < 0)
            break;
        splitPositions[splitIndex]++;
        for (int i = splitIndex + 1; i <= numPieces - 2; i++)
        {
            splitPositions[i] = splitPositions[i - 1] + 1;
        }

    }
    
    return splitStrings;
}

map<pair<string, int>, bool> stringMatchesRuleLookup;

bool matchesRule(const Rule& rule, const string& strToMatch, map<int, Rule>& ruleByNumber)
{
    if (stringMatchesRuleLookup.find({strToMatch, rule.number}) != stringMatchesRuleLookup.end())
    {
        return stringMatchesRuleLookup[{strToMatch, rule.number}];
    }
    //cout << "strToMatch: " << strToMatch << " matches rule? " << rule.number << endl;
    if (rule.matchSingleChar)
    {
        return (strToMatch.size() == 1) && (strToMatch[0] == rule.charToMatch);
    }
    bool matchesAny = false;
    for (const auto& ruleConcatsToMatch : rule.otherRulesToMatch)
    {
        const int numPieces = ruleConcatsToMatch.size();
        const auto allWaysToSplitString = getAllWaysToSplitString(strToMatch, numPieces);
        for (const auto& splitString : allWaysToSplitString)
        {
            assert(splitString.size() == ruleConcatsToMatch.size());
            bool matches = true;
            for (int i = 0; i < numPieces; i++)
            {
                if (!matchesRule(ruleByNumber[ruleConcatsToMatch[i]], splitString[i], ruleByNumber))
                {
                    matches = false;
                    break;
                }
            }
            if (matches)
            {
                matchesAny = true;
                break;
            }
        }
    }
    stringMatchesRuleLookup[{strToMatch, rule.number}] = matchesAny;
    return matchesAny;
}

int main()
{
    string ruleLine;
    map<int, Rule> ruleByNumber;
    while (getline(cin, ruleLine))
    {
        cout << "ruleLine: " << ruleLine << endl;
        if (ruleLine.empty())
            break;

        istringstream ruleStream(ruleLine);


        int ruleNumber = -1;
        ruleStream >> ruleNumber;

        Rule& rule = ruleByNumber[ruleNumber];
        rule.number = ruleNumber;

        char dummyChar = '\0';
        ruleStream >> dummyChar;
        assert(dummyChar == ':');

        string definitionNormalised;
        string token;
        while (ruleStream >> token)
        {
            definitionNormalised += token + " ";
        }
        cout << "rule: " <<ruleNumber << " definitionNormalised: " << definitionNormalised << endl;
        if (definitionNormalised.front() == '"')
        {
            rule.matchSingleChar = true;
            rule.charToMatch = definitionNormalised[1];
        }
        else
        {
            definitionNormalised += "|"; // Add sentinel.
            string::size_type startPos = 0;
            while (true)
            {
                const auto nextPipePos = definitionNormalised.find('|', startPos);
                if (nextPipePos == string::npos)
                {
                    break;
                }
                const auto ruleNumbersSubstr = definitionNormalised.substr(startPos, nextPipePos - startPos);
                startPos = nextPipePos + 1;

                vector<int> rulesToMatch;
                istringstream rulesToMatchStream(ruleNumbersSubstr);
                int ruleToMatch;
                while (rulesToMatchStream >> ruleToMatch)
                {
                    rulesToMatch.push_back(ruleToMatch);
                }
                assert(rulesToMatchStream.eof());
                rule.otherRulesToMatch.push_back(rulesToMatch);
            }
        }
        if (rule.matchSingleChar)
        {
            cout << " matches single char: " << rule.charToMatch << endl;
        }
        else
        {
            cout << " matches concatenation of rules: " << endl;
            for (const auto& concatenationOfRules : rule.otherRulesToMatch)
            {
                for (const auto& otherRule : concatenationOfRules)
                {
                    cout << otherRule << " ";
                }
                cout << endl;
            }
        }

    }

    string strToMatch;
    int numMatching = 0;
    const auto& rule0 = ruleByNumber[0];
    while (cin >> strToMatch)
    {
        if (matchesRule(rule0, strToMatch, ruleByNumber))
        {
            numMatching++;
            cout << "string: " << strToMatch << " matches rule 0" << endl;
        }
        else
        {
            cout << "string: " << strToMatch << " DOES NOT match rule 0" << endl;
        }
    }
    cout << numMatching << endl;
}
