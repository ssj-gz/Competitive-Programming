#include <iostream>
#include <vector>
#include <sstream>

using namespace std;

int main()
{
    string sequenceLine;
    int64_t result = 0;
    while (getline(cin, sequenceLine))
    {
        if (sequenceLine.empty())
            continue;
        std::cout << "sequenceLine: " << sequenceLine << std::endl;
        auto parseNumbers = [](const std::string& numberListString)
        {
            vector<int64_t> numbers;
            istringstream numberListStream(numberListString);
            int64_t number;
            while (numberListStream >> number)
            {
                numbers.push_back(number);
            }
            return numbers;
        };

        vector<int64_t> sequence = parseNumbers(sequenceLine);
        //vector<int64_t> lastInSequence;

        int64_t sequenceValue = 0;
        while (true)
        {
            std::cout << "Sequence: " << std::endl;
            for (const auto x : sequence)
            {
                std::cout << " " << x;
            }
            std::cout << std::endl;
            bool isAllZeros = true;
            for (const auto x : sequence)
            {
                if (x != 0)
                {
                    isAllZeros = false;
                    break;
                }
            }
            if (isAllZeros)
                break;
            sequenceValue += sequence.back();
            //lastInSequence.push_back(sequence.back());
            vector<int64_t> nextSequence;
            for (int i = 1; i < sequence.size(); i++)
            {
                nextSequence.push_back(sequence[i] - sequence[i - 1]);
            }
            sequence = nextSequence;
        }
        std::cout << " sequenceValue: " << sequenceValue << std::endl;
        result += sequenceValue;


    }
    std::cout << result << std::endl;
}
