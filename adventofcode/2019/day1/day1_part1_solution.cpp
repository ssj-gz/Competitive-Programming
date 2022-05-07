#include <iostream>

using namespace std;

int main()
{
    int64_t moduleMass;
    int64_t totalfuelNeeded = 0;
    while (cin >> moduleMass)
    {
        const int64_t moduleFuelNeeded = (moduleMass / 3) - 2;
        cout << "moduleFuelNeeded: " << moduleFuelNeeded << endl;
        totalfuelNeeded += moduleFuelNeeded;
    }
    cout << "totalfuelNeeded: " << totalfuelNeeded << endl;
}
