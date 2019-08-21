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

bool isAmbiguous(const vector<int>& a)
{
    const int N = a.size();
    vector<int> inversePermutation(N);

    for (int i = 0; i < N; i++)
    {
        inversePermutation[a[i] - 1] = i + 1;
    }

    return (inversePermutation == a);
}

int main(int argc, char* argv[])
{
    while (true)
    {
        const int N = read<int>();
        if (N == 0)
            break;
        vector<int> a(N);
        for (auto& aValue : a)
        {
            aValue = read<int>();
        }

        if (isAmbiguous(a))
        {
            cout << "ambiguous";
        }
        else
        {
            cout << "not ambiguous";
        }
        cout << endl;
    }

}
