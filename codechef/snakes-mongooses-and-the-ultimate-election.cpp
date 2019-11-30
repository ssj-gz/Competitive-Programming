// Simon St James (ssjgz) - 2019-11-30
// 
// Solution to: https://www.codechef.com/problems/SNELECT
//
#include <iostream>
#include <vector>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!
#include <algorithm> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

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
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
        //const int T = rand() % 100 + 1;
        const int T = 1;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
            string animalString;
            const int numSnakes = rand() % 20 + 1;
            const int numMongooses = rand() % 20 + 1;
            for (int i = 0; i < numSnakes; i++)
                animalString += 's';
            for (int i = 0; i < numMongooses; i++)
                animalString += 'm';
            random_shuffle(animalString.begin(), animalString.end());
            cout << animalString << endl;
        }

        return 0;
    }
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        auto animalString = read<string>();
        for (int i = 0; i < animalString.size(); i++)
        {
            if (animalString[i] == 'm')
            {
                // Prefer to eat the leftmost snake, if there is one.
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
