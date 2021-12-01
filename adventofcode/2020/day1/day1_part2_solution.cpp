#include <iostream>
#include <vector>

using namespace std;

int main()
{
    vector<int64_t> entries;
    int64_t entry = 0;
    while (cin >> entry)
        entries.push_back(entry);

    const int numEntries = entries.size();
    int64_t answer = -1;
    for (int i = 0; i < numEntries; i++)
    {
        for (int j = i + 1; j < numEntries; j++)
        {
            for (int k = j + 1; k < numEntries; k++)
            {
            if (entries[i] + entries[j] + entries[k]  == 2020)
                answer = entries[i] * entries[j] * entries[k];
            }
        }
    }
    cout << answer << endl;
}

