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
            bool hasInvalidFieldValue = false;
            while (passportStream >> fieldValuePair)
            {
                const auto colonSeparatorPos = fieldValuePair.find(':');
                assert(colonSeparatorPos != string::npos);
                const string field = fieldValuePair.substr(0, colonSeparatorPos);
                hasField[field] = true;
                const string value = fieldValuePair.substr(colonSeparatorPos + 1);

                static regex yearRegex(R"(\d\d\d\d)");
                if (field == "byr" || field == "iyr" || field == "eyr")
                {
                    if (!regex_match(value, yearRegex))
                    {
                        hasInvalidFieldValue = true; 
                    }
                    else
                    {
                        const int year = stoi(value);
                        if (field == "byr")
                        {
                            if (year < 1920 || year > 2002)
                                hasInvalidFieldValue = true;
                        }
                        else if (field == "iyr")
                        {
                            if (year < 2010 || year > 2020)
                                hasInvalidFieldValue = true;
                        }
                        else if (field == "eyr")
                        {
                            if (year < 2020 || year > 2030)
                                hasInvalidFieldValue = true;
                        }
                        else
                            assert(false);
                    }
                }
                else if (field == "hgt")
                {
                    static regex heightRegex(R"((\d+)(cm|in))");
                    std::smatch heightMatch;
                    if (!regex_match(value, heightMatch, heightRegex))
                        hasInvalidFieldValue = true;
                    else
                    {
                        const string unit = heightMatch[2];
                        const auto numUnits = stol(heightMatch[1]);
                        if (unit == "cm")
                        {
                            if (numUnits < 150 || numUnits > 193)
                                hasInvalidFieldValue = true;
                        }
                        else if (unit == "in")
                        {
                            if (numUnits < 59 || numUnits > 76)
                                hasInvalidFieldValue = true;
                        }
                        else
                        {
                            assert(false);
                        }
                    }
                }
                else if (field == "hcl")
                {
                    static regex hairColourRegex(R"(#[a-f0-9]{6})");
                    if (!regex_match(value, hairColourRegex))
                    {
                        hasInvalidFieldValue = true;
                    }
                }
                else if (field == "ecl")
                {
                    static regex eyeColourRegex("(amb|blu|brn|gry|grn|hzl|oth)");
                    if (!regex_match(value, eyeColourRegex))
                    {
                        hasInvalidFieldValue = true;
                    }

                }
                else if (field == "pid")
                {
                    static regex passportIdRegex(R"(\d{9})");
                    if (!regex_match(value, passportIdRegex))
                    {
                        hasInvalidFieldValue = true;
                    }

                }
                else if (field == "cid")
                {
                    // Nothing to do.
                }
                else
                {
                    assert(false);
                }
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
            if (hasAllRequiredFields && !hasInvalidFieldValue)
                validPassportCount++;
            currentPassport.clear();
        }
        if (!cin)
            break;

    }
    cout << validPassportCount << endl;
}
