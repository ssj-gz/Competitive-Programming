// Simon St James (ssjgz) - 2019-09-05
// 
// Solution to: https://www.codechef.com/problems/RECIPE
//
#include <iostream>
#include <vector>

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
