#include <iostream>
#include <vector>
#include <regex>
#include <cassert>

using namespace std;

struct Rule
{
    string fieldName;
    vector<pair<int64_t, int64_t>> validRangesInclusive;
};

void findValidRuleOrder(const vector<Rule>& rules, const vector<vector<int>>& ruleIndexCouldBeFieldIndex, int ruleNumber, vector<int>& unusedRuleIndices, vector<int>& currentRuleIndices, const vector<vector<int>>& validTicketsFieldValues, vector<Rule>& destOrderedRules)
{
    static int largestRuleNumber = -1;
    if (ruleNumber > largestRuleNumber)
    {
        cout << "ruleNumber: " << ruleNumber << " / " << rules.size() << endl;
        largestRuleNumber = ruleNumber;
    }
    if (ruleNumber == rules.size())
    {
        assert(destOrderedRules.empty());
        assert(currentRuleIndices.size() == rules.size());
        for (const auto ruleIndex : currentRuleIndices)
        {
            destOrderedRules.push_back(rules[ruleIndex]);
        }
        cout << "found" << endl;
        return;
    }
    for (int i = 0; i < unusedRuleIndices.size(); i++)
    {
        const int thisRuleIndex = unusedRuleIndices.back();
        const auto& rule = rules[thisRuleIndex];
        unusedRuleIndices.pop_back();

#if 0
        bool ruleMatchesAllFields = true;
        for (const auto& fieldValues : validTicketsFieldValues)
        {
            const auto fieldValue = fieldValues[ruleNumber];
            bool fieldValueIsValid = false;
            for (const auto& validRange : rule.validRangesInclusive)
            {
                if (fieldValue >= validRange.first && fieldValue <= validRange.second)
                    fieldValueIsValid = true;
            }
            if (!fieldValueIsValid)
            {
                ruleMatchesAllFields = false;
            }
        }

        if (ruleMatchesAllFields)
        {
            currentRuleIndices.push_back(thisRuleIndex);
            findValidRuleOrder(rules, ruleIndexCouldBeFieldIndex, ruleNumber + 1, unusedRuleIndices, currentRuleIndices, validTicketsFieldValues, destOrderedRules);
            currentRuleIndices.pop_back();
            assert(ruleIndexCouldBeFieldIndex[thisRuleIndex][ruleNumber]);
        }
        else
        {
            assert(!ruleIndexCouldBeFieldIndex[thisRuleIndex][ruleNumber]);
        }
#endif
        if (ruleIndexCouldBeFieldIndex[thisRuleIndex][ruleNumber])
        {
            currentRuleIndices.push_back(thisRuleIndex);
            findValidRuleOrder(rules, ruleIndexCouldBeFieldIndex, ruleNumber + 1, unusedRuleIndices, currentRuleIndices, validTicketsFieldValues, destOrderedRules);
            currentRuleIndices.pop_back();
            assert(ruleIndexCouldBeFieldIndex[thisRuleIndex][ruleNumber]);
        }

        unusedRuleIndices.insert(unusedRuleIndices.begin(), thisRuleIndex);

    }
}

int main()
{
    string ruleLine;

    vector<Rule> rules;
    while (getline(cin, ruleLine))
    {
        if (ruleLine.empty())
            break;
        static regex ruleRegex(R"(([^:]+):\s*(\d+)-(\d+)\s*or\s*(\d+)-(\d+)\s*)");
        cout << "line: " << ruleLine << endl;
        std::smatch ruleMatch;
        
        if(regex_match(ruleLine, ruleMatch, ruleRegex))
        {
            const auto fieldName = ruleMatch[1];
            const auto rangeBegin1 = stol(ruleMatch[2]);
            const auto rangeEnd1 = stol(ruleMatch[3]);
            const auto rangeBegin2 = stol(ruleMatch[4]);
            const auto rangeEnd2 = stol(ruleMatch[5]);

            rules.push_back({fieldName, { {rangeBegin1, rangeEnd1}, {rangeBegin2, rangeEnd2} }});
        }
        else
        {
            assert(false);
        }
    }

    string line;
    getline(cin, line);
    assert(line == "your ticket:");
    string yourTicket;
    getline(cin, yourTicket);
    istringstream yourTicketStream(yourTicket);
    vector<int> yourTicketFieldValues;
    int yourTicketFieldValue;
    while (yourTicketStream >> yourTicketFieldValue)
    {
        yourTicketFieldValues.push_back(yourTicketFieldValue);
        char dummyChar;
        yourTicketStream >> dummyChar;
        assert(!yourTicketStream || dummyChar == ',');
    }
    assert(yourTicketFieldValues.size() == rules.size());

    getline(cin, line);
    assert(line.empty());
    getline(cin, line);
    assert(line == "nearby tickets:");

    string nearbyTicketLine;
    vector<vector<int>> validTicketsFieldValues;
    while (getline(cin, nearbyTicketLine))
    {
        istringstream nearbyTicketStream(nearbyTicketLine);
        int fieldValue;
        vector<int> fieldValues;
        bool isValidTicket = true;
        while (nearbyTicketStream >> fieldValue)
        {
            fieldValues.push_back(fieldValue);
            bool fieldValueIsValid = false;
            for (const auto& rule : rules)
            {
                for (const auto& validRange : rule.validRangesInclusive)
                {
                    if (fieldValue >= validRange.first && fieldValue <= validRange.second)
                        fieldValueIsValid = true;
                }
            }
            if (!fieldValueIsValid)
                isValidTicket = false;
            char dummyChar;
            nearbyTicketStream >> dummyChar;
            assert(!nearbyTicketStream || dummyChar == ',');
        }
        if (isValidTicket)
        {
            validTicketsFieldValues.push_back(fieldValues);
        }

    }

    vector<Rule> destOrderedRules;
    vector<int> unusedRuleIndices;
    vector<int> currentRuleIndices;
    for (int ruleIndex = 0; ruleIndex < rules.size(); ruleIndex++)
    {
        unusedRuleIndices.push_back(ruleIndex);
    }
    vector<vector<int>> ruleIndexCouldBeFieldIndex(rules.size(), vector<int>(rules.size(), false));
    for (int ruleIndex = 0; ruleIndex < rules.size(); ruleIndex++)
    {
        for (int fieldIndex = 0; fieldIndex < rules.size(); fieldIndex++)
        {
            const auto& rule = rules[ruleIndex];
            bool allFieldValuesMatchRule = true;
            for (const auto& ticketFieldValues : validTicketsFieldValues)
            {
                const auto fieldValue = ticketFieldValues[fieldIndex];
                bool fieldValueIsValid = false;
                for (const auto& validRange : rule.validRangesInclusive)
                {
                    if (fieldValue >= validRange.first && fieldValue <= validRange.second)
                        fieldValueIsValid = true;
                }
                if (!fieldValueIsValid)
                {
                    allFieldValuesMatchRule = false;
                }
            }

            ruleIndexCouldBeFieldIndex[ruleIndex][fieldIndex] = allFieldValuesMatchRule;
        }
    }
    findValidRuleOrder(rules, ruleIndexCouldBeFieldIndex, 0, unusedRuleIndices, currentRuleIndices, validTicketsFieldValues, destOrderedRules);
    assert(!destOrderedRules.empty());
    cout << "fields in order:" << endl;
    int64_t departureValuesProduct = 1;
    for (int ruleIndex = 0; ruleIndex < destOrderedRules.size(); ruleIndex++)
    {
        const auto& rule = destOrderedRules[ruleIndex];
        cout << "field name: " << rule.fieldName << " your ticket field value: " << yourTicketFieldValues[ruleIndex] << endl;
        if (rule.fieldName.find("departure") == 0)
        {
            departureValuesProduct *= yourTicketFieldValues[ruleIndex];
        }
    }
    cout << departureValuesProduct << endl;


#if 0
    vector<int> ruleOrder;
    for (int ruleIndex = 0; ruleIndex < rules.size(); ruleIndex++)
    {
        ruleOrder.push_back(ruleIndex);
    }
    const auto originalRuleOrder = ruleOrder;
    do
    {
        for (const auto& fieldValues : validTicketsFieldValues)
        {
            assert(fieldValues.size() == rules.size());
        }

        next_permutation(ruleOrder.begin(), ruleOrder.end());
    }
    while (ruleOrder != originalRuleOrder);
#endif
}
