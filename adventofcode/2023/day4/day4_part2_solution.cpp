#include <iostream>
#include <vector>
#include <sstream>
#include <regex>
#include <cassert>

using namespace std;

int main()
{
    string scratchCardLine;
    std::regex scratchCardRegex(R"(^Card\s*(\d+)\s*:(.*)\|(.*)$)");
    vector<int> numWinsForScratchCard; // Indexed by id, but made zero-relative.
    while (getline(cin, scratchCardLine))
    {
        std::cout << "scratchCardLine: " << scratchCardLine << std::endl;
        std::smatch scratchCardRegexMatch;
        const bool matchSuccesful = std::regex_match(scratchCardLine, scratchCardRegexMatch, scratchCardRegex);
        assert(matchSuccesful);
        std::cout << " card id: " << std::stoi(scratchCardRegexMatch[1]) << " winning numbers: " << scratchCardRegexMatch[2] << " my numbers: " << scratchCardRegexMatch[3] << std::endl;
        auto parseNumbers = [](const std::string& numberListString)
        {
            vector<int> numbers;
            istringstream numberListStream(numberListString);
            int number;
            while (numberListStream >> number)
            {
                numbers.push_back(number);
            }
            return numbers;
        };
        const vector<int> winningNumbers = parseNumbers(scratchCardRegexMatch[2]);
        const vector<int> myNumbers = parseNumbers(scratchCardRegexMatch[3]);
        int numWinningsNumbers = 0;
        for (const auto& winningNumber : winningNumbers)
        {
            if (std::find(myNumbers.begin(), myNumbers.end(), winningNumber) != myNumbers.end())
            {
                numWinningsNumbers++;
            }
        }
        numWinsForScratchCard.push_back(numWinningsNumbers);
    }
    int64_t totalScratchCardsProcessed = 0;
    vector<int64_t> numOfScratchCardToProcess(numWinsForScratchCard.size(), 1);
    while (true)
    {
        bool changeMade = false;
        for (int scratchCardId = 0; scratchCardId < numOfScratchCardToProcess.size(); scratchCardId++)
        {
            const int numOfScratchCard = numOfScratchCardToProcess[scratchCardId];
            std::cout << "numOfScratchCard for id: " << (scratchCardId + 1) << " = " << numOfScratchCard << std::endl;
            if (numOfScratchCard == 0)
                continue;
            totalScratchCardsProcessed += numOfScratchCard;
            numOfScratchCardToProcess[scratchCardId] = 0;
            const int numWins = numWinsForScratchCard[scratchCardId];
            for (int idToBump = scratchCardId + 1; idToBump <= std::min<int>(numOfScratchCardToProcess.size() - 1, scratchCardId + numWins); idToBump++)
            {
                changeMade = true;
                numOfScratchCardToProcess[idToBump] += numOfScratchCard;
                std::cout << " bumped idToBump: " << (idToBump + 1) << " up to " << numOfScratchCardToProcess[idToBump] << std::endl;
            }

        } 

        if (!changeMade)
            break;
    }
    std::cout << "totalScratchCardsProcessed: " << totalScratchCardsProcessed << std::endl;

}

