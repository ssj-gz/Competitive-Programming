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
    joltages.push_back(joltages.back() + 3); // Built-in adaptor.
    joltages.insert(joltages.begin(), 0); // Outlet.

    int numJumpsOf1 = 0;
    int numJumpsOf3 = 0;
    for (int joltageindex = 1; joltageindex < joltages.size(); joltageindex++)
    {
        const int jumpSize = joltages[joltageindex] - joltages[joltageindex - 1];
        if (jumpSize == 1)
            numJumpsOf1++;
        else if (jumpSize == 3)
            numJumpsOf3++;
        cout << "jumpSize: " << jumpSize << endl;
    }
    cout << "numJumpsOf1: " << numJumpsOf1 << " numJumpsOf3: " << numJumpsOf3 << endl;
    cout << numJumpsOf1 * numJumpsOf3 << endl;
}
