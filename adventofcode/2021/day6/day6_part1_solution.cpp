#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <numeric>

using namespace std;

int main()
{
    string agesLine;
    getline(cin, agesLine);
    std::replace(agesLine.begin(), agesLine.end(), ',', ' ');
    istringstream agesStream(agesLine);

    vector<int64_t> listOfLanternFishAges;
    int64_t age = -1;
    while (agesStream >> age)
    {
        listOfLanternFishAges.push_back(age);
    }
    cout << "# blah" << listOfLanternFishAges.size() << endl;

    for (int day = 1; day <= 80; day++)
    {
        vector<int64_t> nextListOfLanternFishAges;
        vector<int64_t> newFish;
        for (auto age : listOfLanternFishAges)
        {
            age--;
            if (age == -1)
            {
                nextListOfLanternFishAges.push_back(6);
                newFish.push_back(8);
            }
            else
            {
                nextListOfLanternFishAges.push_back(age);
            }
        }
        nextListOfLanternFishAges.insert(nextListOfLanternFishAges.end(), newFish.begin(), newFish.end());

        listOfLanternFishAges = nextListOfLanternFishAges;
        cout << "Day " << day << endl;
        for (const auto age : listOfLanternFishAges)
        {
            cout << age << " ";
        }
        cout << "(total: " << listOfLanternFishAges.size() << ")";
        cout << endl;
    }


}
