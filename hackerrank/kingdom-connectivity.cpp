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

void setCanReachW(City* currentCity)
{
    if (currentCity->visitedInDFS)
        return;

    currentCity->visitedInDFS = true;
    currentCity->canReachW = true;
    for (const auto neighbourIn : currentCity->neighboursIn)
    {
        setCanReachW(neighbourIn);
    }
}

// Returns the number of ways of reaching the targetCity from currentCity, or -1 if there are infinite
// such paths.  Requires each cities canReachW to be up-to-date.
int64_t findNumWaysOfReachingW(City* currentCity, City* targetCity)
{
    if (!currentCity->canReachW)
        return 0;

    if (currentCity->isOnDFSStack)
        return -1;

    if (currentCity->visitedInDFS) // This city will have been fully explored, so use memo-ised result.
        return currentCity->numWaysOfReachingW;

    // We might be tempted to stop here and result "1" if currentCity == targetCity,
    // but we need to continue exploring in case there are self-loops from targetCity
    // to itself!

    int64_t result = 0;
    currentCity->isOnDFSStack = true;
    currentCity->visitedInDFS = true;
    for (const auto child : currentCity->neighboursOut)
    {
        const auto childResult = findNumWaysOfReachingW(child, targetCity);
        assert(childResult == -1 || (childResult >= 0 && childResult < ::modulus));
        if (childResult == -1)
        {
            // Cycle found; bail.
            result = -1;
            break;
        }
        else
            result = (result + childResult) % ::modulus;

    }
    currentCity->isOnDFSStack = false;

    if (currentCity == targetCity && result != -1)
    {
        // This is the target city, and we found no self-loops from targetCity to itself;
        // override the result with "1".
        result = 1;
    }

    // Memo-ise.
    currentCity->numWaysOfReachingW = result;

    return result;

}

int main()
{
    // Fundamentally easy, though with a few "schoolboy errors" along the way; the dumbest
    // of which was using the wrong modulus XD
    //
    // So: one might think that the presence of a cycle in the graph indicates infinite paths,
    // but this is not always the case: it will only trigger infinite paths from F to W if
    // at least one of the cities in the cycle can reach W! So the first thing we do is
    // determine whether a city canReachW or not: this is accomplished using a DFS
    // that works backwards from Warfare.
    //
    // After that, we just do another DFS from F and note that, in a DFS from Finance, the number of ways of reaching Warfare
    // from a city C is just the sum of the number of ways its children can reach it, assuming no
    // cycles on any path from C to W.  As with any DFS, we have a cycle if and only if we reach a city that is already on the
    // DFS stack: by ignoring cities that cannot reach W, it's trivial to decide if there is a path from C to W that contains
    // a cycle.
    //
    // As a small complication, we can't just stop the DFS at W as W may have a loop from itself, but this complication is easily
    // resolved :)
    //
    // And that's about it!
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

    setCanReachW(warfareCity);

    for (auto& city : cities)
        city.visitedInDFS = false;

    const auto numWaysFromFinancialToWarfare = findNumWaysOfReachingW(financialCity, warfareCity);
    if (numWaysFromFinancialToWarfare == -1)
        cout << "INFINITE PATHS";
    else
        cout << numWaysFromFinancialToWarfare;

    cout << '\n';
}
