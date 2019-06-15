// Simon St James (ssjgz) - 2019-06-15
#include <iostream>
#include <map>
#include <cassert>

using namespace std;

int main()
{
    int Q;
    cin >> Q;

    map<int, int> numOccurrencesOf;
    map<int, int> numWithFrequency;

    for (int q = 0; q < Q; q++)
    {
        int queryType;
        cin >> queryType;
        int queryValue;
        cin >> queryValue;

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
                    const auto newFrequency = prevFrequency - 1;
                    numOccurrencesOf[queryValue]--;
                    numWithFrequency[prevFrequency]--;
                    numWithFrequency[newFrequency]++;
                    break;
                }
            case 3:
                {
                    if (numWithFrequency[queryValue] > 0)
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
