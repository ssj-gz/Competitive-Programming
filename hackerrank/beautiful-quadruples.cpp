#include <iostream>
#include <array>
#include <algorithm>

using namespace std;

constexpr int numComponents = 4;

uint64_t bruteForce(const array<int, numComponents>& maxComponentValues)
{
    uint64_t numBeautiful = 0;
    for (int i = 1; i <= maxComponentValues[0]; i++)
    {
        for (int j = i; j <= maxComponentValues[1]; j++)
        {
            for (int k = j; k <= maxComponentValues[2]; k++)
            {
                for (int l = k; l <= maxComponentValues[3]; l++)
                {
                    const bool isBeautiful = ((i ^ j ^ k ^ l) != 0);
                    if (isBeautiful)
                        numBeautiful++;
                }
            }
        }
    }
    return numBeautiful;
}

int main()
{
    array<int, numComponents> maxComponentValues;
    for (int i = 0; i < numComponents; i++)
    {
        cin >> maxComponentValues[i];
    }
    sort(maxComponentValues.begin(), maxComponentValues.end());

    cout << bruteForce(maxComponentValues) << endl;
}
