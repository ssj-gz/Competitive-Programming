// Simon St James (ssjgz) - 2019-11-06
// 
// Solution to: https://www.codechef.com/problems/SPELLBOB
//
#include <iostream>
#include <vector>
#include <algorithm>

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

string applyPermutation(const string toPermute, const vector<int>& indexPermutation)
{
    string permuted;
    for (auto index : indexPermutation)
        permuted += toPermute[index];

    return permuted;
}

bool canSpellBob(const string& topOrig, const string& bottomOrig)
{
    assert(topOrig.length() == 3 && bottomOrig.length() == 3);
    vector<int> indexPermutation = {0, 1, 2};
    const auto indexPermutationOrig = indexPermutation;
    while (true)
    {
        const string top = applyPermutation(topOrig, indexPermutation);
        const string bottom = applyPermutation(bottomOrig, indexPermutation);

        if ((top[0] == 'b' || bottom[0] == 'b') &&
            (top[1] == 'o' || bottom[1] == 'o') &&
            (top[2] == 'b' || bottom[2] == 'b'))
        {
            return true;
        }

        next_permutation(indexPermutation.begin(), indexPermutation.end());
        if (indexPermutation == indexPermutationOrig)
            break;
    }

    return false;
}


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto top = read<string>();
        const auto bottom = read<string>();

        if (canSpellBob(top, bottom))
            cout << "yes";
        else
            cout << "no";
        cout << endl;
    }

    assert(cin);
}
