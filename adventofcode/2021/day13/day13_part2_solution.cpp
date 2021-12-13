#include <iostream>
#include <regex>
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
        cout << "originalWidth: " << originalWidth << " foldAtCoord: " << foldInstruction.foldAtCoord << endl;
        assert(foldInstruction.foldAtCoord * 2 + 1 == originalWidth);
        const int foldedWidth = originalWidth / 2;
        result = vector<vector<char>>(foldedWidth, vector<char>(originalHeight, '.'));
        for (int x = 0; x < foldedWidth; x++)
        {
            for (int y = 0; y < originalHeight; y++)
            {
                cout << "x: " << x << y << " y: " << y << " originalWidth - x: " << (originalWidth - x - 1) << " originalWidth: " << originalWidth << endl;
                if (originalPaper[x][y] == '#' || originalPaper[originalWidth - x - 1][y] == '#')
                {
                    result[x][y] = '#';
                }
            }
        }
        
    }
    else if (foldInstruction.foldDirection == FoldInstruction::BottomToTop)
    {
        cout << "originalHeight: " << originalHeight << " foldAtCoord: " << foldInstruction.foldAtCoord << endl;
        assert(foldInstruction.foldAtCoord * 2 + 1 == originalHeight);
        const int foldedHeight = originalHeight / 2;
        result = vector<vector<char>>(originalWidth, vector<char>(foldedHeight, '.'));
        for (int x = 0; x < originalWidth; x++)
        {
            for (int y = 0; y < foldedHeight; y++)
            {
                cout << "x: " << x << y << " y: " << y << " originalHeight - y: " << (originalHeight - y) << " originalHeight: " << originalHeight << endl;
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
    string dotCoord;
    struct Coord
    {
        int x = -1;
        int y = -1;
    };
    vector<Coord> dotCoords;
    int width = 0;
    int height = 0;
    while (getline(cin, dotCoord))
    {
        if (dotCoord.empty())
            break;

        static regex dotCoordRegex(R"((\d+),(\d+)\s*)");
        std::smatch dotCoordMatch;
        cout << "Line: " << dotCoord << endl;
        if (regex_match(dotCoord, dotCoordMatch, dotCoordRegex))
        {
            const int x = stoi(dotCoordMatch[1]);
            const int y = stoi(dotCoordMatch[2]);

            dotCoords.push_back({x, y});

            width = max(width, x + 1);
            height = max(height, y + 1);

        }
        else
        {
            assert(false);
        }
    }
    cout << "Dots: " << endl;
    vector<FoldInstruction> foldInstructions;
    string foldInstruction;
    while (getline(cin, foldInstruction))
    {
        static regex foldInstructionRegex(R"(fold along (.)=(\d+)\s*)");
        std::smatch foldInstructionMatch;
        cout << "Line: " << foldInstruction << endl;
        if (regex_match(foldInstruction, foldInstructionMatch, foldInstructionRegex))
        {
            foldInstructions.push_back({foldInstructionMatch[1] == "x"s ? FoldInstruction::RightToLeft : FoldInstruction::BottomToTop, stoi(foldInstructionMatch[2])});
        }
        else
        {
            assert(false);
        }
    }

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
        cout << dotCoord.x << ", " << dotCoord.y << endl;
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

