// Simon St James (ssjgz) - 2019-09-05
// 
// Solution to: https://www.codechef.com/problems/RECIPE
//
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

int gcd(int a, int b)
{
    while (true)
    {
        if (a > b)
            swap(a, b);
        if (a == 0)
            break;
        const int nextA = a;
        const int nextB = b - a;

        a = nextA;
        b = nextB;
    }
    return b;
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
            const int N = 1 + rand() % 10;
            const int maxA = 1 + rand() % 1000;
            const int multiplier = 1 + rand() % min(10, maxA);
            assert(maxA / multiplier > 0);

            cout << N << endl;
            for (int i = 0; i < N; i++)
            {
                const int value = multiplier * (1 + rand() % (maxA / multiplier));
                assert(value > 0);
                cout << value;
                if (i != N - 1)
                {
                    cout << " ";
                }
            }
            cout << endl;
        }

        return 0;
    }

    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        vector<int> ingredients(N);
        for (auto& x : ingredients)
        {
            x = read<int>();
        }

        int gcdOfAllIngredients = ingredients.front();
        for (int i = 1; i < N; i++)
        {
            gcdOfAllIngredients = gcd(gcdOfAllIngredients, ingredients[i]);
        }

        for (auto& x : ingredients)
        {
            assert((x % gcdOfAllIngredients) == 0);
            cout << (x / gcdOfAllIngredients) << " ";
        }
        cout << endl;
    }

    assert(cin);
}
