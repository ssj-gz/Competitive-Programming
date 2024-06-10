#include <iostream>
#include <map>
#include <regex>
#include <limits>
#include <cassert>

#define _GLIBCXX_DEBUG       // Iterator safety; out-of-bounds access for Containers, etc.
#pragma GCC optimize "trapv" // abort() on (signed) integer overflow.

using namespace std;

struct MonkeyYell
{
    string monkeyName;
    enum class Type { Number, Operation };
    Type yellType;

    int64_t numberToYell = std::numeric_limits<int64_t>::max(); // For "Number" yellType only.

    string monkeyOp1Name; // For "Operation" yellType only.
    string monkeyOp2Name; // For "Operation" yellType only.
    char operation = '\0';
};

int64_t findYellForMonkey(const std::string& monkeyName, std::map<string, MonkeyYell>& yellInfoForMonkey)
{
    assert(yellInfoForMonkey.contains(monkeyName));
    const MonkeyYell& yellForMonkey = yellInfoForMonkey[monkeyName];
    if (yellForMonkey.yellType == MonkeyYell::Type::Number)
    {
        assert(yellForMonkey.numberToYell != std::numeric_limits<int64_t>::max());
        return yellForMonkey.numberToYell;
    }
    else
    {
        assert(yellForMonkey.yellType == MonkeyYell::Type::Operation);
        const int64_t yellForOp1Monkey = findYellForMonkey(yellForMonkey.monkeyOp1Name, yellInfoForMonkey);
        const int64_t yellForOp2Monkey = findYellForMonkey(yellForMonkey.monkeyOp2Name, yellInfoForMonkey);
        std::cout << "yellForOp1Monkey: " << yellForMonkey.monkeyOp1Name << " = " << yellForOp1Monkey << std::endl;
        std::cout << "yellForOp2Monkey: " << yellForMonkey.monkeyOp2Name << " = " << yellForOp2Monkey << std::endl;
        switch (yellForMonkey.operation)
        {
            case '+':
                return yellForOp1Monkey + yellForOp2Monkey;
            case '-':
                return yellForOp1Monkey - yellForOp2Monkey;
            case '/':
                assert(yellForOp2Monkey != 0);
                return yellForOp1Monkey / yellForOp2Monkey;
            case '*':
                return yellForOp1Monkey * yellForOp2Monkey;
            default:
                assert(false);
        };
    }
    assert(false);
}

int main()
{
    std::map<string, MonkeyYell> yellInfoForMonkey;

    string monkeyYellInfoLine;
    std::regex monkeyYellOpRegex(R"(^(\w+):\s*(\w+)\s*([+/\-*])\s*(\w+)$)");
    std::regex monkeyYellNumberRegex(R"(^(\w+):\s*(\d+)$)");

    while (std::getline(cin, monkeyYellInfoLine))
    {
        MonkeyYell monkeyYellInfo;

        std::smatch monkeyYellOpMatch;
        const bool opMatchSuccessful = std::regex_match(monkeyYellInfoLine, monkeyYellOpMatch, monkeyYellOpRegex);
        if (opMatchSuccessful)
        {
            monkeyYellInfo.yellType = MonkeyYell::Type::Operation;
            monkeyYellInfo.monkeyName = monkeyYellOpMatch[1];
            monkeyYellInfo.monkeyOp1Name = monkeyYellOpMatch[2];
            monkeyYellInfo.monkeyOp2Name = monkeyYellOpMatch[4];
            monkeyYellInfo.operation = std::string(monkeyYellOpMatch[3])[0];
            std::cout << "Op: name: " << monkeyYellInfo.monkeyName << " opname1: " << monkeyYellInfo.monkeyOp1Name << " - opname2: " << monkeyYellInfo.monkeyOp2Name << " - op: " << monkeyYellInfo.operation << std::endl;
        }
        else
        {
            std::smatch monkeyYellNumberMatch;
            const bool numberMatchSuccessful = std::regex_match(monkeyYellInfoLine, monkeyYellNumberMatch, monkeyYellNumberRegex);
            assert(numberMatchSuccessful);

            monkeyYellInfo.yellType = MonkeyYell::Type::Number;
            monkeyYellInfo.monkeyName = monkeyYellNumberMatch[1];
            monkeyYellInfo.numberToYell = std::stoll(monkeyYellNumberMatch[2]);
            std::cout << "Number: name: " << monkeyYellInfo.monkeyName << " - numberToYell: " << monkeyYellInfo.numberToYell << std::endl;
        }
        yellInfoForMonkey[monkeyYellInfo.monkeyName] = monkeyYellInfo;
    }

    const int64_t answer = findYellForMonkey("root", yellInfoForMonkey);

    std::cout << "root yells: " << answer << std::endl;


}
