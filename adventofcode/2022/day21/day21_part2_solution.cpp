#include <iostream>
#include <map>
#include <regex>
#include <limits>
#include <cassert>

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

// Finds the yell for the named monkey and additionally, if this monkey depends (directly or indirectly)
// on humn, finds the value of humn's yell that such that this monkey's operation results in desiredValue.
// This value of humn's yell will be stored in yellInfoForMonkey["humn"].numberToYell.
int64_t findYellForMonkey(const std::string& monkeyName, std::map<string, MonkeyYell>& yellInfoForMonkey, int64_t desiredValue, std::map<string, bool>& monkeyDependsOnHumn)
{
    assert(yellInfoForMonkey.contains(monkeyName));
    const MonkeyYell& yellForMonkey = yellInfoForMonkey[monkeyName];
    if (yellForMonkey.yellType == MonkeyYell::Type::Number)
    {
        if (monkeyName == "humn")
        {
            // Yell the desiredValue, humn!
            yellInfoForMonkey[monkeyName].numberToYell = desiredValue;
        }
        assert(yellForMonkey.numberToYell != std::numeric_limits<int64_t>::max());
        return yellForMonkey.numberToYell;
    }
    else
    {
        assert(yellForMonkey.yellType == MonkeyYell::Type::Operation);
        int64_t yellForOp1Monkey = 0;
        int64_t yellForOp2Monkey = 0;

        // Calculate the result for those of monkeyOp1Name & monkeyOp2Name which don't depend on humn
        // (at least one of them will not).
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
            // Call findYellForMonkey for monkeyOp1, but with a desiredValueForOp1 specifically calculated
            // to satisfy the desiredValue argument that was passed in.
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
            // Call findYellForMonkey for monkeyOp2, but with a desiredValueForOp2 specifically calculated
            // to satisfy the desiredValue argument that was passed in.
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
        // We now have yellForOp1Monkey and yellForOp2Monkey; perform the operation.  
        // If one of them depended on humn, then the result of the operation should be
        // desiredValue.
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
        }
        else
        {
            std::smatch monkeyYellNumberMatch;
            const bool numberMatchSuccessful = std::regex_match(monkeyYellInfoLine, monkeyYellNumberMatch, monkeyYellNumberRegex);
            assert(numberMatchSuccessful);

            monkeyYellInfo.yellType = MonkeyYell::Type::Number;
            monkeyYellInfo.monkeyName = monkeyYellNumberMatch[1];
            monkeyYellInfo.numberToYell = std::stoll(monkeyYellNumberMatch[2]);
        }
        yellInfoForMonkey[monkeyYellInfo.monkeyName] = monkeyYellInfo;
    }

    // Find the monkeys that depend (directly or indirectly) on humn.
    std::map<string, bool> monkeyDependsOnHumn;
    calcIfMonkeyDependsOnHumn("root", yellInfoForMonkey, monkeyDependsOnHumn);

    yellInfoForMonkey["root"].operation = '=';
    const int64_t rootYell = findYellForMonkey("root", yellInfoForMonkey, 1, monkeyDependsOnHumn);
    std::cout << "root yells: " << rootYell << std::endl;
    assert(rootYell == 1);

    std::cout << "The required value for humn to yell is: " << yellInfoForMonkey["humn"].numberToYell << std::endl;
}
