#include <iostream>
#include <map>

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
        char mostFrequentLetter = '\0';
        int frequencyOfMostFrequent =  -1;
        for (const auto& [letterAndColumn, frequency] : letterFrequencyAtColumn)
        {
            if (letterAndColumn.second != col)
                continue;
            if (frequency > frequencyOfMostFrequent)
            {
                frequencyOfMostFrequent = frequency;
                mostFrequentLetter = letterAndColumn.first;
            }
        }
        errorCorrectedMessage += mostFrequentLetter;
    }
    std::cout << "errorCorrectedMessage: " << errorCorrectedMessage << std::endl;

    return 0;
}
