// Simon St James (ssjgz) - 2021-04-14
// 
// Solution to: https://www.codechef.com/problems/CHEFCH
//
#include <iostream>
#include <cassert>

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

int countChangesRequiredToMakeChain(const string& originalString)
{
    auto countChangesRequiredToMakeChainBeginningWith = [&originalString](const char firstCharInChain)
    {
        char requiredChar = firstCharInChain;
        int numChangesRequired = 0;
        for (const auto originalChar : originalString)
        {
            if (originalChar != requiredChar)
                numChangesRequired++;

            requiredChar = (requiredChar == '+' ? '-' : '+');
        }
        return numChangesRequired;
    };

    return min(countChangesRequiredToMakeChainBeginningWith('+'), countChangesRequiredToMakeChainBeginningWith('-'));
}


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        for (int i = 1; i < 8192; i++)
        {
            string s;
            int n = i;
            while (n > 0)
            {
                if ((n & 1) == 1)
                    s = '+' + s;
                else
                    s = '-' + s;
                n >>= 1;
            }
            cout << "Q: 2 lines" << endl;
            cout << 1 << endl;
            cout << s << endl;
            cout << "A: 1 lines" << endl;
            cout << countChangesRequiredToMakeChain(s) << endl;
        }

        return 0;
    }
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto originalString = read<string>();

        cout << countChangesRequiredToMakeChain(originalString) << endl;
    }

    assert(cin);
}
