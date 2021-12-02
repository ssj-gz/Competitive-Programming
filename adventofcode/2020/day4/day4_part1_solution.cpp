#include <iostream>
#include <map>
#include <sstream>
#include <regex>
#include <cassert>

using namespace std;

int main()
{
    string line;
    string currentPassport;
    int validPassportCount = 0;
    while (true)
    {
        std::getline(cin, line);
        if (cin && !line.empty())
        {
            currentPassport += " " + line;
        }
        else
        {
            const string requiredFields[] = 
            {
                "byr",
                "iyr",
                "eyr",
                "hgt",
                "hcl",
                "ecl",
                "pid"
            };
            map<string, bool> hasField;
            istringstream passportStream(currentPassport);
            string fieldValuePair;
            while (passportStream >> fieldValuePair)
            {
                const auto colonSeparatorPos = fieldValuePair.find(':');
                assert(colonSeparatorPos != string::npos);
                const string field = fieldValuePair.substr(0, colonSeparatorPos);
                hasField[field] = true;
            }

            bool hasAllRequiredFields = true;
            for (const auto& requiredField : requiredFields)
            {
                if (!hasField[requiredField])
                {
                    hasAllRequiredFields = false;
                    break;
                }
            }
            if (hasAllRequiredFields)
                validPassportCount++;
            currentPassport.clear();
        }
        if (!cin)
            break;

    }
    cout << validPassportCount << endl;
}
