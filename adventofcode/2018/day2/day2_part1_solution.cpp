#include <iostream>
#include <map>

using namespace std;

int main()
{
    string id;
    int numIdsWithTwoOccurrences = 0;
    int numIdsWithThreeOccurrences = 0;
    while (cin >> id)
    {
        map<char, int> letterHistogram;
        for (const auto& letter : id)
            letterHistogram[letter]++;
        bool hasLetterWithTwoOccurences = false;
        bool hasLetterWithThreeOccurences = false;
        for (const auto& [letter, frequency]  : letterHistogram)
        {
            if (frequency == 2)
                hasLetterWithTwoOccurences = true;
            else if (frequency == 3)
                hasLetterWithThreeOccurences = true;
        }
        if (hasLetterWithTwoOccurences)
            numIdsWithTwoOccurrences++;
        if (hasLetterWithThreeOccurences)
            numIdsWithThreeOccurrences++;
    }
    std::cout << "result: " << (numIdsWithTwoOccurrences * numIdsWithThreeOccurrences) << std::endl;
    return 0;
}
