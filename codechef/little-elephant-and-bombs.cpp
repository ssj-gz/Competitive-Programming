// Simon St James (ssjgz) - 2019-12-29
// 
// Solution to: https://www.codechef.com/problems/LEBOMBS
//
#include <iostream>
#include <vector>
#include <algorithm>

#include <cassert>

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int numBuildings = read<int>();
        const auto buildingString = read<string>();
        auto buildingStringAfterExplosion = buildingString;

        for (int buildingIndex = 0; buildingIndex < numBuildings; buildingIndex++)
        {
            if (buildingString[buildingIndex] == '1')
            {
                if (buildingIndex > 0)
                    buildingStringAfterExplosion[buildingIndex - 1] = 'E';
                if (buildingIndex + 1 < numBuildings)
                    buildingStringAfterExplosion[buildingIndex + 1] = 'E';
                buildingStringAfterExplosion[buildingIndex] = 'E';
            }
        }

#ifdef DIAGNOSTICS
        cout << "Original:        " << buildingString << endl;
        cout << "After explosion: " << buildingStringAfterExplosion << endl;
#endif

        cout << count(buildingStringAfterExplosion.begin(), buildingStringAfterExplosion.end(), '0') << endl;
    }

    assert(cin);
}
