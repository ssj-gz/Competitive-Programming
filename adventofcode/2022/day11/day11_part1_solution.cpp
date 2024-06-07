#include <iostream>
#include <sstream>
#include <deque>
#include <map>
#include <regex>

#include <cassert>

using namespace std;

std::smatch matchRegex(const std::string& regex, const std::string& line)
{
    std::cout << "line: " << line << " regex: " << regex << std::endl;
    std::smatch match;
    const bool matched = std::regex_match(line, match, std::regex(regex));
    assert(matched);
    return match;
}

int main()
{

    struct Monkey
    {
        deque<int> startingItems;

        enum { Multiply, Add } updateOp;
        bool updateAppliesToOld = false;
        int updateNumberToApplyTo;

        int testDivisibleBy = -1;

        int throwToMonkeyIfFalse = -1;
        int throwToMonkeyIfTrue = -1;

        int numItemsExpected = 0;

    };

    std::map<int, Monkey> monkeysById;

    while (true)
    {
        string monkeyDefLine;
        if (!std::getline(cin, monkeyDefLine))
        {
            break;
        }
        const std::smatch monkeyIdMatch = matchRegex(R"(^\s*Monkey\s*(\d+)\s*:\s*$)", monkeyDefLine);
        const int monkeyId = stoi(monkeyIdMatch[1]);
        std::cout << "monkeyId: " << monkeyId << std::endl;
        Monkey& newMonkey = monkeysById[monkeyId];

        std::getline(cin, monkeyDefLine);
        const std::smatch startingItemsMatch = matchRegex(R"(^\s*Starting\s*items\s*:\s*(.*)$)", monkeyDefLine);
        std::string startItemsString = startingItemsMatch[1];
        std::replace(startItemsString.begin(), startItemsString.end(), ',', ' ');
        istringstream startingItemsStream(startItemsString);
        int startingItem;
        while (startingItemsStream >> startingItem)
        {
            newMonkey.startingItems.push_back(startingItem);
            std::cout << " startingItem: " << startingItem << std::endl;
        }

        std::getline(cin, monkeyDefLine);
        const std::smatch updateFormulaMatch = matchRegex(R"(^\s*Operation\s*:\s*new\s*=\s*old\s*([^\s])\s*(old|\d+)\s*$)", monkeyDefLine);
        const string opString = updateFormulaMatch[1];
        const string opArg = updateFormulaMatch[2];
        std::cout << "opString: " << opString << " opArg: " << opArg << std::endl;
        assert(opString == "*" || opString == "+");
        newMonkey.updateOp = (opString == "*" ? Monkey::Multiply : Monkey::Add);
        if (opArg == "old")
        {
            newMonkey.updateAppliesToOld = true;
        }
        else
        {
            newMonkey.updateNumberToApplyTo = stoi(opArg);
        }


        std::getline(cin, monkeyDefLine);
        const std::smatch testDivisibleMatch = matchRegex(R"(^\s*Test\s*:\s*divisible\s*by\s*(\d+)\s*$)", monkeyDefLine);
        newMonkey.testDivisibleBy = stoi(testDivisibleMatch[1]);

        std::getline(cin, monkeyDefLine);
        const std::smatch monkeyToThrowToIfTrueMatch = matchRegex(R"(^\s*If\s*true\s*:\s*throw\s*to\s*monkey\s*(\d+)\s*$)", monkeyDefLine);
        newMonkey.throwToMonkeyIfTrue = stoi(monkeyToThrowToIfTrueMatch[1]);

        std::getline(cin, monkeyDefLine);
        const std::smatch monkeyToThrowToIfFalseMatch = matchRegex(R"(^\s*If\s*false\s*:\s*throw\s*to\s*monkey\s*(\d+)\s*$)", monkeyDefLine);
        newMonkey.throwToMonkeyIfFalse = stoi(monkeyToThrowToIfFalseMatch[1]);

        std::getline(cin, monkeyDefLine);
        assert(monkeyDefLine.empty());
    }

    for (int round = 1; round <= 20; round++)
    {
        std::cout << "round " << round << std::endl;
        for (auto& [monkeyId, monkey] : monkeysById)
        {
            const auto& monkeyItems = monkey.startingItems;
            std::cout << "Monkey " << monkeyId << " items: ";
            for (const auto item : monkeyItems)
            {
                std::cout << item << " ";
            }
            std::cout << std::endl;
        }
        for (auto& [monkeyId, monkey] : monkeysById)
        {
            auto& monkeyItems = monkey.startingItems;
            while (!monkeyItems.empty())
            {
                const int item = monkeyItems.front();
                monkeyItems.pop_front();
                monkey.numItemsExpected++;

                const int opArg = monkey.updateAppliesToOld ? item : monkey.updateNumberToApplyTo;
                //std::cout << "opArg: " << opArg << std::endl;
                int processedItemValue = (monkey.updateOp == Monkey::Multiply ? item * opArg : item + opArg);
                processedItemValue /= 3;
                //std::cout << "item: " << item << " processedItemValue: " << processedItemValue << std::endl;
                if ((processedItemValue % monkey.testDivisibleBy) == 0)
                {
                    monkeysById[monkey.throwToMonkeyIfTrue].startingItems.push_back(processedItemValue);
                }
                else
                {
                    monkeysById[monkey.throwToMonkeyIfFalse].startingItems.push_back(processedItemValue);
                }
            }
        }
    }

    vector<int> numItemsInspectedByMonkeys;
    for (auto& [monkeyId, monkey] : monkeysById)
    {
        std::cout << "Monkey: " << monkeyId << " inspected: " << monkey.numItemsExpected << std::endl;
        numItemsInspectedByMonkeys.push_back(monkey.numItemsExpected);
    }
    sort(numItemsInspectedByMonkeys.begin(), numItemsInspectedByMonkeys.end(), std::greater<>());
    std::cout << "monkey business: " << numItemsInspectedByMonkeys[0] * numItemsInspectedByMonkeys[1] << std::endl;

}