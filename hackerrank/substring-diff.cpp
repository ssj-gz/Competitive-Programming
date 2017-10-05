#include <iostream>
#include <vector>
#include <string>

using namespace std;

int blah(const string& a, const string& b, int S)
{
    //const int maxSizeDiffAB = abs(static_cast<long>(a.size() - b.size()));
    const int maxSizeDiffAB = a.size();

    int maxLWitNumMismatchesLessThanS = 0;
    for (int sizeDiffAB = 0; sizeDiffAB <= maxSizeDiffAB; sizeDiffAB++)
    {
        int lastIndexInAOfMismatch = -1;
        int lastIndexInBOfMismatch = -1;

        int i = a.size() - 1 - sizeDiffAB;
        int j = b.size() - 1;

        int commonSuffixLength = 0;
        int numMismatches = 0;

        while (i >= 0)
        {
            cout << "S: " << S << " sizeDiffAB: "<< sizeDiffAB << " i = " << i << " j = " << j << " a.size(): " << a.size() << " b.size(): " << b.size() << " Comparing: " << endl;
            cout << a.substr(i) << endl;
            cout << b.substr(j) << endl;
            commonSuffixLength++;
            if (a[i] != b[j])
            {
                if (lastIndexInAOfMismatch == -1 || lastIndexInBOfMismatch == -1)
                {
                    lastIndexInAOfMismatch = i;
                    lastIndexInBOfMismatch = j;
                }
                numMismatches++;
                if (numMismatches > S)
                {
                    cout << "Exceeded mismatches; last commonSuffixLength: " << commonSuffixLength << endl;
                    numMismatches--;
                    assert(lastIndexInAOfMismatch != -1 && lastIndexInBOfMismatch != -1);
                    assert(lastIndexInAOfMismatch >= i);
                    commonSuffixLength = (lastIndexInAOfMismatch - i);

                    cout << "new commonSuffixLength: " << commonSuffixLength << endl;

                    //while (lastIndexInAOfMismatch - 1 >= i && lastIndexInBOfMismatch - 1 >= j)
                    while (lastIndexInAOfMismatch - 1 >= 0 && lastIndexInBOfMismatch - 1 >= 0)
                    {
                        lastIndexInAOfMismatch--;
                        lastIndexInBOfMismatch--;
                        if (a[lastIndexInAOfMismatch] != b[lastIndexInBOfMismatch])
                        {
                            break;
                        }
                    }
                    cout << "new lastIndexInAOfMismatch: " << lastIndexInAOfMismatch << " new lastIndexInBOfMismatch: " << lastIndexInBOfMismatch << endl;
                }
            }
            else
            {
                //commonSuffixLength++;
            }

            cout << "commonSuffixLength: " << commonSuffixLength << endl;
            cout << "numMismatches: " << numMismatches << endl;

            maxLWitNumMismatchesLessThanS = max(maxLWitNumMismatchesLessThanS, commonSuffixLength);
            i--;
            j--;
        }
    }
    return maxLWitNumMismatchesLessThanS;
}

int main()
{
    int T;
    cin >> T;
    
    for (int t = 0; t < T; t++)
    {
        int S;
        cin >> S;
        string a, b;
        cin >> a >> b;

#if 0
        vector<vector<int>> numMatchingAfterIndices(a.size() + 1, vector<int>(b.size() + 1));
        for (int i = a.size() - 1; i >= 0; i--)
        {
            for (int j = b.size() - 1; j >= 0; j--)
            {
                const bool charsMatch = (a[i] == b[j]);
                numMatchingAfterIndices[i][j] = (charsMatch ? 1 : 0) + numMatchingAfterIndices[i + 1][j + 1];
            }
        }
        int maxL = 0;
        for (int i = 0; i < a.size(); i++)
        {
            for (int j = 0; j < b.size(); j++)
            {
                const int commonSuffixLength = min(a.size() - i, b.size() - j);
                const int numMismatches = commonSuffixLength - numMatchingAfterIndices[i][j];
                int dbgNumMismatches = 0;
                const auto dbA = a.substr(i, commonSuffixLength);
                const auto dbB = b.substr(j, commonSuffixLength);
                for (int k = 0; k < commonSuffixLength; k++)
                {
                    if (dbA[k] != dbB[k])
                        dbgNumMismatches++;
                }
                assert(dbgNumMismatches == numMismatches);
                if (numMismatches <= S)
                    maxL = max(maxL, commonSuffixLength);
            }
        }
        cout << maxL << endl;
#endif
        cout << max(blah(a, b, S), blah(b, a, S)) << endl;

    }
}
