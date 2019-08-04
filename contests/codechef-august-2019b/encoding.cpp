// Simon St James (ssjgz) - 2019-XX-XX
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>

#include <cassert>

#include <sys/time.h>

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

int64_t calcF(const string& s)
{
    int64_t result = 0;

    int powerOf10 = 1;
    int64_t toAdd = 0;
    for (int i = s.size() - 1; i >= 0; i--)
    {
        if (i + 1 < s.size() && s[i] != s[i + 1])
        {
            result += toAdd;
            toAdd = 0;
        }

        toAdd = powerOf10 * (s[i] - '0');
        powerOf10 = powerOf10 * 10;
    }
    result += toAdd;

    return result;
}

int64_t solveBruteForce(const string& L, const string& R)
{
    int64_t result = 0;
    string current = L;

    while (true)
    {
        //cout << " current: " << current << endl;
        result += calcF(current);
        if (current == R)
            break;

        int index = current.size() - 1;
        while (index >= 0 && current[index] == '9')
        {
            current[index] = '0';
            index--;
        }
        if (index == -1)
        {
            current = '1' + current;
        }
        else
        {
            current[index]++;
        }
    }

    return result;
}

#if 0
int64_t solveOptimised()
{
    int64_t result = 0;

    return result;
}
#endif


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
        return 0;
    }

    const int T = read<int>();
    for (int t = 0; t < T; t++)
    {
        read<int>();
        const string L = read<string>();
        read<int>();
        const string R = read<string>();


#ifdef BRUTE_FORCE
#if 1
        const auto solutionBruteForce = solveBruteForce(L, R);
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
#endif
#if 0
        const auto solutionOptimised = solveOptimised();
        cout << "solutionOptimised: " << solutionOptimised << endl;

        assert(solutionOptimised == solutionBruteForce);
#endif
#else
        const auto solutionOptimised = solveOptimised();
        cout << solutionOptimised << endl;
#endif
    }

}


