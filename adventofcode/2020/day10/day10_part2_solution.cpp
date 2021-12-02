#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
    vector<int64_t> joltages;
    int64_t joltage;
    while (cin >> joltage)
        joltages.push_back(joltage);

    sort(joltages.begin(), joltages.end());
    const auto maxJoltage = joltages.back();

    vector<vector<int64_t>> numWaysOfReachingXUsingFirstY(maxJoltage + 1, vector<int64_t>(joltages.size() + 1, 0));

    numWaysOfReachingXUsingFirstY[0][0] = 1;

    for (int adaptorIndex = 0; adaptorIndex < joltages.size(); adaptorIndex++)
    {
        for (int joltage = 0; joltage <= maxJoltage; joltage++)
        {
            // Don't use this adaptor.
            numWaysOfReachingXUsingFirstY[joltage][adaptorIndex + 1] += numWaysOfReachingXUsingFirstY[joltage][adaptorIndex];
        }
        const int adaptorJoltage = joltages[adaptorIndex];
        for (int previousJoltage = adaptorJoltage - 3; previousJoltage <= adaptorJoltage - 1; previousJoltage++)
        {
            if (previousJoltage >= 0)
            {
                // Do use this adaptor (connected to adaptor with joltage previousJoltage).
                numWaysOfReachingXUsingFirstY[adaptorJoltage][adaptorIndex + 1] += numWaysOfReachingXUsingFirstY[previousJoltage][adaptorIndex];
            }
        }
#if 0
        cout << "Num of ways to reach joltage using first " << (adaptorIndex + 1) << " adaptors: " << endl;
        for (int joltage = 0; joltage <= maxJoltage; joltage++)
        {
            cout << " joltage: " << joltage << " numWaysOfReachingXUsingFirstY: " << numWaysOfReachingXUsingFirstY[joltage][adaptorIndex + 1] << endl;
        }
#endif

    }
    // The device *must* connect to the adaptor with highest joltage.
    cout << numWaysOfReachingXUsingFirstY[maxJoltage][joltages.size()] << endl;

}
