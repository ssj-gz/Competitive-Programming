// Simon St James (ssjgz) - 2019-11-06
// 
// Solution to: https://www.codechef.com/problems/SPELLBOB
//
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

bool canSpellBob(const string& topOrig, const string& bottomOrig)
{
    assert(topOrig.length() == 3 && bottomOrig.length() == 3);
    vector<int> permutation = {0, 1, 2};
    const auto origPermutation = permutation;
    while (true)
    {
        string top;
        for (auto index : permutation)
            top += topOrig[index];
        string bottom;
        for (auto index : permutation)
            bottom += bottomOrig[index];

        if ((top[0] == 'b' || bottom[0] == 'b') &&
            (top[1] == 'o' || bottom[1] == 'o') &&
            (top[2] == 'b' || bottom[2] == 'b'))
        {
            return true;
        }

        next_permutation(permutation.begin(), permutation.end());
        if (permutation == origPermutation)
            break;
    }

    return false;
}


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
        //const int T = rand() % 100 + 1;
        const int T = 1;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
        }

        return 0;
    }
    
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
