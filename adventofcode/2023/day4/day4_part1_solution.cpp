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
    int64_t totalScore = 0;
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
        int64_t score = 0;
        for (const auto& winningNumber : winningNumbers)
        {
            if (std::find(myNumbers.begin(), myNumbers.end(), winningNumber) != myNumbers.end())
            {
                if (score == 0)
                    score = 1;
                else
                    score *= 2;
            }
        }
        std::cout << " score: " << score << std::endl;
        totalScore += score;
    }
    std::cout << "totalScore: " << totalScore << std::endl;
}

