#include <iostream>

#include <cassert>

using namespace std;

int main()
{
    string password;
    cin >> password;
    assert(cin);

    const string forbiddenStrings[] = {"i", "o", "l"};


    int64_t numTried = 0;
    do 
    {
        string::size_type incPos = password.size() - 1;
        while (password[incPos] == 'z')
        {
            password[incPos] = 'a';
            assert(incPos != 0);
            incPos--;
        }
        password[incPos]++;
        //std::cout << "candidate password: " << password << std::endl;

        bool containsNonOverlappingRepeat = false;
        for (string::size_type pos = 0; pos + 1 < password.size() && !containsNonOverlappingRepeat; pos++)
        {
            if (password[pos] != password[pos + 1])
                continue;
            for (string::size_type otherPos = pos + 2; otherPos + 1 < password.size(); otherPos++)
            {
                if (password[otherPos] == password[otherPos + 1])
                {
                    containsNonOverlappingRepeat = true;
                    break;
                }
            }
        }

        bool containsIncreasingSubsequence = false;
        for (string::size_type pos = 0; pos + 2 < password.size(); pos++)
        {
            if ((password[pos + 2] == password[pos + 1] + 1) && (password[pos + 1] == password[pos + 0] + 1))
            {
                containsIncreasingSubsequence = true;
                break;
            }
        }

        bool containsForbiddenChar = false;
        for (const auto& forbiddenString : forbiddenStrings)
        {
            if (password.find(forbiddenString) != string::npos)
            {
                containsForbiddenChar = true;
                break;
            }
        }
        //std::cout << "candidate password: " << password << " containsNonOverlappingRepeat: " << containsNonOverlappingRepeat << " containsForbiddenChar: " << containsForbiddenChar << " containsIncreasingSubsequence: " << containsIncreasingSubsequence << std::endl;
        numTried++;
        if (numTried % 100'000 == 0)
            cout << "numTried: " << numTried << std::endl;

        if (containsNonOverlappingRepeat && containsIncreasingSubsequence && !containsForbiddenChar)
        {
            break;
        }

    } while (true);
    std::cout << "password: " << password << std::endl;

    return 0;
}
