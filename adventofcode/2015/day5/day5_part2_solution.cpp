#include <iostream>
#include <iterator>
#include <algorithm>

#include <cassert>

using namespace std;

int main()
{
    string str;
    const char vowels[] = "aeiou";
    std::cout << " Blee: " << std::size(vowels) << std::endl;
    const string forbiddenStrings[] = {"ab", "cd", "pq", "xy"};
    int numNiceStrings = 0;
    while (getline(cin, str))
    {
        std::cout << "str: " << str << std::endl;
        bool containsNonOverlappingRepeat = false;
        for (string::size_type pos = 0; pos + 1 < str.size() && !containsNonOverlappingRepeat; pos++)
        {
            const auto pair = str.substr(pos, 2);
            string::size_type pairSearchBeginPos = 0;
            do
            {
                const auto pairOccurrencePos = str.find(pair, pairSearchBeginPos);
                if (pairOccurrencePos == string::npos)
                    break;
                if (pairOccurrencePos + 1 < pos || pairOccurrencePos >= pos + 2)
                {
                    containsNonOverlappingRepeat = true;
                    std::cout << " found repeat of " << pair << " (initial occurrence: " << pos << " non-overlapping occurrence: " << pairOccurrencePos << ")" << std::endl;
                }
                pairSearchBeginPos = pairOccurrencePos + 1;
            }
            while (true);
        }
        bool containsSandwich = false;
        for (string::size_type pos = 0; pos + 2 < str.size(); pos++)
        {
            if (str[pos] == str[pos + 2] && str[pos] != str[pos + 1])
            {
                containsSandwich = true;
                break;
            }
        }

        const bool isNice = containsNonOverlappingRepeat && containsSandwich;
        std::cout << "str: >" << str << "<" << " containsNonOverlappingRepeat: " << containsNonOverlappingRepeat << " containsSandwich: " << containsSandwich << " isNice: " << isNice << std::endl;
        if (isNice)
            numNiceStrings++;
    }
    std::cout << "numNiceStrings: " << numNiceStrings << std::endl;
    return 0;
}
