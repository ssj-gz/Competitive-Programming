#include <iostream>
#include <vector>
#include <set>

using namespace std;

int main()
{
    set<int64_t> frequencies;
    vector<int64_t> frequencyChanges;

    int64_t frequency = 0;
    int64_t frequencyChange;
    while (cin >> frequencyChange)
    {
        frequencyChanges.push_back(frequencyChange);
    }

    while (true)
    {
        for (const auto& frequencyChange :frequencyChanges)
        {
            frequency += frequencyChange;
            if (frequencies.contains(frequency))
            {
                std::cout << "first repeated frequency: " << frequency << std::endl;
                return 0;
            }
            frequencies.insert(frequency);
        }
    }
    return 0;
}
