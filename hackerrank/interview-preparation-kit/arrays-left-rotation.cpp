// Simon St James (ssjgz) - 2019-06-15
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

int main()
{
    // Easy one; hopefully code is self-explanatory.
    // Just doing these Interview Preparation Kit questions
    // for the Endorphin rush :)
    int n;
    cin >> n;

    int d;
    cin >> d;

    vector<int> a(n);

    for (int i = 0; i < n; i++)
    {
        cin >> a[i];
    }

    assert(cin);
    assert(d <= n);

    for (int j = d; j < n; j++)
    {
        cout << a[j] << " ";
    }
    for (int j = 0; j < d; j++)
    {
        cout << a[j] << " ";
    }
    cout << endl;
}
