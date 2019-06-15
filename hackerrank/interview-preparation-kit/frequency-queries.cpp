// Simon St James (ssjgz) - 2019-06-15
#include <iostream>
#include <map>
#include <vector>
#include <cassert>

using namespace std;

int main()
{
    // This one is pretty I/O heavy.
    ios::sync_with_stdio(false);

    int Q;
    cin >> Q;

    const int maxFrequency = Q; // We can add at most one element per query.
    map<int, int> numOccurrencesOf;
    vector<int> numWithFrequency(maxFrequency + 1, 0);

    for (int q = 0; q < Q; q++)
    {
        int queryType;
        cin >> queryType;
        int queryValue;
        cin >> queryValue;

        assert(queryValue != 0);
        switch (queryType)
        {
            case 1:
                {
                    const auto prevFrequency = numOccurrencesOf[queryValue];
                    const auto newFrequency = prevFrequency + 1;
                    numOccurrencesOf[queryValue]++;
                    numWithFrequency[prevFrequency]--;
                    numWithFrequency[newFrequency]++;
                    break;
                }
            case 2:
                {
                    const auto prevFrequency = numOccurrencesOf[queryValue];
                    if (prevFrequency > 0)
                    {
                        const auto newFrequency = prevFrequency - 1;
                        assert(newFrequency >= 0);
                        numOccurrencesOf[queryValue]--;
                        numWithFrequency[prevFrequency]--;
                        numWithFrequency[newFrequency]++;
                    }
                    break;
                }
            case 3:
                {
                    if (queryValue < numWithFrequency.size() && numWithFrequency[queryValue] > 0)
                    {
                        cout << 1 << endl;
                    }
                    else
                    {
                        cout << 0 << endl;
                    }
                    break;
                }
            default:
                assert(false);
        }
    }
    assert(cin);
}
