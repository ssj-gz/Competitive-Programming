#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <array>
#include <cstdint>
#include <cassert>

using namespace std;

struct Coord
{
    int x = -1;
    int y = -1;
    auto operator<=>(const Coord& other) const = default;
};

class KeySet
{
    public:
        KeySet& addKey(char keyChar)
        {
            assert(!hasKey(keyChar));
            m_keyBits |= keyBitForKey(keyChar);
            return *this;
        };
        bool hasKey(char keyChar) const
        {
            return m_keyBits & keyBitForKey(keyChar);
        }
        bool canOpenDoor(char doorChar) const
        {
            assert('A' <= doorChar && doorChar <= 'Z');
            const char correspondingKeyChar = doorChar - 'A' + 'a';
            return hasKey(correspondingKeyChar);
        }
        auto operator<=>(const KeySet& other) const = default;
        string toString() const
        {
            string asStr;
            for (char key = 'a'; key <= 'z'; key++)
            {
                if (hasKey(key))
                    asStr += key;
            }
            return asStr;
        }
    private:
        uint32_t m_keyBits = 0;

        uint32_t keyBitForKey(char keyChar) const
        {
            assert('a' <= keyChar && keyChar <= 'z');
            uint32_t keyBit = static_cast<uint32_t>(1) << static_cast<uint32_t>(keyChar - 'a');
            return keyBit;
        }
};

ostream& operator<<(ostream& os, const KeySet& keySet)
{
    os << keySet.toString();
    return os;
}

struct NewKeyEvent
{
    KeySet keySet;
    std::array<Coord, 4> robotPositions;
    auto operator<=>(const NewKeyEvent& other) const = default;
};

vector<std::pair<int, NewKeyEvent>> getNewKeyEvents(const KeySet& startingKeySet, const std::array<Coord, 4> robotPositions, int robotIndex, const vector<string>& vaultMapOrig)
{
    auto vaultMap = vaultMapOrig;
    const int vaultHeight = vaultMap.size();
    const int vaultWidth = vaultMap.front().size();
    vector<std::pair<int, NewKeyEvent>> newKeyEvents;
    std::vector<Coord> toExplore = {robotPositions[robotIndex]};

    int time = 1;
    while (!toExplore.empty())
    {
        //std::cout << " # toExplore: " << toExplore.size() << std::endl;
        std::vector<Coord> nextToExplore;
        for (const auto coord : toExplore)
        {
            //std::cout << "  coord: " << coord.x << "," << coord.y << std::endl;
            const int directions[][2] = { {-1, 0}, { 1, 0}, { 0, -1}, {0, 1} };
            for (const auto dxdy : directions )
            {
                const Coord neighbour = { coord.x + dxdy[0], coord.y + dxdy[1] };
                //std::cout << "   neighbour: " << neighbour.x  << "," << neighbour.y << std::endl;
                if (neighbour.x < 0 || neighbour.x >= vaultWidth)
                    continue;
                if (neighbour.y < 0 || neighbour.y >= vaultHeight)
                    continue;
                auto& cellContents = vaultMap[neighbour.y][neighbour.x];
                //std::cout << "    cell: " << cellContents << std::endl;
                if (cellContents == '#')
                    continue;
                else if ('a' <= cellContents && cellContents <= 'z')
                {
                    if (!startingKeySet.hasKey(cellContents))
                    {
                        std::cout << "    Got new NewKeyEvent! - new key: " << cellContents << " @time: " << time << std::endl;
                        const KeySet newKeySet = KeySet(startingKeySet).addKey(cellContents);
                        auto newRobotPositions = robotPositions;
                        newRobotPositions[robotIndex] = neighbour;
                        newKeyEvents.push_back({time, {newKeySet, newRobotPositions}});
                    }
                    else
                    {
                        nextToExplore.push_back(neighbour);
                    }
                }
                else if ('A' <= cellContents && cellContents <= 'Z')
                {
                    if (startingKeySet.canOpenDoor(cellContents))
                    {
                        //std::cout << "    opening door: " << cellContents << std::endl;
                        nextToExplore.push_back(neighbour);
                    }
                }
                else if (cellContents == '.')
                {
                    nextToExplore.push_back(neighbour);
                }
                else
                {
                    assert(false);
                }
                cellContents = '#'; // Prevent re-visiting.
            }
        }

        toExplore = nextToExplore;
        time++;
    }

    return newKeyEvents;
}

int main()
{
    vector<string> vaultMap;
    string vaultMapRow;
    while (std::getline(cin, vaultMapRow))
    {
        vaultMap.push_back(vaultMapRow);
    }
    for (const auto& vaultMapRow : vaultMap)
    {
        std::cout << vaultMapRow << std::endl;
    }

    std::array<Coord, 4> robotStartCoords;
    const int vaultHeight = vaultMap.size();
    const int vaultWidth = vaultMap.front().size();
    KeySet allKeys;
    for (int y = 0; y < vaultHeight; y++)
    {
        for (int x = 0; x < vaultWidth; x++)
        {
            auto& cellContents = vaultMap[y][x];
            if (cellContents == '@')
            {
                cellContents = '#';
                vaultMap[y - 1][x] = '#';
                vaultMap[y + 1][x] = '#';
                vaultMap[y][x - 1] = '#';
                vaultMap[y][x + 1] = '#';
                robotStartCoords[0] = { x - 1, y - 1 };
                robotStartCoords[1] = { x + 1, y - 1 };
                robotStartCoords[2] = { x + 1, y + 1 };
                robotStartCoords[3] = { x - 1, y + 1 };

            }
            else if ('a' <= cellContents && cellContents <= 'z')
            {
                allKeys.addKey(cellContents);
            }
        }
    }
    for (int i = 0; i < 4; i++)
    {
        std::cout << "robot startCoord: " << robotStartCoords[i].x << "," << robotStartCoords[i].y << std::endl;
    }
    std::cout << "allKeys: " << allKeys << std::endl;

    KeySet keySet;
    std::map<int, set<NewKeyEvent>> keyEventsForTime;
    keyEventsForTime[0] = { { keySet, robotStartCoords } };
    map<NewKeyEvent, int> earliestOccurenceOfKeyEvent;

    auto keyEventTimeIter = keyEventsForTime.begin();
    int quickestTime = -1;
    while (keyEventTimeIter != keyEventsForTime.end() && quickestTime == -1)
    {
        const auto& [currentTime, keyEventsAtTime ] = *keyEventTimeIter;
#if 0
        set<string> blah;
        for (const auto keyEvent : keyEventsAtTime)
        {
            KeySet keySet = keyEvent.previousKeySet;
            keySet.addKey(keyEvent.newKey);
            blah.insert(keySet.toString());
        }
        std::cout << "currentTime: " << currentTime << " # blah: " << blah.size() << std::endl;
#endif
        for (const auto keyEvent : keyEventsAtTime)
        {
            KeySet keySet = keyEvent.keySet;
            std::cout << "keySet: " << keySet << " currentTime: " << currentTime << " # events at time: " << keyEventsAtTime.size() << std::endl;
            if (keySet == allKeys)
            {
                std::cout << "woohoo - " << currentTime << std::endl;
                quickestTime = currentTime;
                break;
            }
            for (int robotIndex = 0; robotIndex < 4; robotIndex++)
            {
                const auto newKeyEvents = getNewKeyEvents(keySet, keyEvent.robotPositions, robotIndex, vaultMap);
                for (const auto& [timeOfAcquisition, newKeyEvent] : newKeyEvents)
                {
                    assert(timeOfAcquisition != 0);
                    bool add = true;
                    if (!earliestOccurenceOfKeyEvent.contains(newKeyEvent))
                    {
                        earliestOccurenceOfKeyEvent[newKeyEvent] = currentTime + timeOfAcquisition;
                    }
                    else
                    {
                        if (earliestOccurenceOfKeyEvent[newKeyEvent] > currentTime + timeOfAcquisition)
                            earliestOccurenceOfKeyEvent[newKeyEvent] = currentTime + timeOfAcquisition;
                        else
                            add = false;
                    }
                    if (add)
                        keyEventsForTime[timeOfAcquisition + currentTime].insert(newKeyEvent);
                }
            }
        }

        keyEventTimeIter++;
    }
    std::cout << "quickestTime: " << quickestTime << std::endl;
}
