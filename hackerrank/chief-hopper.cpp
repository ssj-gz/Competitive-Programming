#include <iostream>
#include <vector>

using namespace std;

int main()
{
    int N;
    cin >> N;

    vector<int> heights(N);
    for (int i = 0; i < N; i++)
    {
        cin >> heights[i];
    }

    int startEnergy = 0;
    while (true)
    {
        int energy = startEnergy;
        cout << "startEnergy: " << startEnergy << endl;
        for (int i = 0; i < N; i++)
        {
            cout << "i: " << i << " energy: " << energy << endl;
            if (heights[i] > energy)
            {
                energy -= (heights[i] - energy);
            } 
            else
            {
                energy += (energy - heights[i]);
            }

            //if (energy < 0)
                //break;
        }

        cout << "Done - final energy: " << energy << endl;

        if (energy >= 0)
        {
            cout << startEnergy;
            break;
        }


        startEnergy++;
    }

}
