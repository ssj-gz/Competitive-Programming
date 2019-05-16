#include <iostream>
#include <vector>

using namespace std;

struct Query
{
    int x;
    int y;
    int k;
};

string binaryAsString(const vector<bool>& binary)
{
    string result;
    for (const auto digit : binary)
    {
        result.push_back('0' + digit);
    }
    return result;
}

int64_t g(const vector<bool>& binaryString)
{
    int64_t result = 0;
    for (int i = 0; i < binaryString.size(); i++)
    {
        for (int j = i; j < binaryString.size(); j++)
        {
            int64_t xorSum = 0;
            for (int k = i; k <=j; k++)
            {
                xorSum ^= binaryString[k];
            }
            result += xorSum;
        }
    }
    return result;
}

void findMaxGWithChanges(const vector<bool>& originalBinaryString, vector<bool>& binaryStringSoFar, int numChangesLeft, int numDigitsLeft, int64_t& greatestG)
{
    if (numDigitsLeft == 0)
    {
        greatestG = max(greatestG, g(binaryStringSoFar));
        return;
    }

    const bool currentDigit = originalBinaryString[originalBinaryString.size() - numDigitsLeft];
    binaryStringSoFar.push_back(currentDigit);
    findMaxGWithChanges(originalBinaryString, binaryStringSoFar, numChangesLeft, numDigitsLeft - 1, greatestG);
    binaryStringSoFar.pop_back();
    
    if (numChangesLeft > 0)
    {
        binaryStringSoFar.push_back(~currentDigit);
        findMaxGWithChanges(originalBinaryString, binaryStringSoFar, numChangesLeft - 1, numDigitsLeft - 1, greatestG);
        binaryStringSoFar.pop_back();
    }
}

int64_t findMaxGWithChanges(const vector<bool>& binaryString, int k)
{
    vector<bool> binaryStringSoFar;
    int64_t greatestG = -1;
    findMaxGWithChanges(binaryString, binaryStringSoFar, k, binaryString.size(), greatestG);
    return greatestG;
}

vector<int64_t> solveBruteForce(const vector<bool>& a, const vector<Query>& queries)
{
    vector<int64_t> result;
    for (const auto& query : queries)
    {
        vector<bool> extractedRange(a.begin() + query.x, a.begin() + query.y + 1);
        result.push_back(findMaxGWithChanges(extractedRange, query.k));
    }
    return result;
}



int main(int argc, char* argv[])
{
    int n;
    cin >> n;

    int Q;
    cin >> Q;

    vector<bool> a(n);
    for (int i = 0; i < n; i++)
    {
        int bit;
        cin >> bit;
        a[i] = bit;
    }

    vector<Query> queries(Q);
    for (int q = 0; q < Q; q++)
    {
        cin >> queries[q].x;
        cin >> queries[q].y;
        cin >> queries[q].k;
    }

    const auto resultsBruteForce = solveBruteForce(a, queries);
    for (const auto x : resultsBruteForce)
    {
        cout << x << endl;
    }

}
