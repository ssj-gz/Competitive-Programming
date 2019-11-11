// Simon St James (ssjgz) - 2019-11-11
// 
// Solution to: https://www.codechef.com/problems/HRDSEQ
//
#include <iostream>
#include <vector>
#include <algorithm>

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

int solveBruteForce(int N)
{
    vector<int> elements;
    elements.push_back(0);

    for (int i = 0; i < N - 1; i++)
    {
        const auto lastElement = elements.back();
        int lastOccurence = -1;

        if (elements.size() > 1)
        {
            for (int index = elements.size() - 2;  index >= 0; index--)
            {
                if (elements[index] == lastElement)
                {
                    lastOccurence = index;
                    break;
                }
            }
        }

        if (lastOccurence != -1)
        {
            elements.push_back(elements.size() - 1 - lastOccurence);
        }
        else
        {
            elements.push_back(0);
        }
    }

    return count(elements.begin(), elements.end(), elements.back());
}

int main(int argc, char* argv[])
{
    // Easy one - the constraints are sufficiently small that a naive
    // brute-force approach will work.
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();

        cout << solveBruteForce(N) << endl;
    }

    assert(cin);
}
