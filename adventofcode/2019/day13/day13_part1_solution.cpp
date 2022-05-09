#include "../shared/intcodecomputer.h"

#include <iostream>
#include <vector>
#include <map>

using namespace std;

int main()
{
    vector<int64_t> program;
    int64_t programInput;
    while (true)
    {
        cin >> programInput;
        program.push_back(programInput);
        assert(cin);
        char comma;
        cin >> comma;
        if (!cin)
            break;
        assert(comma == ',');
    }

    IntCodeComputer intCodeComputer(program);
    const auto status = intCodeComputer.run();
    assert(status == IntCodeComputer::Terminated);

    auto output = intCodeComputer.takeOutput();

    assert(output.size() % 3 == 0);

    struct Coord
    {
        int x = -1;
        int y = -1;
        auto operator<=>(const Coord&) const = default;
    };

    map<Coord, int> tileContents;

    while (!output.empty())
    {
        const Coord coord = {static_cast<int>(output[0]), static_cast<int>(output[1])};
        tileContents[coord] = static_cast<int>(output[2]);
        
        output.erase(output.begin(), output.begin() + 3);
    }

    int numBlocks = 0;
    for (const auto& [coord, contents] : tileContents)
    {
        cout << "Tile: " << coord.x << "," << coord.y << " = " << contents << endl;
        if (contents == 2)
            numBlocks++;
    }
    cout << "numBlocks: " << numBlocks << endl;
}

