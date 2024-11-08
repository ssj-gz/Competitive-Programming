#include <iostream>

#include <cassert>

using namespace std;

int main()
{
    string quotedString;
    int totalCodeChars = 0;
    int totalInMemoryChars = 0;
    while (std::getline(cin, quotedString))
    {
        std::cout << "quotedString: " << quotedString << std::endl;
        int numInMemoryChars = 0;
        bool isInQuotes = false;
        for (string::size_type pos = 0; pos < quotedString.size(); pos++)
        {
            const auto character = quotedString[pos];
            if (character == '"')
            {
                assert((pos == 0 && !isInQuotes) || (isInQuotes && (pos + 1 == quotedString.size())));
                isInQuotes = !isInQuotes;
            }
            else if (character == '\\')
            {
                assert(pos + 1 < quotedString.size());
                const auto escapedChar = quotedString[pos + 1];
                if (escapedChar == '\\')
                {
                    numInMemoryChars++;
                    pos++;
                }
                else if (escapedChar == '"')
                {
                    numInMemoryChars++;
                    pos++;
                }
                else if (escapedChar == 'x')
                {
                    numInMemoryChars++;
                    pos += 3;
                }
                else
                    assert(false);
            }
            else
                numInMemoryChars++;
        }
        std::cout << "numInMemoryChars: " << numInMemoryChars << std::endl;
        totalCodeChars += quotedString.size();
        totalInMemoryChars += numInMemoryChars;
    };
    std::cout << "totalInMemoryChars: " << totalInMemoryChars << " totalCodeChars: " << totalCodeChars << std::endl;
    std::cout << "result: " << (totalCodeChars - totalInMemoryChars) << std::endl;
    return 0;
}
