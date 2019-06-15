// Simon St James (ssjgz) - 2019-06-05
#include <iostream>
#include <vector>
#include <map>
#include <utility>

using namespace std;

int main()
{
    // Easy one, and similar to a bunch of other challenges (Value of Friendship;
    // Journey to the Moon; etc) - friendship is an equivalence relation, and so
    // partitions the people into disjoint components.  Introducing a friendship
    // between people in difference components merges the two components.
    // We can do this merging efficiently using the well-known Disjoint Set Union
    // algorithm to process all queries in O(Q log2 Q).
    int Q;
    cin >> Q;

    struct Query
    {
        int person1 = -1;
        int person2 = -1;
    };

    vector<Query> queries(Q);
    for (int q = 0; q < Q; q++)
    {
        cin >> queries[q].person1;
        cin >> queries[q].person2;
    }

    map<int, int> originalToSquashedPersonIds;
    auto getSquashedPersonId = [&originalToSquashedPersonIds](int originalPerson)
    {
        if (originalToSquashedPersonIds.find(originalPerson) == originalToSquashedPersonIds.end())
        {
            const int newSquashedId = originalToSquashedPersonIds.size();
            originalToSquashedPersonIds[originalPerson] = newSquashedId;
            return newSquashedId;
        }
        else
        {
            return originalToSquashedPersonIds[originalPerson];
        }
    };

    // Re-map the original person ids to the consecutive "squashed" ids 0, 1, ...
    // This might seem like a micro-optimisation, but it should knock off a O(log2 Q)
    // asymptotically.
    for (auto& query : queries)
    {
        query.person1 = getSquashedPersonId(query.person1);
        query.person2 = getSquashedPersonId(query.person2);
    }

    struct Component
    {
        vector<int> squashedPersonIdsInComponent;
    };

    // Have each person in their own component, initially.
    const int numDistinctPersons = originalToSquashedPersonIds.size();
    vector<Component> components(numDistinctPersons);
    vector<Component*> componentForSquashedPersonId(numDistinctPersons);
    for (int squashedId = 0; squashedId < numDistinctPersons; squashedId++)
    {
        componentForSquashedPersonId[squashedId] = &(components[squashedId]);
        components[squashedId].squashedPersonIdsInComponent.push_back(squashedId);
    }

    int largestComponentSize = 1;
    for (const auto& query : queries)
    {
        const auto componentPerson1 = componentForSquashedPersonId[query.person1];
        const auto componentPerson2 = componentForSquashedPersonId[query.person2];

        if (componentPerson1 != componentPerson2)
        {
            // Standard Disjoint Set Union - absorb the smaller component into the larger component and update.
            // This changes what looks like a O(Q ^ 2) algorithm into an O(Q log2 Q) algorithm.
            const auto absorbingComponent = (componentPerson1->squashedPersonIdsInComponent.size() > componentPerson2->squashedPersonIdsInComponent.size()) ? 
                componentPerson1 : componentPerson2;
            const auto componentToAbsorb = (absorbingComponent == componentPerson1) ? componentPerson2 : componentPerson1;

            for (auto squashedPersonId : componentToAbsorb->squashedPersonIdsInComponent)
            {
                componentForSquashedPersonId[squashedPersonId] = absorbingComponent;
                absorbingComponent->squashedPersonIdsInComponent.push_back(squashedPersonId);
            }
            componentToAbsorb->squashedPersonIdsInComponent.clear();


            largestComponentSize = std::max(largestComponentSize, static_cast<int>(absorbingComponent->squashedPersonIdsInComponent.size()));
        }

        cout << largestComponentSize << endl;
    }
}


