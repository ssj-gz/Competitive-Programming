#include <iostream>
#include <vector>

using namespace std;

int main()
{
    vector<int64_t> depths;
    int64_t depth = 0;
    while (cin >> depth)
        depths.push_back(depth);

    vector<int64_t> sums;
    for (int sumBeginIndex = 0; sumBeginIndex + 2 < depths.size(); sumBeginIndex++)
    {
        int64_t sum = 0;
        for (int depthIndex = sumBeginIndex; depthIndex <= sumBeginIndex + 2; depthIndex++)
        {
            sum += depths[depthIndex];
        }
        sums.push_back(sum);
    }


    bool havePreviousSum = false;
    int64_t previousSum = -1;
    int numIncreasedSums = 0;
    for (const auto sum : sums)
    {
        if (havePreviousSum && sum > previousSum)
            numIncreasedSums++;

        previousSum = sum;
        havePreviousSum = true;
    }
    cout << numIncreasedSums << endl;
}
