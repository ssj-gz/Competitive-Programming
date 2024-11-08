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
        int numVowels = 0;
        for (const auto& letter : str)
        {
            if (std::find(std::begin(vowels), std::end(vowels), letter) != std::end(vowels))
            {
                numVowels++;
            }
        }
        bool containsDoubleLetter = false;
        for (string::size_type pos = 0; pos + 1 < str.size(); pos++)
        {
            if (str[pos] == str[pos + 1])
            {
                containsDoubleLetter = true;
                break;
            }
        }
        bool containsForbiddenString = false;
        for (const auto& forbiddenString : forbiddenStrings)
        {
            if (str.find(forbiddenString) != string::npos)
            {
                containsForbiddenString = true;
                break;
            }
        }
        const bool isNice = (numVowels >= 3) && containsDoubleLetter && !containsForbiddenString;
        std::cout << "str: >" << str << "<" << " numVowels: " << numVowels << " containsDoubleLetter: " << containsDoubleLetter << " containsForbiddenString: " << containsForbiddenString << " isNice: " << isNice << std::endl;
        if (isNice)
            numNiceStrings++;
    }
    std::cout << "numNiceStrings: " << numNiceStrings << std::endl;
    return 0;
}
