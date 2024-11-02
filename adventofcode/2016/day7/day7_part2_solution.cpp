#include <iostream>
#include <regex>
#include <set>

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

        set<string> areaBroadcastAccessors;
        set<string> byteAllocationBlocks;
        auto flushSubstring = [&currentSubstring, &isInSquareBrackets, &areaBroadcastAccessors, &byteAllocationBlocks]()
        {
            std::cout << "substring: " << currentSubstring << " isInSquareBrackets: " << isInSquareBrackets << std::endl;
            constexpr int abaLen = 3;
            for (string::size_type pos = 0; pos + abaLen - 1 < currentSubstring.size(); pos++)
            {
                if (currentSubstring[pos] == currentSubstring[pos + 2]  && currentSubstring[pos] != currentSubstring[pos + 1])
                {
                    const auto aba = currentSubstring.substr(pos, abaLen);
                    if (isInSquareBrackets)
                    {
                        byteAllocationBlocks.insert(aba);
                    }
                    else
                    {
                        areaBroadcastAccessors.insert(aba);
                    }

                }
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
        bool isValid = false;
        for (const auto& aba : areaBroadcastAccessors)
        {
            const std::string bab = std::string() + aba[1] + aba[0] + aba[1];
            if (byteAllocationBlocks.contains(bab))
            {
                std::cout << "byteAllocationBlocks contains " << bab << " corresponding to aba: " << aba << std::endl;
                isValid = true;
            }
        }
        std::cout << "isValid: " << isValid << std::endl;
        if (isValid)
            numValidIps++;
    }

    std::cout << "numValidIps: " << numValidIps << std::endl;


    return 0;
}
