// Simon St James (ssjgz) - 2017-09-16 09:21
#define SUBMISSION
#ifndef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <queue>
#include <cassert>

using namespace std;

struct Astronaut;
struct Edge
{
    Edge() = default;
    Edge(Astronaut* astronautA, Astronaut *astronautB)
        : astronautA(astronautA), astronautB(astronautB)
    {

    }
    Astronaut *astronautA = nullptr;
    Astronaut *astronautB = nullptr;
};
struct Astronaut
{
    vector<Edge*> edges;
    int componentNum = -1;
};

int main() {
    int N, P;
    cin >> N >> P;
    vector<Astronaut> allAstronaut(N);
    vector<Edge> allEdges(P);
    for (int i = 0; i < P; i++)
    {
        int astronautAIndex, astronautBIndex;
        cin >> astronautAIndex >> astronautBIndex;

        Astronaut *astronautA = &(allAstronaut[astronautAIndex]);
        Astronaut *astronautB = &(allAstronaut[astronautBIndex]);

        allEdges[i] = Edge(astronautA, astronautB);

        astronautA->edges.push_back(&(allEdges[i]));
        astronautB->edges.push_back(&(allEdges[i]));
    }

    vector<vector<Astronaut*>> astronautsInComponent;

    for (const auto& edge : allEdges)
    {
        Astronaut *astronautA = edge.astronautA;
        Astronaut *astronautB = edge.astronautB;

        if (astronautA->componentNum == -1 && astronautB->componentNum == -1)
        {
            // Join astronauts, and create brand-new component.
            astronautsInComponent.push_back({astronautA, astronautB});
            astronautA->componentNum = astronautsInComponent.size() - 1;
            astronautB->componentNum = astronautsInComponent.size() - 1;
            continue;
        }
        if (astronautA->componentNum == astronautB->componentNum)
        {
            continue;
        }
        // Have two astronauts, at least one of which is in a component, and they are
        // not in the same component.
        int absorbingComponent = -1;
        int absorbedComponent = -1;
        Astronaut *absorbedAstronaut = nullptr;
        // If either Astronaut has no component, then it is absorbed into the other astronauts' component; otherwise, the larger
        // component absorbs the smaller.
        if (astronautA->componentNum == -1 || (astronautB->componentNum != -1 && astronautsInComponent[astronautA->componentNum].size() < astronautsInComponent[astronautB->componentNum].size() ))
        {
            absorbingComponent = astronautB->componentNum;
            absorbedComponent = astronautA->componentNum;
            absorbedAstronaut = astronautA;
        }
        else
        {
            absorbingComponent = astronautA->componentNum;
            absorbedComponent = astronautB->componentNum;
            absorbedAstronaut = astronautB;
        }
        assert(absorbingComponent != -1);
        if (absorbedComponent != -1)
        {
            for (auto astronautToAbsorb : astronautsInComponent[absorbedComponent])
            {
                astronautToAbsorb->componentNum = absorbingComponent;
                astronautsInComponent[absorbingComponent].push_back(astronautToAbsorb);
            }
            astronautsInComponent[absorbedComponent].clear();
        }
        else
        {
            absorbedAstronaut->componentNum = absorbingComponent;
            astronautsInComponent[absorbingComponent].push_back(absorbedAstronaut);
        }
    }

    // Pick up the astronauts that are each in their own country.
    for (auto& astronaut : allAstronaut)
    {
        if (astronaut.componentNum == -1)
            astronautsInComponent.push_back({&astronaut});
    }

    auto numPairs = [](int64_t numAstronauts)
    {
        return numAstronauts * (numAstronauts - 1) / 2;
    };
    const int numCountries = astronautsInComponent.size();
    const int64_t numPairsTotal = numPairs(N);
    int64_t numPairsDifferentCountry = 0;

    if (numCountries > 0)
    {
        numPairsDifferentCountry = numPairsTotal;
        for (const auto& astronautsInSameCountry : astronautsInComponent)
        {
            const int numAstronautsInThisCountry = astronautsInSameCountry.size();
            const int64_t numPairsFromThisCountry  = numPairs(numAstronautsInThisCountry);
            numPairsDifferentCountry -= numPairsFromThisCountry;
        }
    }

    cout << numPairsDifferentCountry << endl;

    return 0;
}

