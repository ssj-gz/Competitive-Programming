#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main(int argc, char* argv[])
{
    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        int n;
        cin >> n;
        vector<int64_t> songLengths(n);

        for (int i = 0; i < n; i++)
        {
            cin >> songLengths[i];
        }
        int k;
        cin >> k;
        k--;

        const int uncleJohnySongLength = songLengths[k];

        sort(songLengths.begin(), songLengths.end());

        int newPosition = std::find(songLengths.begin(), songLengths.end(), uncleJohnySongLength) - songLengths.begin() + 1;
        cout << newPosition << endl;
    }


}
