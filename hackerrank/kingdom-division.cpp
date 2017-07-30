//#define EXPLORE
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cassert>

using namespace std;

struct City
{
    vector<City*> neighbours;
    int64_t numPeacefulCitiesIfBetty = -1;
    int64_t numPeacefulCitiesIfReggie = -1;
    int64_t numAlmostPeacefulCitiesIfBetty = -1;
    int64_t numAlmostPeacefulCitiesIfReggie = -1;
};

void calcPeacefulAux(vector<City>& cities, City* const rootCity, City* parentCity, const int64_t modulus)
{
    // Calculate values for children, recursing if necessary.
    vector<City*> childCities;
    childCities.reserve(rootCity->neighbours.size() - 1);
    for (const auto neighbourCity : rootCity->neighbours)
    {
        if (neighbourCity == parentCity)
            continue;

        childCities.push_back(neighbourCity);

        calcPeacefulAux(cities, neighbourCity, rootCity, modulus);
    }

    // We have everything we need from our children - calculate the values described earlier, 
    // taking modulus's frequently to avoid overflow.

    // Almost-Peacefulness.
    // Reggie.
    rootCity->numAlmostPeacefulCitiesIfReggie = 1;
    for (const auto childCity : childCities)
    {
        rootCity->numAlmostPeacefulCitiesIfReggie *= childCity->numPeacefulCitiesIfBetty;
        rootCity->numAlmostPeacefulCitiesIfReggie %= modulus;
    }
    assert(rootCity->numAlmostPeacefulCitiesIfReggie >= 0);

    // Betty.
    rootCity->numAlmostPeacefulCitiesIfBetty = 1;
    for (const auto childCity : childCities)
    {
        rootCity->numAlmostPeacefulCitiesIfBetty *= childCity->numPeacefulCitiesIfReggie;
        rootCity->numAlmostPeacefulCitiesIfBetty %= modulus;
    }
    assert(rootCity->numAlmostPeacefulCitiesIfBetty >= 0);

    // Peacefulness.
    // Reggie.
    rootCity->numPeacefulCitiesIfReggie = 0;
    if (!childCities.empty())
    {
        rootCity->numPeacefulCitiesIfReggie = 1;
        for (const auto childCity : childCities)
        {
            rootCity->numPeacefulCitiesIfReggie *= (childCity->numPeacefulCitiesIfReggie + childCity->numPeacefulCitiesIfBetty + childCity->numAlmostPeacefulCitiesIfReggie);
            rootCity->numPeacefulCitiesIfReggie %= modulus;
        }
        // We'll have counted configurations where all rootCity's children are owned by Betty: adjust for this.
        int64_t overCount = 1;
        for (const auto childCity : childCities)
        {
            overCount *= (childCity->numPeacefulCitiesIfBetty);
            overCount %= modulus;
        }
        rootCity->numPeacefulCitiesIfReggie += modulus;
        rootCity->numPeacefulCitiesIfReggie -= overCount;
    }
    assert(rootCity->numPeacefulCitiesIfReggie >= 0);
    rootCity->numPeacefulCitiesIfReggie %= modulus;


    // Betty.
    rootCity->numPeacefulCitiesIfBetty = 0;
    if (!childCities.empty())
    {
        rootCity->numPeacefulCitiesIfBetty = 1;
        for (const auto childCity : childCities)
        {
            rootCity->numPeacefulCitiesIfBetty *= (childCity->numPeacefulCitiesIfReggie + childCity->numPeacefulCitiesIfBetty + childCity->numAlmostPeacefulCitiesIfBetty);
            rootCity->numPeacefulCitiesIfBetty %= modulus;
        }
        // We'll have counted configurations where all rootCity's children are owned by Reggie: adjust for this.
        int64_t overCount = 1;
        for (const auto childCity : childCities)
        {
            overCount *= (childCity->numPeacefulCitiesIfReggie);
            overCount %= modulus;
        }
        rootCity->numPeacefulCitiesIfBetty += modulus;
        rootCity->numPeacefulCitiesIfBetty -= overCount;
    }
    rootCity->numPeacefulCitiesIfBetty %= modulus;
    assert(rootCity->numPeacefulCitiesIfBetty >= 0);
}

int64_t calcPeaceful(vector<City>& cities)
{
    const int64_t modulus = 1'000'000'007;
    City* rootCity = &(cities[0]);
    calcPeacefulAux(cities, rootCity, nullptr, modulus);

    return (rootCity->numPeacefulCitiesIfReggie + rootCity->numPeacefulCitiesIfBetty) % modulus;
}

int main()
{
    // Liked this one :)
    // Fundamentally, it's quite easy, though I made a couple of dumb mistakes - the dumbest and hardest to track down
    // was trying to read in *n* edges, rather than *n - 1* XD.  The only other one was not guarding sufficiently against
    // overflow.

    // So: we pick a city abitrarily - the first one, here - as the "root city", C, and try to compute:
    //  - The number of Peaceful kingdoms that could be formed if C were owned by Reggie; and
    //  - The number of Peaceful kingdoms that could be formed if C were owned by Betty.
    // There are probably symmetry arguments that could be used to cut this down, but this is nice and 
    // simple :)
    // If C were owned by, say, Reggie, and we had a Peaceful kingdom, what could we say about
    // C's neighbours, or rather, since we're about to do a DFS from rootCity, its children? 
    // Call C's children c1, c2, ... cm, and the sub-kingdom that has ci as its root if we do a DFS from rootCity
    // Ci.
    // If the full kingdom is peaceful, then each Ci must either:
    //  - Itself be peaceful; or
    //  - Be "almost peaceful" in the sense that it is *not* peaceful when detached from C, but *is* peaceful when attached to C.
    //
    // Additionally, at least one of the the ci must be owned by Reggie.  How many such combinations of Ci with these
    // properties are there? For a given city x, let P(x, o) be the number of Peaceful kingdoms rooted at x when x is owned by o in a DFS starting
    // at C, and AP(x,o) be the number of almost peaceful kingdoms rooted at X when x is owned by o.
    // We can make an intial stab at P(C, R) using the first two rules above as follows:
    // 
    //  P(C, R) = sum over i = 1 .. m { P(Ci, R) + P (Ci, B) + AP(Ci, R) }.
    //
    // i.e. we're multiplying all combinations of child kingdoms that give rise to a Peaceful kingdom when connected to C.
    // But in doing this, we're also counting the case where all child kingdoms are owned by Betty, which would give rise
    // to an Unpeaceful kingdom, so we have to subtract this contribution.  This contribution is:
    //
    //  sum over i = 1 .. m { P(Ci, B) }.
    // 
    // Thus we have for C, and in fact for all cities x:
    //
    //  P(C, R) = sum over i = 1 .. m { P(Ci, R) + P (Ci, B) + AP(Ci, R) } - sum over i = 1 .. m { P(Ci, B) }
    //
    // As a special case, if C has no children, then P(C, R) = P(C, B) = 0.
    // It remains to compute AP(x, o) for a given x.  This is very easy: if x has no children, it is 1; otherwise,
    // it is the number of kingdoms where each child of x is Peaceful and is owned by not-o: i.e.
    //  AP(x, o) = sum over child c of x { P(C, not-o) } if x has children, 1 otherwise.
    // And that's it!

    int n;
    cin >> n;
    vector<City> cities(n);
    for (int i = 0; i < n - 1; i++)
    {
        int u, v;
        cin >> u >> v;
        cities[u - 1].neighbours.push_back(&(cities[v - 1]));
        cities[v - 1].neighbours.push_back(&(cities[u - 1]));

    }

    cout << calcPeaceful(cities) << endl;
}
