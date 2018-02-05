// Simon St James (ssjgz) - 2018-02-05 07:07
#include <iostream>
#include <cassert>

using namespace std;

int main()
{
    int Q;
    cin >> Q;

    for (int q = 0; q < Q; q++)
    {
        string s;
        cin >> s;

        assert(s.size() >= 2);
        const auto endsInIc = s[s.size() - 2] == 'i' && s[s.size() - 1] == 'c' ;
        const auto beginsWithHydro = (s.find("hydro") == 0);

        if (endsInIc)
        {
            if (beginsWithHydro)
                cout << "non-metal acid";
            else
                cout << "polyatomic acid";
        }
        else
        {
            cout << "not an acid";
        }
        cout << endl;
    }
}
