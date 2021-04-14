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

    return min(countChangesRequiredToMakeChainBeginningWith('+'), 
               countChangesRequiredToMakeChainBeginningWith('-'));
}


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto originalString = read<string>();

        cout << countChangesRequiredToMakeChain(originalString) << endl;
    }

    assert(cin);
}
