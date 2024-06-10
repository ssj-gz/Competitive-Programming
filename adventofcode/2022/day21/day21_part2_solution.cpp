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

int64_t findYellForMonkey(const std::string& monkeyName, std::map<string, MonkeyYell>& yellInfoForMonkey, int64_t desiredValue, std::map<string, bool>& monkeyDependsOnHumn)
{
    assert(yellInfoForMonkey.contains(monkeyName));
    const MonkeyYell& yellForMonkey = yellInfoForMonkey[monkeyName];
    if (yellForMonkey.yellType == MonkeyYell::Type::Number)
    {
        if (monkeyName == "humn")
        {
            std::cout << "Me! - desiredValue: " << desiredValue << std::endl;
            yellInfoForMonkey[monkeyName].numberToYell = desiredValue;
        }
        assert(yellForMonkey.numberToYell != std::numeric_limits<int64_t>::max());
        return yellForMonkey.numberToYell;
    }
    else
    {
        assert(yellForMonkey.yellType == MonkeyYell::Type::Operation);
        //const int64_t yellForOp1Monkey = findYellForMonkey(yellForMonkey.monkeyOp1Name, yellInfoForMonkey, desiredValue, monkeyDependsOnHumn);
        //const int64_t yellForOp2Monkey = findYellForMonkey(yellForMonkey.monkeyOp2Name, yellInfoForMonkey, desiredValue, monkeyDependsOnHumn);
#if 1
        int64_t yellForOp1Monkey = 0;
        int64_t yellForOp2Monkey = 0;
        assert(!(monkeyDependsOnHumn[yellForMonkey.monkeyOp1Name] && monkeyDependsOnHumn[yellForMonkey.monkeyOp2Name]));

        if (!monkeyDependsOnHumn[yellForMonkey.monkeyOp1Name])
        {
            yellForOp1Monkey = findYellForMonkey(yellForMonkey.monkeyOp1Name, yellInfoForMonkey, desiredValue, monkeyDependsOnHumn);
        }
        if (!monkeyDependsOnHumn[yellForMonkey.monkeyOp2Name])
        {
            yellForOp2Monkey = findYellForMonkey(yellForMonkey.monkeyOp2Name, yellInfoForMonkey, desiredValue, monkeyDependsOnHumn);
        }

        if (monkeyDependsOnHumn[yellForMonkey.monkeyOp1Name])
        {
            int64_t desiredValueForOp1;
            switch (yellForMonkey.operation)
            {
                case '+':
                    desiredValueForOp1 = desiredValue - yellForOp2Monkey;
                    break;
                case '-':
                    desiredValueForOp1 = desiredValue + yellForOp2Monkey;
                    break;
                case '/':
                    desiredValueForOp1 = desiredValue * yellForOp2Monkey;
                    break;
                case '*':
                    assert(yellForOp2Monkey != 0);
                    assert(desiredValue % yellForOp2Monkey == 0);
                    desiredValueForOp1 = desiredValue / yellForOp2Monkey;
                    break;
                case '=':
                    desiredValueForOp1 = yellForOp2Monkey;
                    break;

                default:
                    assert(false);
            }

            yellForOp1Monkey = findYellForMonkey(yellForMonkey.monkeyOp1Name, yellInfoForMonkey, desiredValueForOp1, monkeyDependsOnHumn);
        }
        else if (monkeyDependsOnHumn[yellForMonkey.monkeyOp2Name])
        {
            int64_t desiredValueForOp2;
            switch (yellForMonkey.operation)
            {
                case '+':
                    desiredValueForOp2 = desiredValue - yellForOp1Monkey;
                    break;
                case '-':
                    desiredValueForOp2 = yellForOp1Monkey - desiredValue;
                    break;
                case '/':
                    assert(desiredValue != 0);
                    assert(yellForOp1Monkey % desiredValue == 0);
                    desiredValueForOp2 = yellForOp1Monkey / desiredValue;
                    break;
                case '*':
                    assert(yellForOp1Monkey != 0);
                    assert(desiredValue % yellForOp1Monkey == 0);
                    desiredValueForOp2 = desiredValue / yellForOp1Monkey;
                    desiredValue = yellForOp1Monkey * desiredValueForOp2;
                    break;
                case '=':
                    desiredValueForOp2 = yellForOp1Monkey;
                    break;
                default:
                    assert(false);
            }

            yellForOp2Monkey = findYellForMonkey(yellForMonkey.monkeyOp2Name, yellInfoForMonkey, desiredValueForOp2, monkeyDependsOnHumn);
        }
#endif
        //std::cout << "yellForOp1Monkey: " << yellForMonkey.monkeyOp1Name << " = " << yellForOp1Monkey << std::endl;
        //std::cout << "yellForOp2Monkey: " << yellForMonkey.monkeyOp2Name << " = " << yellForOp2Monkey << std::endl;
        switch (yellForMonkey.operation)
        {
            case '+':
                return yellForOp1Monkey + yellForOp2Monkey;
            case '-':
                return yellForOp1Monkey - yellForOp2Monkey;
            case '/':
                assert(yellForOp2Monkey != 0);
                assert(yellForOp1Monkey % yellForOp2Monkey == 0);
                return yellForOp1Monkey / yellForOp2Monkey;
            case '*':
                return yellForOp1Monkey * yellForOp2Monkey;
            case '=':
                return yellForOp1Monkey == yellForOp2Monkey;
            default:
                assert(false);
        };
    }
    assert(false);
}

bool calcIfMonkeyDependsOnHumn(const std::string& monkeyName, std::map<string, MonkeyYell>& yellInfoForMonkey, std::map<string, bool>& monkeyDependsOnHumn)
{
    bool answer = false;
    const auto& monkeyYell = yellInfoForMonkey[monkeyName];
    if (monkeyYell.monkeyName == "humn")
        answer = true;
    if (monkeyYell.yellType == MonkeyYell::Type::Operation)
        answer = calcIfMonkeyDependsOnHumn(monkeyYell.monkeyOp1Name, yellInfoForMonkey, monkeyDependsOnHumn) || calcIfMonkeyDependsOnHumn(monkeyYell.monkeyOp2Name, yellInfoForMonkey, monkeyDependsOnHumn);

    monkeyDependsOnHumn[monkeyName] = answer;
    return answer;
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

    std::map<string, bool> monkeyDependsOnHumn;
    calcIfMonkeyDependsOnHumn("root", yellInfoForMonkey, monkeyDependsOnHumn);
    for (const auto& [name, dependsOnHumn] : monkeyDependsOnHumn)
    {
        std::cout << "monkey: " << name << " dependsOnHumn? " << dependsOnHumn << std::endl;
    }

    yellInfoForMonkey["root"].operation = '=';
    const int64_t rootYell = findYellForMonkey("root", yellInfoForMonkey, 1, monkeyDependsOnHumn);
    std::cout << "root yells: " << rootYell << std::endl;

#if 0
    int64_t previousRootYell = std::numeric_limits<int64_t>::min();
    int64_t meYell = -1'000'000;
    while (true)
    {
        std::cout << "meYell: " << meYell << std::endl;
        assert(yellInfoForMonkey["humn"].yellType == MonkeyYell::Type::Number);
        yellInfoForMonkey["humn"].numberToYell = meYell;
        const int64_t rootYell = findYellForMonkey("root", yellInfoForMonkey);
        std::cout << "root yells: " << rootYell << std::endl;
        if (rootYell == 0)
        {
            std::cout << "Hooray!: " << meYell << std::endl;
            break;
        }
        if (previousRootYell != std::numeric_limits<int64_t>::min())
        {
            if (0 <= rootYell && rootYell < previousRootYell)
            {
                meYell *= 2;
            }
            else
            {
                meYell /= 2;
            }

        }

        previousRootYell = rootYell;
    }
#endif



}
