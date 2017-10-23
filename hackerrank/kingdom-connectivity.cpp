// Simon St James (ssjgz) - 2017-10-23 10:24
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

const int64_t modulus = 1'000'000'000ULL;

struct City
{
    vector<City*> neighboursOut;
    vector<City*> neighboursIn;

    bool visitedInDFS = false;
    bool isOnDFSStack = false;
    bool canReachW = false;
    int64_t numWaysOfReachingW = 0;
};

void setCanReachW(City* currentCity, City* parent)
{
    if (currentCity->visitedInDFS)
    {
        return;
    }

    currentCity->visitedInDFS = true;
    currentCity->canReachW = true;
    for (const auto neighbourIn : currentCity->neighboursIn)
    {
        //if (neighbourIn == parent)
            //continue;
        setCanReachW(neighbourIn, currentCity);
    }
}

int64_t findNumWaysOfReachingW(City* currentCity, City* parent, City* targetCity)
{
    if (!currentCity->canReachW)
        return 0;

    if (currentCity->isOnDFSStack)
        return -1;

    if (currentCity->visitedInDFS)
        return currentCity->numWaysOfReachingW;

    int64_t result = 0;
    currentCity->isOnDFSStack = true;
    currentCity->visitedInDFS = true;
    for (const auto child : currentCity->neighboursOut)
    {
        //if (child == parent)
            //continue;
        const auto childResult = findNumWaysOfReachingW(child, currentCity, targetCity);
        assert(childResult == -1 || (childResult >= 0 && childResult < ::modulus));
        if (childResult == -1)
        {
            result = -1;
            break;
        }
        else
            result = (result + childResult) % ::modulus;

    }
    currentCity->isOnDFSStack = false;

    if (currentCity == targetCity && result != -1)
    {
        result = 1;
    } 

    currentCity->numWaysOfReachingW = result;

    return result;

}

int main()
{
    int N, M;
    cin >> N >> M;
    
    vector<City> cities(N);
    for (int i = 0; i < M; i++)
    {
        int x, y;
        cin >> x >> y;
        x--;
        y--;

        City* roadBegin = &(cities[x]);
        City* roadEnd = &(cities[y]);

        roadBegin->neighboursOut.push_back(roadEnd);
        roadEnd->neighboursIn.push_back(roadBegin);
    }

    City* financialCity = &(cities.front());
    City* warfareCity = &(cities.back());

    setCanReachW(warfareCity, nullptr);

    int numCannotReach = 0;
    for (auto& city : cities)
    {
        city.visitedInDFS = false;
        if (!city.canReachW)
            numCannotReach++;
    }
    const auto numWaysFromFinancialToWarfare = findNumWaysOfReachingW(financialCity, nullptr, warfareCity);
    if (numWaysFromFinancialToWarfare == -1)
        cout << "INFINITE PATHS";
    else
        cout << numWaysFromFinancialToWarfare;

    cout << '\n';
}
