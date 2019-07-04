#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <algorithm>

#include <cassert>

using namespace std;

int main()
{
    vector<char> digits = { '0' };
    map<int, vector<int>> decimalsForDeciBinary;
    decimalsForDeciBinary[0] = { 0 };

    int decimalValue = 0;
    while (decimalValue < 100'000'000)
    {
        int digitIndex = digits.size() - 1;
        while (digitIndex >= 0 && digits[digitIndex] == '9')
        {
            digits[digitIndex] = '0';
            digitIndex--;
        }
        if (digitIndex == -1)
        {
            digits.insert(digits.begin(), '1');
        }
        else
        {
            digits[digitIndex]++;
        }
        decimalValue++;

        int decimalValueFromDecibinary = 0;
        int powerOf2 = 1;

        for (auto digitReverseIter = digits.rbegin(); digitReverseIter != digits.rend(); digitReverseIter++)
        {
            decimalValueFromDecibinary += (*digitReverseIter - '0') * powerOf2;
            powerOf2 *= 2;
        }
        decimalsForDeciBinary[decimalValueFromDecibinary].push_back(decimalValue);
    }
#if 1
    int index = 1;
    for (auto& blah : decimalsForDeciBinary)
    {
        cout << blah.first << " (" << blah.second.size() << ")" << endl;
        for (const auto decimal : blah.second)
        {
            cout << blah.first << " " << decimal << "    " << index << endl;
            //break;
            index++;
        }
    }
    for (auto& blah : decimalsForDeciBinary)
    {
        if (blah.first >= 2 && ((blah.first % 2) == 0))
        {
            cout << "flopple: " << (decimalsForDeciBinary[blah.first].size() - decimalsForDeciBinary[blah.first - 2].size()) << endl;
        }
    }
#endif
#if 0
    // How can we form decimalsForDeciBinary[n + 2] from decimalsForDeciBinary[n]?
    const int n = 102;
    cout << "# with " << n << ": " << decimalsForDeciBinary[n].size() << " # with " << (n + 2) << ": " << decimalsForDeciBinary[n + 2].size() << endl;
    set<int> blee;
    int numDupes = 0;
    for (const auto& gleep : decimalsForDeciBinary[n])
    {
        cout << "gleep: " << gleep << endl;
        if ((gleep % 100) != 98)
        {
            int gloop = gleep;
            if ((gleep % 10) != 8)
            {
                gloop += 2;
            }
            else
            {
                gloop += 10;
            }
            const auto isDupe = (blee.find(gloop) != blee.end());
            if (isDupe)
            {
                numDupes++;
            }
            cout << " gloop: " << gloop << " is dupe: " <<  isDupe << endl;
            blee.insert(gloop);
        }
    }
    cout << "numDupes: " << numDupes << endl;
    cout << "# blee: " << blee.size() << endl;
    auto glarp = decimalsForDeciBinary[n + 2];
    for (const auto h : blee)
    {
        glarp.erase(std::remove(glarp.begin(), glarp.end(), h), glarp.end());
    }
    cout << "# in glarp: " << glarp.size() << endl;
    for (const auto h : glarp)
    {
        cout << "h: " << h << endl;
    }

#endif
}



