#include <iostream>
#include <vector>
#include <algorithm>
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

bool isAmbiguous(const vector<int>& a)
{
    const int N = a.size();
    vector<int> inversePermutation(N);

    for (int i = 0; i < N; i++)
    {
        inversePermutation[a[i] - 1] = i + 1;
    }

    return inversePermutation == a;
}


int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        int numGenerated = 0;
        int numAmbiguous = 0;
        int numNotAmbiguous = 0;

        for (int t = 0; t < 20; t++)
        {
            const int N = 15;
            vector<int> a(N);
            for (int i = 0; i < N; i++)
            {
                a[i] = (i + 1);
            }
            const auto originalA = a;

            const bool generateAmbiguous = ((rand() % 2) == 1);

            if (generateAmbiguous)
            {
                numAmbiguous++;
                vector<pair<int, int>> swaps;
                for (int i = 0; i < N; i++)
                {
                    for (int j = i; j < N; j++)
                    {
                        swaps.push_back({i, j});
                    }
                }
                random_shuffle(swaps.begin(), swaps.end());
                assert(isAmbiguous(a));
                for (const auto blah : swaps)
                {
                    swap(a[blah.first], a[blah.second]);
                    if (!isAmbiguous(a))
                    {
                        swap(a[blah.first], a[blah.second]);
                    }
                }
                assert(isAmbiguous(a));
            }
            else
            {
                numNotAmbiguous++;
                while (isAmbiguous(a))
                {
                    random_shuffle(a.begin(), a.end());
                }
                assert(!isAmbiguous(a));
            }
            cout << N << endl;
            for (const auto x : a)
            {
                cout << x << " ";
            }
            cout << endl;


        }
        cout << 0 << endl;
        cerr << "Generated " << numGenerated << " ambiguous: " << numAmbiguous << " not ambiguous: " << numNotAmbiguous << endl;
        return 0;
    }
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
