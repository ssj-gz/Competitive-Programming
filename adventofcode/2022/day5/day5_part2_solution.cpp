#include <iostream>
#include <vector>
#include <regex>
#include <cassert>

using namespace std;

int main()
{
    vector<string> crateDefLines;
    while (true)
    {
        string crateDefLine;
        std::getline(cin, crateDefLine);
        if (crateDefLine.empty())
            break;

        crateDefLines.push_back(crateDefLine);
    }

    const string allCrateNumbersLine = crateDefLines.back();
    std::cout << "allCrateNumbersLine: >" << allCrateNumbersLine << "< size: " << allCrateNumbersLine.size() << std::endl;
    crateDefLines.pop_back();
    assert((allCrateNumbersLine.size() + 1) % 4 == 0);
    const int numCrates = (allCrateNumbersLine.size() + 1) / 4;

    vector<string> cratesContents(numCrates);

    while (!crateDefLines.empty())
    {
        const string crateContentsLine = crateDefLines.back();
        crateDefLines.pop_back();

        for (int crateIndex = 0; crateIndex < numCrates; crateIndex++)
        {
            const auto crateContentPos = crateIndex * 4 + 1;
            if (crateContentPos >= crateContentsLine.size())
                break;

            const char crateContent = crateContentsLine[crateContentPos];
            if (crateContent != ' ')
            {
                cratesContents[crateIndex].push_back(crateContent);
            }
        }
    }

    for (int crateIndex = 0; crateIndex < numCrates; crateIndex++)
    {
        cout << "Crate: " << (crateIndex + 1) << " : " << cratesContents[crateIndex] << std::endl;
    }

    regex moveInstructionRegex(R"(^move (\d+) from (\d+) to (\d+)$)");
    string moveInstructionLine;
    while (std::getline(cin, moveInstructionLine))
    {
        std::smatch moveInstructionMatch;
        const bool matched = std::regex_match(moveInstructionLine, moveInstructionMatch, moveInstructionRegex);
        assert(matched);
        assert(moveInstructionMatch.size() == 4);
        const int numToMove = stoi(moveInstructionMatch[1].str());
        const int sourceCrateIndex = stoi(moveInstructionMatch[2].str()) - 1;
        const int destCrateIndex = stoi(moveInstructionMatch[3].str()) - 1;

        std::cout << "Move " << numToMove << " from " << (sourceCrateIndex + 1) << " to " << (destCrateIndex + 1) << std::endl;

        auto& sourceCrateStack = cratesContents[sourceCrateIndex];
        assert(sourceCrateStack.size() >= numToMove);

        const auto cratesToMove = string{sourceCrateStack.end() - numToMove, sourceCrateStack.end()};
        std::cout << " cratesToMove: " << cratesToMove << std::endl;
        sourceCrateStack.erase(sourceCrateStack.end() - numToMove, sourceCrateStack.end());
        cratesContents[destCrateIndex] += cratesToMove;

    }

    for (const auto& crateContents : cratesContents)
    {
        std::cout << crateContents.back();
    }
    std::cout << std::endl;


}
