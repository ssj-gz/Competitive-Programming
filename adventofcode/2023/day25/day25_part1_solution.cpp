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
            parent = v; /* Hmmm. */
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
    int numConnectedComponents = 0;
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
    auto printGraph = [&allComponents]()
    {
        for (auto* component : allComponents)
        {
            std::cout << "Component " << component->name << " has neighbours: "<< std::endl;
            for (const auto* neighbour : component->neighbours)
            {
                std::cout << " " << neighbour->name;
            }
            std::cout << std::endl;
        }
    };

    int64_t result = -1;
    auto findConnectedComponents = [&allComponents, &printGraph, &result]()
    {
        //std::cout << "findConnectedComponents; current graph: " << std::endl;
        //printGraph();
        for (auto* component : allComponents)
        {
            component->connectedComponentId = -1;
        }
        int numComponents = 0;
        for (auto* startComponent : allComponents)
        {
            if (startComponent->connectedComponentId == -1)
            {
                //std::cout << "Exploring " << startComponent->name << std::endl;
                numComponents++;
                startComponent->connectedComponentId = numComponents;
                vector<Component*> toExplore = { startComponent };
                while (!toExplore.empty())
                {
                    //std::cout << " #toExplore: " << toExplore.size() << std::endl;
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
#if 0
        std::cout << "numComponents: " << numComponents << std::endl;
        for (auto* component : allComponents)
        {
            std::cout << " component: " << component->name << " is in connected component id: " << component->connectedComponentId << std::endl;
        }
#endif
        if (numComponents != 1)
        {
            assert(numComponents == 2);
            const int component1Size = std::count_if(allComponents.begin(), allComponents.end(), [](auto* component) { return component->connectedComponentId == 1; });
            const int component2Size = std::count_if(allComponents.begin(), allComponents.end(), [](auto* component) { return component->connectedComponentId == 2; });
            assert(result == -1);
            result = component1Size * component2Size;
        }
    };

    findConnectedComponents();

    int64_t combinationsRemaining = edges.size() * edges.size();
    for (auto edge1ToRemove = edges.begin(); edge1ToRemove != edges.end(); edge1ToRemove++)
    {
        std::cout << " edge1ToRemove " << edge1ToRemove->component1->name << "," << edge1ToRemove->component2->name  << std::endl;
        removeEdge(*edge1ToRemove);
        for (auto edge2ToRemove = std::next(edge1ToRemove); edge2ToRemove != edges.end(); edge2ToRemove++)
        {
            removeEdge(*edge2ToRemove);
            //std::cout << "Doing Tarjan with graph: " << std::endl;
            //printGraph();
            const auto bridge = findSoleBridge(allComponents);
            if (bridge.has_value())
            {
                removeEdge(bridge.value());
                findConnectedComponents();
                std::cout << "result: " << result << std::endl;
                return 0;
            }
            combinationsRemaining--;
            if (combinationsRemaining % 1000 == 0)
                cout << "combinationsRemaining: " << combinationsRemaining << std::endl;
#if 0
            for (auto edge3ToRemove = std::next(edge2ToRemove); edge3ToRemove != edges.end(); edge3ToRemove++)
            {
                removeEdge(*edge3ToRemove);
                findConnectedComponents();
                addEdge(*edge3ToRemove);
                combinationsRemaining--;
                if (combinationsRemaining % 1000 == 0)
                    cout << "combinationsRemaining: " << combinationsRemaining << std::endl;

            }

#endif
            addEdge(*edge2ToRemove);
        }
        addEdge(*edge1ToRemove);
    }
    std::cout << "result: " << result << std::endl;

    return 0;
}
