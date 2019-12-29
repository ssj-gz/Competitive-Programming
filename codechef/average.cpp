// Simon St James (ssjgz) - 2019-12-18
// 
// Solution to: https://www.codechef.com/IARCSJUD/problems/AVERAGE
//
#include <iostream>
#include <vector>
#include <algorithm>

#include <cassert>

using namespace std;

//#define DIAGNOSTICS

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

bool isAverageOfPair(const int targetAverage, const vector<int>& sortedA)
{
    assert(is_sorted(sortedA.begin(), sortedA.end()));

    const int targetPairSum = 2 * targetAverage;
    int rightIndex = sortedA.size() - 1;
    for (int leftIndex = 0; leftIndex < sortedA.size(); leftIndex++)
    {
        while (rightIndex > leftIndex && sortedA[leftIndex] + sortedA[rightIndex] > targetPairSum)
        {
            rightIndex--;
        }
        if (rightIndex <= leftIndex)
            return false;

        if (sortedA[leftIndex] + sortedA[rightIndex] == targetPairSum)
        {
#ifdef DIAGNOSTICS
            cout << "The number: " << targetAverage << " is the average of " << sortedA[leftIndex] << " and " << sortedA[rightIndex] << endl;
#endif
            return true;
        }
    }

    return false;
}

int countAverageElements(const vector<int>& a)
{
    int result = 0;
    vector<int> sortedA = a;
    sort(sortedA.begin(), sortedA.end());
    for (const auto& targetAverage : a)
    {
        const bool isAverage = isAverageOfPair(targetAverage, sortedA);
        if (isAverage)
        {
            result++;
        }
#ifdef DIAGNOSTICS
        else
        {
            cout << "The number: " << targetAverage << " is the not average of any pair of numbers" << endl;
        }
#endif
    }
    
    return result;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);

    const int N = read<int>();
    vector<int> a(N);
    for (auto& value : a)
        value = read<int>();

    cout << countAverageElements(a) << endl;

    assert(cin);
}
