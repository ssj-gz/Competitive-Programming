#include <iostream>
#include <vector>

using namespace std;

const int64_t Mod = 1'000'000'007;


void bruteForceAux(const vector<int64_t>& a, int index, vector<int64_t>& setSoFar, int64_t& result)
{
    if (index == a.size())
    {
        int64_t xorSum = 0;
        for (const auto x : setSoFar)
        {
            xorSum ^= x;
        }
        result = (result + xorSum) % Mod;
        return;
    }
    // Without this element.
    bruteForceAux(a, index + 1, setSoFar, result);
    // With this element.
    setSoFar.push_back(a[index]);
    bruteForceAux(a, index + 1, setSoFar, result);
    setSoFar.pop_back();
}
int64_t bruteForce(const vector<int64_t>& a)
{
    int64_t result = 0;
    vector<int64_t> setSoFar;

    bruteForceAux(a, 0, setSoFar, result);

    return result;
}

int main()
{
    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        int n;
        cin >> n;

        vector<int64_t> a(n);
        for (int i = 0; i < n; i++)
        {
            cin >> a[i];
        }
        const auto bruteForceResult = bruteForce(a);
        cout << bruteForceResult << endl;
    }
}

