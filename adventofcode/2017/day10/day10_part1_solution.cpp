#include <iostream>
#include <vector>

#include <cassert>

using namespace std;

int main()
{
    vector<int> lengths;
    int length;
    while (cin >> length)
    {
        lengths.push_back(length);
        assert(cin);
        char comma;
        cin >> comma;
        assert(comma == ',' || !cin);
    }
    for (const auto length : lengths)
        std::cout << length << " ";
    std::cout << std::endl;

    constexpr int numElements = 256;
    //constexpr int numElements = 5;
    vector<int> elements;
    for (int i = 0; i < numElements; i++)
    {
        elements.push_back(i);
    }

    int currentPosition = 0;
    int skipSize = 0;
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

    std::cout << "result: " << elements[0] * elements[1] << std::endl;

    return 0;
}
