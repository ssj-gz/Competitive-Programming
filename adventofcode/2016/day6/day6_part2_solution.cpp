#include <iostream>
#include <map>
#include <limits>

#include <cassert>

using namespace std;

int main()
{
    std::map<std::pair<char, int>, int> letterFrequencyAtColumn;
    string message;
    int numColumns = 0;
    while (getline(cin, message))
    {
        numColumns = std::max(numColumns, static_cast<int>(message.size()));
        for (int col = 0; col < static_cast<int>(message.size()); col++)
        {
            const auto letter = message[col];
            letterFrequencyAtColumn[{letter,col}]++;
        }
    }

    string errorCorrectedMessage;
    for (int col = 0; col < numColumns; col++)
    {
        char leastFrequentLetter = '\0';
        int frequencyOfLeastFrequent =  std::numeric_limits<int>::max();
        for (const auto& [letterAndColumn, frequency] : letterFrequencyAtColumn)
        {
            if (letterAndColumn.second != col)
                continue;
            if (frequency < frequencyOfLeastFrequent)
            {
                frequencyOfLeastFrequent = frequency;
                leastFrequentLetter = letterAndColumn.first;
            }
        }
        errorCorrectedMessage += leastFrequentLetter;
    }
    std::cout << "errorCorrectedMessage: " << errorCorrectedMessage << std::endl;

    return 0;
}
