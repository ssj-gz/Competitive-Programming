#include <iostream>
#include <regex>
#include <ranges>
#include <cassert>

using namespace std;

struct FoldInstruction
{
    enum {RightToLeft, BottomToTop} foldDirection;
    int foldAtCoord = -1;
};

vector<vector<char>> getFolded(const vector<vector<char>>& originalPaper,  FoldInstruction foldInstruction)
{
    const int originalWidth = originalPaper.size();
    const int originalHeight = originalPaper.front().size();
    vector<vector<char>> result;
    if (foldInstruction.foldDirection == FoldInstruction::RightToLeft)
    {
        assert(foldInstruction.foldAtCoord * 2 + 1 == originalWidth);
        const int foldedWidth = originalWidth / 2;
        result = vector<vector<char>>(foldedWidth, vector<char>(originalHeight, '.'));
        for (int x = 0; x < foldedWidth; x++)
        {
            for (int y = 0; y < originalHeight; y++)
            {
                if (originalPaper[x][y] == '#' || originalPaper[originalWidth - x - 1][y] == '#')
                {
                    result[x][y] = '#';
                }
            }
        }
        
    }
    else if (foldInstruction.foldDirection == FoldInstruction::BottomToTop)
    {
        assert(foldInstruction.foldAtCoord * 2 + 1 == originalHeight);
        const int foldedHeight = originalHeight / 2;
        result = vector<vector<char>>(originalWidth, vector<char>(foldedHeight, '.'));
        for (int x = 0; x < originalWidth; x++)
        {
            for (int y = 0; y < foldedHeight; y++)
            {
                if (originalPaper[x][y] == '#' || originalPaper[x][originalHeight - y - 1] == '#')
                {
                    result[x][y] = '#';
                }
            }
        }
    }
    return result;
}

int main()
{
    struct Coord
    {
        int x = -1;
        int y = -1;
    };
    vector<Coord> dotCoords;
    string dotCoordLine;
    while (getline(cin, dotCoordLine))
    {
        static regex dotCoordRegex(R"((\d+),(\d+)\s*)");
        std::smatch dotCoordMatch;
        if (regex_match(dotCoordLine, dotCoordMatch, dotCoordRegex))
        {
            const int x = stoi(dotCoordMatch[1]);
            const int y = stoi(dotCoordMatch[2]);

            dotCoords.push_back({x, y});
        }
        else
        {
            break;
        }
    }

    vector<FoldInstruction> foldInstructions;
    string foldInstructionLine;
    while (getline(cin, foldInstructionLine))
    {
        static regex foldInstructionRegex(R"(fold along (.)=(\d+)\s*)");
        std::smatch foldInstructionMatch;
        if (regex_match(foldInstructionLine, foldInstructionMatch, foldInstructionRegex))
        {
            const auto foldDirection = foldInstructionMatch[1] == "x"s ? FoldInstruction::RightToLeft : FoldInstruction::BottomToTop;
            const int foldAtCoord = stoi(foldInstructionMatch[2]);
            foldInstructions.push_back({foldDirection, foldAtCoord});
        }
        else
        {
            assert(false);
        }
    }
    const int height = 1 + 2 * std::ranges::max(foldInstructions 
            | std::views::filter([](const FoldInstruction& foldInstruction) { return foldInstruction.foldDirection == FoldInstruction::BottomToTop; })
            | std::views::transform([](const FoldInstruction& foldInstruction) { return foldInstruction.foldAtCoord; }));
    const int width = 1 + 2 * std::ranges::max(foldInstructions 
            | std::views::filter([](const FoldInstruction& foldInstruction) { return foldInstruction.foldDirection == FoldInstruction::RightToLeft; })
            | std::views::transform([](const FoldInstruction& foldInstruction) { return foldInstruction.foldAtCoord; }));

    cout << "width: " << width << " height: " << height << endl;


    vector<vector<char>> paper(width, vector<char>(height, '.'));
    auto printPaper = [](const vector<vector<char>>& paper)
    {
        const int width = paper.size();
        const int height = paper.front().size();
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                cout << paper[x][y];
            }
            cout << endl;
        }
    };

    for (const auto dotCoord : dotCoords)
    {
        paper[dotCoord.x][dotCoord.y] = '#';
    }
    printPaper(paper);

    for (const auto& foldInstruction : foldInstructions)
    {
        cout << "After fold along " << (foldInstruction.foldDirection == FoldInstruction::RightToLeft ? "y" : "x") << " = " << foldInstruction.foldAtCoord << endl;
        paper = getFolded(paper, foldInstruction);
        printPaper(paper);
    }

}

