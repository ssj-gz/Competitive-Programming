#include <iostream>

#include <cassert>

using namespace std;

int main()
{
    string stream;
    cin >> stream;
    std::cout << "stream size: " << stream.size() << std::endl;

    string::size_type pos = 0;
    int nestingLevel = 0;
    bool isInGarbage = false;
    int64_t score = 0;
    while (pos < stream.size())
    {
        const auto character = stream[pos];
        if (!isInGarbage)
        {
            if (character == '<')
                isInGarbage = true;
            else if (character == '{')
            {
                nestingLevel++;
            }
            else if (character == '}')
            {
                score += nestingLevel;
                nestingLevel--;
                assert(nestingLevel >= 0);
            }
        }
        else
        {
            if (character == '!')
                pos++;
            else if (character == '>')
                isInGarbage = false;
        }

        pos++;
    }
    assert(nestingLevel == 0);
    std::cout << "score: " << score << std::endl;

    return 0;
}
