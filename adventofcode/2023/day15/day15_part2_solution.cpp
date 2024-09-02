#include <iostream>
#include <vector>
#include <algorithm>

#include <cassert>

using namespace std;

int main()
{
    struct Lens
    {
        string label;
        int focalLength = -1;
    };
    string initializationSequence;
    getline(cin, initializationSequence);
    assert(!initializationSequence.empty());
    vector<vector<Lens>> lensesInBox(256);
    while (!initializationSequence.empty())
    {
        const auto nextCommaPos = initializationSequence.find(',');
        string step;
        if (nextCommaPos != std::string::npos)
        {
            step = initializationSequence.substr(0, nextCommaPos);
            initializationSequence.erase(initializationSequence.begin(), initializationSequence.begin() + nextCommaPos + 1);
        }
        else
        {
            step = initializationSequence;
            initializationSequence.clear();
        }
        assert(!step.empty());
        //std::cout << "step: >" << step << "<" << std::endl;
        //std::cout << "initializationSequence: >" << initializationSequence << "<" << std::endl;

        auto hash = [](const auto& stringToHash)
        {
            int64_t hash = 0;
            for (const auto character : stringToHash)
            {
                hash += static_cast<int>(character);
                hash *= 17;
                hash %= 256;
            }
            return hash;
        };
        const auto posOfEquals = step.find('=');
        if (posOfEquals != string::npos)
        {
            Lens lens;
            lens.label = step.substr(0, posOfEquals);
            lens.focalLength = step.back() - '0';
            assert(lens.focalLength >= 1 && lens.focalLength <= 9);
            const int boxIndex = hash(lens.label);
            auto existingLensIter = std::find_if(lensesInBox[boxIndex].begin(), lensesInBox[boxIndex].end(), [label = lens.label](const auto& lens) { return lens.label == label; });
            if (existingLensIter != lensesInBox[boxIndex].end())
            {
                existingLensIter->focalLength = lens.focalLength;
            }
            else
                lensesInBox[boxIndex].push_back(lens);

        }
        else
        {
            assert(step.back() == '-');
            string label = step.substr(0, step.size() - 1);
            const int boxIndex = hash(label);
            auto existingLensIter = std::find_if(lensesInBox[boxIndex].begin(), lensesInBox[boxIndex].end(), [&label](const auto& lens) { return lens.label == label; });
            if (existingLensIter != lensesInBox[boxIndex].end())
            {
                lensesInBox[boxIndex].erase(existingLensIter);
            }
        }

        std::cout << "After \"" << step << "\"" << std::endl;
        for (int boxIndex = 0; boxIndex < lensesInBox.size(); boxIndex++)
        {
            if (!lensesInBox[boxIndex].empty())
            {
                std::cout << "Box " << boxIndex << ":";
                for (const auto& lens : lensesInBox[boxIndex])
                {
                    cout << "[" << lens.label << " " << lens.focalLength << "] ";
                }
                std::cout << endl;
            }
        }
    }
    int64_t focusingPower = 0;
    for (int boxIndex = 0; boxIndex < lensesInBox.size(); boxIndex++)
    {
        for (int lensIndex = 0; lensIndex < lensesInBox[boxIndex].size(); lensIndex++)
        {
            focusingPower += (boxIndex + 1) * (lensIndex + 1) * lensesInBox[boxIndex][lensIndex].focalLength;

        }
    }
    cout << "focusingPower: " << focusingPower << std::endl;
}
