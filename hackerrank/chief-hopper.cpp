// Simon St James (ssjgz) 2018-01-23
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

int main()
{
    int N;
    cin >> N;

    vector<int> heights(N);
    for (int i = 0; i < N; i++)
    {
        cin >> heights[i];
    }

    int startEnergy = 0;
    for (int i = N - 1; i >= 0; i--)
    {
        if (i >= 1)
        {
            heights[i - 1] += heights[i] / 2;
        }
        else
        {
            startEnergy = heights[i] / 2;
        }
        heights[i] %= 2;
    }

    for (const auto h : heights)
    {
        if (h != 0)
        {
            startEnergy++;
            break;
        }
    }

    cout << startEnergy << endl;
}
