#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>

#include <cassert>

using namespace std;

struct Port
{
    int pins[2] = {};
    bool used = false;
};

void buildBridge(int nextNeededPin, int64_t currentStrength, map<int, vector<Port*>>& portsWithPinsLookup, int64_t& highestStrength)
{
    if (currentStrength > highestStrength)
    {
        highestStrength = currentStrength;
        std::cout << "new highest strength: " << highestStrength << std::endl;
    }
    const auto& nextPorts = portsWithPinsLookup[nextNeededPin];
    for (auto* nextPort : nextPorts)
    {
        if (nextPort->used)
            continue;

        const int portStrength = nextPort->pins[0] + nextPort->pins[1];
        int otherPin = -1;
        if (nextPort->pins[0] == nextNeededPin)
            otherPin = nextPort->pins[1];
        else if (nextPort->pins[1] == nextNeededPin)
            otherPin = nextPort->pins[0];
        else
            assert(false);

        nextPort->used = true;
        buildBridge(otherPin, currentStrength + portStrength, portsWithPinsLookup, highestStrength);
        nextPort->used = false;
    }

}

int main()
{
    string portDescrLine;
    vector<Port> ports;
    while (getline(cin, portDescrLine))
    {
        std::replace(portDescrLine.begin(), portDescrLine.end(), '/', ' ');
        istringstream portDescrStream(portDescrLine);;

        Port port;
        portDescrStream >> port.pins[0];
        portDescrStream >> port.pins[1];
        assert(portDescrStream);

        ports.push_back(port);
    }
    map<int, vector<Port*>> portsWithPins;

    for (auto& port : ports)
    {
        portsWithPins[port.pins[0]].push_back(&port);
        portsWithPins[port.pins[1]].push_back(&port);
    }

    int64_t highestStrength = -1;
    buildBridge(0, 0, portsWithPins, highestStrength);
    std::cout << "highestStrength: " << highestStrength << std::endl;

    return 0;
}
