// Simon St James (ssjgz) - 2019-11-30
// 
// Solution to: https://www.codechef.com/problems/SNELECT
//
#include <iostream>
#include <vector>

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
        auto animalString = read<string>();
        for (int i = 0; i < animalString.size(); i++)
        {
            if (animalString[i] == 'm')
            {
                // Prefer to eat the leftmost snake, if there is one.
                // The 'e' character represents "eaten snake".
                if (i - 1 >= 0 && animalString[i - 1] == 's')
                {
                    animalString[i - 1] = 'e';
                }
                else if(i + 1 < animalString.size() && animalString[i + 1] == 's')
                {
                    animalString[i + 1] = 'e';
                }
            }
        }

        int numSnakes = 0;
        int numMongooses = 0;
        for (const auto animalChar : animalString)
        {
            if (animalChar == 'm')
                numMongooses++;
            else if (animalChar == 's')
                numSnakes++;
        }

        if (numSnakes == numMongooses)
            cout << "tie";
        else if (numSnakes < numMongooses)
            cout << "mongooses";
        else
            cout << "snakes";
        cout << endl;
    }

    assert(cin);
}
