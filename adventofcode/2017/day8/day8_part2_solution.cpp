#include <iostream>
#include <map>
#include <regex>
#include <limits>

#include <cassert>

using namespace std;

int main()
{
    string instruction;
    map<string, int64_t> registerValue;
    std::regex instructionRegex(R"(^(\w+) (inc|dec) ([-0-9]+) if ([-0-9a-z]+) (<|>|==|<=|>=|!=) ([-0-9a-z]+)$)");

    int64_t largestRegisterValue = std::numeric_limits<int64_t>::min();
    while (getline(cin, instruction))
    {
        std::smatch instructionMatch;
        std::cout << "instruction: " << instruction << std::endl;
        const bool matchSuccessful = std::regex_match(instruction, instructionMatch, instructionRegex);
        assert(matchSuccessful);
        auto value = [&registerValue](const std::string& regOrInt) -> int64_t
        {
            if (std::find_if(regOrInt.begin(), regOrInt.end(), [](const auto character) { return character >= 'a' && character <= 'z'; }) != regOrInt.end())
            {
                return registerValue[regOrInt];
            }
            else
                return std::stoll(regOrInt);

        };

        const auto lhsConditionalValue = value(instructionMatch[4]);
        const auto rhsConditionalValue = value(instructionMatch[6]);
        std::cout << "lhsConditionalValue: " << lhsConditionalValue << " rhsConditionalValue: " << rhsConditionalValue << std::endl;
        bool conditionFired = false;
        const auto conditionalOp = instructionMatch[5];
        if (conditionalOp == "==")
        {
           conditionFired = (lhsConditionalValue == rhsConditionalValue);
        }
        else if (conditionalOp == "!=")
        {
           conditionFired = (lhsConditionalValue != rhsConditionalValue);
        }
        else if (conditionalOp == "<=")
        {
           conditionFired = (lhsConditionalValue <= rhsConditionalValue);
        }
        else if (conditionalOp == ">=")
        {
           conditionFired = (lhsConditionalValue >= rhsConditionalValue);
        }
        else if (conditionalOp == "<")
        {
           conditionFired = (lhsConditionalValue < rhsConditionalValue);
        }
        else if (conditionalOp == ">")
        {
           conditionFired = (lhsConditionalValue > rhsConditionalValue);
        }
        else {
            assert(false);
        }
        if (conditionFired)
        {
            const auto decOrInc = instructionMatch[2];
            const auto regNameToChange = instructionMatch[1];
            const auto valueToChangeBy = value(instructionMatch[3]);
            if (decOrInc == "inc")
            {
                registerValue[regNameToChange] += valueToChangeBy;
            }
            else if (decOrInc == "dec")
            {
                registerValue[regNameToChange] -= valueToChangeBy;
            }
            else
            {
                assert(false);
            }
        }
        for (const auto& [regName, value] : registerValue)
        {
            largestRegisterValue = std::max(largestRegisterValue, value);
        }

    }

    for (const auto& [regName, value] : registerValue)
    {
        std::cout << "register: " << regName << " has value: " << value << std::endl;
    }
    std::cout << "largestRegisterValue: " << largestRegisterValue << std::endl;

    return 0;
}
