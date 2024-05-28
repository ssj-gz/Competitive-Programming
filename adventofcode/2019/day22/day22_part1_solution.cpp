#include <iostream>
#include <vector>
#include <regex>
#include <charconv>
#include <cassert>

using namespace std;

int main()
{

    std::regex numberRegex("-?\\d+");

    auto extractNumber = [&numberRegex](const std::string& stringContainingNumber)
    {
            std::smatch numMatch;
            if (std::regex_search(stringContainingNumber, numMatch, numberRegex))
            {
                std::string numberAsString(numMatch[0]);
                int number;
                auto [ptr, ec] = std::from_chars(numberAsString.data(), numberAsString.data()+numberAsString.size(), number);
                assert(ec == std::errc{});
                return number;
            }
            else
            {
                assert(false);
                return -1;
            }

    };

    const int numCards = 10007;
    string instructionLine;
    vector<int> cards;
    for (int i = 0; i < numCards; i++)
        cards.push_back(i);
    while (std::getline(cin, instructionLine))
    {
        if (instructionLine.starts_with("deal with increment "))
        {
            const int increment = extractNumber(instructionLine);
            std::cout << "Deal with increment " << increment << std::endl;
            vector<int> newCards(numCards, -1);
            int newCardsPos = 0;
            for (int i = 0; i < numCards; i++)
            {
                assert(newCards[newCardsPos] == -1);
                newCards[newCardsPos] = cards[i];

                newCardsPos = (newCardsPos + increment) % numCards;
            }
            cards = newCards;
        }
        else if (instructionLine.starts_with("deal into new stack"))
        {
            std::cout << "Deal into new stack" << std::endl;
            std::reverse(cards.begin(), cards.end());
        }
        else if (instructionLine.starts_with("cut"))
        {
            const int cutAmount = extractNumber(instructionLine);
            assert(abs(cutAmount) <= numCards);
            std::cout << "Cut " << cutAmount << std::endl;
            if (cutAmount >= 0)
                std::rotate(cards.begin(), cards.begin() + cutAmount, cards.end());
            else
                std::rotate(cards.rbegin(), cards.rbegin() + abs(cutAmount), cards.rend());
        }
        else
        {
            assert(false);
        }
        std::cout << "After processing this instruction, cards are now: " << std::endl;
        for (const auto card : cards)
        {
            cout << card << " ";
        }
        cout << endl;
    }

    const auto requiredCardIter = std::find(cards.begin(), cards.end(), 2019);
    assert(requiredCardIter != cards.end());
    std::cout << "Pos: " << (requiredCardIter - cards.begin()) << std::endl;
}
