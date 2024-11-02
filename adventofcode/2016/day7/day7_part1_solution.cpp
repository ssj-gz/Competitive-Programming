#include <iostream>
#include <regex>

#include <cassert>

using namespace std;

int main()
{
    string ip;
    int numValidIps = 0;
    while (getline(cin, ip))
    {
        std::cout << "ip: " << ip << std::endl;
        bool isInSquareBrackets = false;
        string currentSubstring;

        bool hasAbbaOutOfSquareBrackets = false;
        bool hasAbbaInSquareBrackets = false;
        auto flushSubstring = [&currentSubstring, &isInSquareBrackets, &hasAbbaOutOfSquareBrackets, &hasAbbaInSquareBrackets]()
        {
            std::cout << "substring: " << currentSubstring << " isInSquareBrackets: " << isInSquareBrackets << std::endl;
            bool hasAbba = false;
            constexpr int abbaLen = 4;
            for (string::size_type pos = 0; pos < currentSubstring.size() - abbaLen + 1; pos++)
            {
                if (currentSubstring[pos] == currentSubstring[pos + 3] && currentSubstring[pos + 1] == currentSubstring[pos + 2] && currentSubstring[pos] != currentSubstring[pos + 1])
                {
                    hasAbba = true;
                }
            }
            std::cout << "hasAbba: " << hasAbba << std::endl;
            if (hasAbba)
            {
                if (isInSquareBrackets)
                    hasAbbaInSquareBrackets = true;
                else
                    hasAbbaOutOfSquareBrackets = true;
            }

            currentSubstring.clear();
        };

        for (string::size_type pos = 0; pos < ip.size(); pos++)
        {
            const auto character = ip[pos];
            if (character == '[')
            {
                assert(!isInSquareBrackets);
                flushSubstring();
                isInSquareBrackets = true;
            }
            else if (character == ']')
            {
                assert(isInSquareBrackets);
                flushSubstring();
                isInSquareBrackets = false;
            }
            else
            {
                currentSubstring += character;
                if (pos == ip.size() - 1)
                    flushSubstring();
            }
        }
        std::cout << " hasAbbaOutOfSquareBrackets: " << hasAbbaOutOfSquareBrackets << std::endl;
        std::cout << " hasAbbaInSquareBrackets: " << hasAbbaInSquareBrackets << std::endl;

        const bool valid = hasAbbaOutOfSquareBrackets && !hasAbbaInSquareBrackets;
        std::cout << " valid: " << valid << std::endl;
        if (valid)
            numValidIps++;
    }

    std::cout << "numValidIps: " << numValidIps << std::endl;


    return 0;
}
