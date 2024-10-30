#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>

#include <cassert>

using namespace std;

int main()
{
    std::string lengthsAscii;
    std::getline(cin, lengthsAscii);
    vector<int> lengths;
    for (const auto lengthChar : lengthsAscii)
    {
        lengths.push_back(static_cast<int>(lengthChar));
    }
    lengths.push_back(17); lengths.push_back(31); lengths.push_back(73); lengths.push_back(47); lengths.push_back(23);
    for (const auto x : lengths)
        std::cout << x << " ";
    std::cout << std::endl;

    constexpr int numElements = 256;
    vector<int> elements;
    for (int i = 0; i < numElements; i++)
    {
        elements.push_back(i);
    }

    int currentPosition = 0;
    int skipSize = 0;
    for (int round = 1; round <= 64; round++)
    {
        for (const auto& length : lengths)
        {
            int reversalBeginPos = currentPosition;
            int reversalEndPos = (currentPosition + length - 1) % numElements;
            for (int i = 0; i < length / 2; i++)
            {
                swap(elements[reversalBeginPos], elements[reversalEndPos]);
                reversalBeginPos = (reversalBeginPos + 1) % numElements;
                reversalEndPos = (reversalEndPos - 1 + numElements) % numElements;
            }
            currentPosition = (currentPosition + length + skipSize) % numElements;
            skipSize++;
            std::cout << "After processing length: " << length << " elements are: " << std::endl;
            for (const auto& x : elements)
                std::cout << x << " ";
            std::cout << std::endl;
        }
    }
    constexpr int blockSize = 16;
    ostringstream outputHexStream;
    while (!elements.empty())
    {
        const vector<int> block(elements.begin(), elements.begin() + blockSize);
        elements.erase(elements.begin(), elements.begin() + blockSize);
        assert(elements.size() % blockSize == 0);

        int hashValue = 0;
        for (const auto& value : block)
        {
            hashValue ^= value;
        }

        outputHexStream << std::setfill('0') << std::setw(2) << std::hex << hashValue;
    }


    std::cout << "result: " << outputHexStream.str() << std::endl;

    return 0;
}
