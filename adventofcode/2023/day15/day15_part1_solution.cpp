#include <iostream>

#include <cassert>

using namespace std;

int main()
{
    string initializationSequence;
    getline(cin, initializationSequence);
    assert(!initializationSequence.empty());
    int64_t result = 0;
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
        std::cout << "step: >" << step << "<" << std::endl;
        std::cout << "initializationSequence: >" << initializationSequence << "<" << std::endl;
        int64_t hash = 0;
        for (const auto character : step)
        {
            hash += static_cast<int>(character);
            hash *= 17;
            hash %= 256;
        }

        result += hash;
    }
    cout << "result: " << result << std::endl;
}
