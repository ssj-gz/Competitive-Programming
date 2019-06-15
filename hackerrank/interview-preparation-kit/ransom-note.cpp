// Simon St James (ssjgz) - 2019-06-15.
#include <iostream>
#include <map>

using namespace std;

int main()
{
    int n, m;
    cin >> n >> m;

    map<string, int> numOfWordInMagazine;
    for (int i = 0; i < n; i++)
    {
        string magazineWord;
        cin >> magazineWord;

        numOfWordInMagazine[magazineWord]++;
    }

    for (int i = 0; i < m; i++)
    {
        string noteWord;
        cin >> noteWord;

        if (numOfWordInMagazine[noteWord] == 0)
        {
            cout << "No" << endl;
            return 0;
        }

        numOfWordInMagazine[noteWord]--;
    }

    cout << "Yes" << endl;
}



