#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

int main()
{
#if 0
    int N;
    cin >> N;

    vector<int> heights(N);
    for (int i = 0; i < N; i++)
    {
        cin >> heights[i];
    }
#endif
    while (true)
    {
        const int N = rand() % 10 + 1;
        vector<int> heights;
        for (int i = 0; i < N; i++)
        {
            heights.push_back(rand() % 10000 + 1);
        }

        int64_t startEnergy = 0;
        while (true)
        {
            int64_t energy = startEnergy;
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
                int64_t dbgEnergy = (startEnergy << (i + 1));
                for (int j = 0; j <= i; j++)
                {
                    dbgEnergy -= (heights[j] << (i - j));
                }
                cout << "Wee: " << energy << " woo: " << dbgEnergy << " glah: " << -(dbgEnergy - (startEnergy << (i + 1))) << endl;
                assert(energy == dbgEnergy);

            }

            cout << "Done - final energy: " << energy << endl;

            if (energy >= 0)
            {
                cout << startEnergy << endl;
                break;
            }


            startEnergy++;
        }

        auto fleep = [&heights]()
        {
            int sausage = 0;
            const int N = heights.size();
            for (int i = 0; i < N; i++)
            {
                sausage += (1 << (N - 1 - i)) * heights[i];
                cout << "i: " << i << " sausage: " << sausage << endl;
            }
            return sausage;

        };

        const int sausage = fleep();

#if 0
        for (int i = N - 1; i >= 0; i--)
        {
            auto blee = heights[i];

            heights[i] = 0;
            int k = i;
            while (blee > 0 && k >= 0)
            {
                if ((blee % 2) == 1)
                {
                    heights[k]++;
                }

                if (k > 0)
                {
                    blee >>= 1;
                    k--;
                }
                else
                {
                    break;
                }

                cout << "i: " << i << " k: " << k << " blee: " << blee << endl;
            }
            //if (i == 0)
            heights[0] += blee;
        }
#endif
        int flipple = 0;
        for (int i = N - 1; i >= 0; i--)
        {
            if (i >= 1)
            {
            heights[i - 1] += heights[i] / 2;
            }
            else
            {
                flipple = heights[i] / 2;
            }
            heights[i] %= 2;
        }
        for (const auto h : heights)
        {
            if (h != 0)
            {
                flipple++;
                break;
            }
        }

        cout << "Final heights: " << endl;
        for (const auto h : heights)
        {
            cout << h << " ";
        }
        cout << endl;

        const int pickle = fleep();
        cout << "sausage: " << sausage << " pickle: " << pickle << endl;
        //assert(sausage == pickle);

        const int blah = (startEnergy * (1 << N)) - sausage;
        cout << "blah: " << blah << endl;

        cout << "startEnergy: " << startEnergy << " flipple: " << flipple << endl;
        assert(startEnergy == flipple);
    }
}
