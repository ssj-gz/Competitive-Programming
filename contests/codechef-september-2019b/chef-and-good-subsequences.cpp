// Simon St James (ssjgz) - 2019-09-09
// 
// Solution to: https://www.codechef.com/SEPT19B/problems/GDSUB
//
#include <iostream>
#include <vector>
#include <algorithm>

#include <cassert>

using namespace std;

const int64_t Mod = 1'000'000'007ULL;

class ModNum
{
    public:
        ModNum(int64_t n = 0)
            : m_n{n}
        {
            assert(n >= 0);
        }
        ModNum& operator+=(const ModNum& other)
        {
            m_n = (m_n + other.m_n) % Mod;
            return *this;
        }
        ModNum& operator*=(const ModNum& other)
        {
            m_n = (m_n * other.m_n) % Mod;
            return *this;
        }
        int64_t value() const { return m_n; };
    private:
        int64_t m_n;
};

ModNum operator+(const ModNum& lhs, const ModNum& rhs)
{
    ModNum result(lhs);
    result += rhs;
    return result;
}

ModNum operator*(const ModNum& lhs, const ModNum& rhs)
{
    ModNum result(lhs);
    result *= rhs;
    return result;
}

ostream& operator<<(ostream& os, const ModNum& toPrint)
{
    os << toPrint.value();
    return os;
}

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

ModNum findNumGoodSubsequences(const int N, const int K, const vector<int>& aOriginal)
{
    auto a = aOriginal;
    sort(a.begin(), a.end());
    struct DistinctElementInfo
    {
        int element = -1;
        int numOccurrences = 0;
    };
    vector<DistinctElementInfo> distinctElementInfo;

    int prevElement = -1;
    while (!a.empty())
    {
        const int element = a.back();
        if (element != prevElement)
        {
            distinctElementInfo.push_back({element, 0});
        }
        distinctElementInfo.back().numOccurrences++;

        a.pop_back();
        prevElement = element;
    }
    // The number numDistinctElements <= 1007, the number of distinct primes <= 8000.
    const int numDistinctElements = distinctElementInfo.size(); 

    // numWaysToChooseFromFirstDistinct[i][j] is the number of ways of choosing j elements from the first i groups of 
    // distinct elements.
    // NB: this crops up throughout - we can't make more choices than there are distinct elements!
    vector<vector<ModNum>> numWaysToChooseFromFirstDistinct(numDistinctElements + 1, vector<ModNum>(numDistinctElements + 1, 0));
    for (int i = 0; i <= numDistinctElements; i++)
    {
        numWaysToChooseFromFirstDistinct[i][0] = 1;
    }

    for (int firstNumDistinctElements = 1; firstNumDistinctElements <= numDistinctElements; firstNumDistinctElements++)
    {
        for (int numChosen = 1; numChosen <= firstNumDistinctElements; numChosen++)
        {
            assert(firstNumDistinctElements - 1 >= 0);
            // Don't choose any elements from this group of same elements.
            numWaysToChooseFromFirstDistinct[firstNumDistinctElements][numChosen] += numWaysToChooseFromFirstDistinct[firstNumDistinctElements - 1][numChosen]; 

            // Do choose *one* element from this group of same elements.
            assert(numChosen - 1 >= 0 && numChosen - 1 <= firstNumDistinctElements - 1);
            const auto numOfThisDistinctElement = (distinctElementInfo[firstNumDistinctElements - 1].numOccurrences);
            numWaysToChooseFromFirstDistinct[firstNumDistinctElements][numChosen] += 
                numWaysToChooseFromFirstDistinct[firstNumDistinctElements - 1][numChosen - 1] * numOfThisDistinctElement;

        }
    }

    ModNum numGoodSubsequences;
    for (int numChosen = 0; numChosen <= min(K, numDistinctElements); numChosen++)
    {
        numGoodSubsequences += numWaysToChooseFromFirstDistinct[numDistinctElements][numChosen];
    }
    
    return numGoodSubsequences;
}

int main(int argc, char* argv[])
{
    // Pretty easy one - since there are only 1007 primes up to 8000, our array A must
    // have the same elements duplicated many times over.
    //
    // If we group the elements of A into disjoint groups of a duplicated element, then a 
    // subset of A that contains no duplicates can be formed simply by choosing 
    // *at most one* element from each of these groups of duplicates.
    //
    // Hopefully the recurrence relation is clear - see numWaysToChooseFromFirstDistinct.
    //
    // Computing all values of numWaysToChooseFromFirstDistinct is O(numDistinctElements x numDistinctElements),
    // so ~1'000'000 operations - nice and tractable :)
    ios::sync_with_stdio(false);

    const int N = read<int>();
    const int K = read<int>();

    vector<int> a(N);
    for (auto& aElement : a)
    {
        aElement = read<int>();
    }

    cout << findNumGoodSubsequences(N, K, a) << endl;

    assert(cin);
}
