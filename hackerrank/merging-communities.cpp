// Simon St James (ssjgz) - 2019-07-06
#include <iostream>
#include <vector>

#include <cassert>

using namespace std;

int main()
{
    // Easy one, and similar to a bunch of other challenges (Value of Friendship;
    // Journey to the Moon; Friend Circle Queries etc) - membership in a community  
    // is an equivalence relation, and so partitions the people into disjoint components.  
    // Merging two communities can be accomplished in O(lg2 N)  using the well-known 
    // Disjoint Set Union algorithm, so we can process all queries in O(Q log2 N).
    auto readInt = [](){ int x; cin >> x; assert(cin); return x; };
            
    const int numDistinctPersons = readInt();
    const int numQueries = readInt();

    struct Component
    {
        vector<int> personIdsInComponent;
    };
    // Have each person in their own component, initially.
    vector<Component> components(numDistinctPersons);
    vector<Component*> componentForPersonId(numDistinctPersons);
    for (int personId = 0; personId < numDistinctPersons; personId++)
    {
        componentForPersonId[personId] = &(components[personId]);
        components[personId].personIdsInComponent.push_back(personId);
    }

    for (int q = 0; q < numQueries; q++)
    {
        char queryType;
        cin >> queryType;

        if (queryType == 'M')
        {
            const int personId1 = readInt() - 1;
            const int personId2 = readInt() - 1;

            const auto componentPerson1 = componentForPersonId[personId1];
            const auto componentPerson2 = componentForPersonId[personId2];

            if (componentPerson1 != componentPerson2)
            {
                // Standard Disjoint Set Union - absorb the smaller component into the larger component and update.
                // This changes what looks like a O(Q ^ 2) algorithm into an O(Q log2 Q) algorithm.
                const auto absorbingComponent = (componentPerson1->personIdsInComponent.size() > componentPerson2->personIdsInComponent.size()) ? 
                    componentPerson1 : componentPerson2;
                const auto componentToAbsorb = (absorbingComponent == componentPerson1) ? componentPerson2 : componentPerson1;

                for (auto personId : componentToAbsorb->personIdsInComponent)
                {
                    componentForPersonId[personId] = absorbingComponent;
                    absorbingComponent->personIdsInComponent.push_back(personId);
                }
                componentToAbsorb->personIdsInComponent.clear();
            }
        }
        else if (queryType == 'Q')
        {
            const int personId = readInt() - 1;

            cout << componentForPersonId[personId]->personIdsInComponent.size() << endl;
        }
        else
        {
            assert(false);
        }
    }
}

