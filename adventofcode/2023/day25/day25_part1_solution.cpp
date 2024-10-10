#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <optional>

#include <cassert>

using namespace std;

struct Component
{
    string name;
    vector<Component*> neighbours;
    int connectedComponentId = -1;
    // Bridge-finding stuff.
    int disc = -1;
    int low = -1;
    bool visited = false;
};
struct Edge
{
    Component *component1 = nullptr;
    Component *component2 = nullptr;
    auto operator<=>(const Edge& other) const = default;
};

void bridgeAux(Component* v, Component* parent, int& time, vector<Edge>& bridges)
{
    v->visited = true;
    time++;
    v->low = time;
    v->disc = time;

    for (auto* neighbour : v->neighbours)
    {
        if (neighbour == parent)
            continue;

        if (neighbour->visited)
        {
            v->low = std::min(v->low, neighbour->disc);
        }
        else
        {
            parent = v; /* Hmmm.  Looks dodgy, but seems to work. */
            bridgeAux(neighbour, parent, time, bridges); 
            v->low = min(v->low, neighbour->low);
            if (neighbour->low > v->disc)
            {
                std::cout << " found bridge: " << neighbour->name << " - " << v->name << std::endl;
                bridges.push_back({neighbour, v});
            }
        }
    }

}

std::optional<Edge> findSoleBridge(const vector<Component*>& allComponents)
{
    // This, and bridgeAux, adapted from https://www.geeksforgeeks.org/bridge-in-a-graph/.
    for (auto* component : allComponents)
    {
        component->visited = false;
        component->disc = -1;
        component->low = -1;
    }
    int time = 0;
    vector<Edge> bridges;

    for (auto* component : allComponents)
    {
        if (!component->visited)
            bridgeAux(component, nullptr, time, bridges);
    }
    assert(bridges.size() == 0 || bridges.size() == 1);
    if (bridges.size() == 1)
    {
        return bridges.front();
    }
    else
        return std::optional<Edge>();
}

int main()
{
    map<string, Component> componentForName;
    string wiringDiagramLine;
    vector<Edge> edges;
    while (getline(cin, wiringDiagramLine))
    {
        istringstream wiringDiagramIn(wiringDiagramLine);
        string sourceComponentName;
        wiringDiagramIn >> sourceComponentName;
        assert(sourceComponentName.back() == ':');
        sourceComponentName.pop_back();
        Component& sourceComponent = componentForName[sourceComponentName];
        sourceComponent.name = sourceComponentName;

        string neighbourComponentName;
        while (wiringDiagramIn >> neighbourComponentName)
        {
            Component& neighbourComponent = componentForName[neighbourComponentName];
            neighbourComponent.name = neighbourComponentName;

            sourceComponent.neighbours.push_back(&neighbourComponent);
            neighbourComponent.neighbours.push_back(&sourceComponent);

            std::cout << " added edge between " << sourceComponent.name << " and " << neighbourComponent.name << std::endl;
            Edge edge = {&sourceComponent, &neighbourComponent};
            if (edge.component1->name > edge.component2->name)
            {
                // Normalise.
                swap(edge.component1, edge.component2);
            }

            edges.push_back(edge);

        }
    }
    std::cout << "# edges before duplicate removal: " << edges.size() << std::endl;
    std::sort(edges.begin(), edges.end());
    edges.erase(std::unique(edges.begin(), edges.end()), edges.end());
    std::cout << "# edges after duplicate removal: " << edges.size() << std::endl;

    auto removeEdge = [](const auto& edgeToRemove)
    {
        auto& component1Neighbours = edgeToRemove.component1->neighbours;
        component1Neighbours.erase(std::find(component1Neighbours.begin(), component1Neighbours.end(), edgeToRemove.component2));
        auto& component2Neighbours = edgeToRemove.component2->neighbours;
        component2Neighbours.erase(std::find(component2Neighbours.begin(), component2Neighbours.end(), edgeToRemove.component1));
    };
    auto addEdge = [](const auto& edgeToRemove)
    {
        auto& component1Neighbours = edgeToRemove.component1->neighbours;
        component1Neighbours.push_back(edgeToRemove.component2);
        auto& component2Neighbours = edgeToRemove.component2->neighbours;
        component2Neighbours.push_back(edgeToRemove.component1);
    };

    vector<Component*> allComponents;
    for (auto& [name, component] : componentForName)
    {
        allComponents.push_back(&component);
    }
    for (auto* component : allComponents)
    {
        sort(component->neighbours.begin(), component->neighbours.end());
        component->neighbours.erase(std::unique(component->neighbours.begin(), component->neighbours.end()), component->neighbours.end());
    }

    auto computeComponentSizeProduct = [&allComponents]()
    {
        // Should only be called when we know the graph has been split into
        // exactly two components.
        for (auto* component : allComponents)
        {
            component->connectedComponentId = -1;
        }
        int numComponents = 0;
        for (auto* startComponent : allComponents)
        {
            if (startComponent->connectedComponentId == -1)
            {
                numComponents++;
                startComponent->connectedComponentId = numComponents;
                vector<Component*> toExplore = { startComponent };
                while (!toExplore.empty())
                {
                    vector<Component*> nextToExplore;
                    for (auto* component : toExplore)
                    {
                        for (auto* neighbourComponent : component->neighbours)
                        {
                            if (neighbourComponent->connectedComponentId == -1)
                            {
                                neighbourComponent->connectedComponentId = numComponents;
                                nextToExplore.push_back(neighbourComponent);
                            }
                        }
                    }

                    toExplore = nextToExplore;
                }
            }
        }
        assert(numComponents == 2);
        const int component1Size = std::count_if(allComponents.begin(), allComponents.end(), [](auto* component) { return component->connectedComponentId == 1; });
        const int component2Size = std::count_if(allComponents.begin(), allComponents.end(), [](auto* component) { return component->connectedComponentId == 2; });
        return component1Size * component2Size;
    };

    int64_t combinationsRemaining = edges.size() * edges.size();
    for (auto edge1ToRemove = edges.begin(); edge1ToRemove != edges.end(); edge1ToRemove++)
    {
        removeEdge(*edge1ToRemove);
        for (auto edge2ToRemove = std::next(edge1ToRemove); edge2ToRemove != edges.end(); edge2ToRemove++)
        {
            removeEdge(*edge2ToRemove);
            const auto bridge = findSoleBridge(allComponents);
            if (bridge.has_value())
            {
                removeEdge(bridge.value());
                const auto result = computeComponentSizeProduct();
                std::cout << "result: " << result << std::endl;
                return 0;
            }
            combinationsRemaining--;
            if (combinationsRemaining % 1000 == 0)
                cout << "combinationsRemaining: " << combinationsRemaining << std::endl;
            addEdge(*edge2ToRemove);
        }
        addEdge(*edge1ToRemove);
    }

    return 0;
}
