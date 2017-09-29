// Simon St James (ssjgz) 2017-09-29 14:20
#include <iostream>
#include <vector>
#include <chrono>

using namespace std;

int main()
{
    struct Junction;
    struct Road
    {
        Junction* junction;
        int cost = 0;
    };
    struct Junction
    {
        vector<Road> neighbours;
        int index = 0;
        bool reached = false;
    };

    int n, e;
    cin >> n >> e;

    vector<Junction> junctions(n);
    for (int i = 0; i < n; i++)
    {
        junctions[i].index = i;
    }

    for (int i = 0; i < e; i++)
    {
        int x, y, r;
        cin >> x >> y >> r;
        x--;
        y--;
        r %= 10;

        Road roadXY;
        roadXY.cost = r;
        roadXY.junction = &(junctions[y]);
        junctions[x].neighbours.push_back(roadXY);

        Road roadYX;
        roadYX.junction = &(junctions[x]);
        roadYX.cost = (10 - r ) % 10;
        junctions[y].neighbours.push_back(roadYX);
    }

    int numPairsWithCost[10] = {};

    for (int rootNodeIndex = 0; rootNodeIndex < n; rootNodeIndex++)
    {
        if (junctions[rootNodeIndex].reached)
            continue;
        Junction* rootJunction = &(junctions[rootNodeIndex]);
        //cout << "rootJunction index: " << rootJunction->index << endl;

        auto begin = chrono::high_resolution_clock::now();    
        // We use char as bool is *vastly* slower.
        vector<vector<char>> isJunctionIndexReachableFromRootWithCost(10, vector<char>(n));
        auto end = chrono::high_resolution_clock::now();    
        auto dur = end - begin;
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
        //cout << "allocating took: " << ms << "ms" <<  endl;

        struct JunctionAndCost
        {
            Junction *junction = nullptr;
            int cost = 0;
        };

        JunctionAndCost rootJunctionAndCost;
        rootJunctionAndCost.junction = rootJunction;

        vector<JunctionAndCost> junctionsAndCostsToExplore = { rootJunctionAndCost };

        while (!junctionsAndCostsToExplore.empty())
        {
            vector<JunctionAndCost> nextJunctionsAndCostsToExplore;
            for (const auto& junctionAndCost : junctionsAndCostsToExplore)
            {
                auto junction = junctionAndCost.junction;
                const auto cost = junctionAndCost.cost;
                if (isJunctionIndexReachableFromRootWithCost[junctionAndCost.cost][junction->index])
                    continue;
                isJunctionIndexReachableFromRootWithCost[junctionAndCost.cost][junction->index] = true;
                junction->reached = true;

                for (const auto& road : junction->neighbours)
                {
                    auto neighbourJunction = road.junction;
                    const auto costToReachNeighbour = (cost + road.cost) % 10;
                    if (!isJunctionIndexReachableFromRootWithCost[costToReachNeighbour][neighbourJunction->index])
                    {
                        JunctionAndCost neighbourJunctionAndCost;
                        neighbourJunctionAndCost.junction = neighbourJunction;
                        neighbourJunctionAndCost.cost = costToReachNeighbour;
                        nextJunctionsAndCostsToExplore.push_back(neighbourJunctionAndCost);
                    }
                }
            }
            junctionsAndCostsToExplore = nextJunctionsAndCostsToExplore;
        }

#if 0
        for (int cost = 0; cost <= 9; cost++)
        {
            cout << "cost: " << cost << " reachable node indices:" << endl;
            for (int nodeIndex = 0; nodeIndex < n; nodeIndex++)
            {
                if (isJunctionIndexReachableFromRootWithCost[cost][nodeIndex])
                    cout << nodeIndex << " ";

            }
            cout << endl;
        }
#endif

        vector<int> rootNodeSelfLoopCosts;
        for (int cost = 0; cost <= 9; cost++)
        {
            if (isJunctionIndexReachableFromRootWithCost[cost][rootJunction->index])
            {
                rootNodeSelfLoopCosts.push_back(cost);
            }
        }

        const auto countTrue = [](const vector<char>& bools)
        {
            int numTrue = 0;
            for (const auto boolValue : bools)
            {
                if (boolValue)
                    numTrue++;
            }
            return numTrue;
        };

        //cout << "Main bit" << endl;
        for (int cost = 0; cost <= 9; cost++)
        {
            //cout << "cost: " << cost << endl;
            vector<int> processedXCosts;
            int xCost = 0;
            int yCost = xCost + cost;

            int64_t numPairs = 0;
            bool alreadyHandledEquvialent = false;
            while (xCost <= 9)
            {
                //cout << " xCost" << xCost << endl;
                for (const auto processedXCost : processedXCosts)
                {
                    for (const auto rootNodeSelfLoopCost : rootNodeSelfLoopCosts)
                    {
                        if ((processedXCost + rootNodeSelfLoopCost) % 10 == xCost)
                        {
                            alreadyHandledEquvialent = true;
                        }
                    }
                }
                if (!alreadyHandledEquvialent)
                {
                    numPairs += countTrue(isJunctionIndexReachableFromRootWithCost[yCost]) * countTrue(isJunctionIndexReachableFromRootWithCost[xCost]);
                    for (int nodeIndex = 0; nodeIndex < n; nodeIndex++)
                    {
                        if (isJunctionIndexReachableFromRootWithCost[yCost][nodeIndex] && isJunctionIndexReachableFromRootWithCost[xCost][nodeIndex])
                            numPairs--;
                    }

                }
                processedXCosts.push_back(xCost);
                xCost++;
                yCost = (yCost + 1) % 10;
            }
            numPairsWithCost[cost] += numPairs;
        }
    }
    for (const auto numPairs : numPairsWithCost)
    {
        cout << numPairs << endl;
    }
}
