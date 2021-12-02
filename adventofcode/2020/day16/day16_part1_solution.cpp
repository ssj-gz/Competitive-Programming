#include <iostream>
#include <vector>
#include <regex>
#include <cassert>

using namespace std;

int main()
{
    string ruleLine;
    struct Rule
    {
        string fieldName;
        vector<pair<int64_t, int64_t>> validRangesInclusive;
    };

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
    getline(cin, line);
    getline(cin, line);
    assert(line.empty());
    getline(cin, line);
    assert(line == "nearby tickets:");

    string nearbyTicketLine;
    int64_t ticketScanningErrorRate = 0;
    while (getline(cin, nearbyTicketLine))
    {
        istringstream nearbyTicketStream(nearbyTicketLine);
        int fieldValue;
        while (nearbyTicketStream >> fieldValue)
        {
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
                ticketScanningErrorRate += fieldValue;
            char dummyChar;
            nearbyTicketStream >> dummyChar;
            assert(!nearbyTicketStream || dummyChar == ',');
        }

    }
    cout << ticketScanningErrorRate << endl;
}
