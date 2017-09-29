// Simon St James (ssjgz) 2017-09-29 14:20
#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <cassert>

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

    int64_t numPairsWithCost[10] = {};

    // We use char as bool is *vastly* slower.
    vector<vector<char>> isJunctionIndexReachableFromRootWithCost(10, vector<char>(n));


    for (int rootNodeIndex = 0; rootNodeIndex < n; rootNodeIndex++)
    {
        if (junctions[rootNodeIndex].reached)
            continue;
        Junction* rootJunction = &(junctions[rootNodeIndex]);
        //cout << "rootJunction: " << rootJunction << endl;

        int64_t numJunctionsReachableFromRootWithCost[10] = {};

        //auto begin = chrono::high_resolution_clock::now();    
        //auto end = chrono::high_resolution_clock::now();    
        //auto dur = end - begin;
        //auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
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
                numJunctionsReachableFromRootWithCost[cost]++;

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

        //const auto countTrue = [](const vector<char>& bools)
        //{
            //int64_t numTrue = 0;
            //for (const auto boolValue : bools)
            //{
                //if (boolValue)
                    //numTrue++;
            //}
            //return numTrue;
        //};


#if 0
        for (int cost = 0; cost <= 9; cost++)
        {
            cout << "cost: " << cost << " reachable node indices: (#" << countTrue(isJunctionIndexReachableFromRootWithCost[cost]) << ")" << endl;
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
            //cout << "Blee:" << countTrue(isJunctionIndexReachableFromRootWithCost[cost]) << " blaa: " << numJunctionsReachableFromRootWithCost[cost] << endl;
            //assert(countTrue(isJunctionIndexReachableFromRootWithCost[cost]) == numJunctionsReachableFromRootWithCost[cost]);
            if (isJunctionIndexReachableFromRootWithCost[cost][rootJunction->index])
            {
                rootNodeSelfLoopCosts.push_back(cost);
            }
        }

        //cout << "Main bit" << endl;
        for (int cost = 0; cost <= 9; cost++)
        {
            vector<int> processedXCosts;
            int xCost = 0;
            int yCost = xCost + cost;
            const bool junctionsReachableWithXAndYCostAreEqual = (find(rootNodeSelfLoopCosts.begin(), rootNodeSelfLoopCosts.end(), cost) != rootNodeSelfLoopCosts.end());

            int64_t numPairs = 0;
            bool alreadyHandledEquvialent = false;
            while (xCost <= 9)
            {
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
                    numPairs += numJunctionsReachableFromRootWithCost[yCost] * numJunctionsReachableFromRootWithCost[xCost];
                    //int overCount = 0;
                    //for (int nodeIndex = 0; nodeIndex < n; nodeIndex++)
                    //{
                        //if (isJunctionIndexReachableFromRootWithCost[yCost][nodeIndex] && isJunctionIndexReachableFromRootWithCost[xCost][nodeIndex])
                            //overCount++;
                    //}
                    //assert(overCount == (junctionsReachableWithXAndYCostAreEqual ? numJunctionsReachableFromRootWithCost[xCost] : 0));
                    const int64_t overCount = (junctionsReachableWithXAndYCostAreEqual ? numJunctionsReachableFromRootWithCost[xCost] : 0);
                    numPairs -= overCount;

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
