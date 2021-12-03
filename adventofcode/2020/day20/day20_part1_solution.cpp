#include <iostream>
#include <vector>
#include <map> 
#include <regex>
#include <set>
#include <cassert>

using namespace std;

vector<string> flippedHorizontally(const vector<string>& tileDef)
{
    vector<string> flipped(tileDef);
    for (auto& patternLine : flipped)
    {
        reverse(patternLine.begin(), patternLine.end());
    }
    return flipped;
}

vector<string> rotated90DegClockwise(const vector<string>& tileDef, int numTimes = 1)
{
    if (numTimes != 1)
    {
        vector<string> rotated = tileDef;
        for (int i = 0; i < numTimes; i++)
        {
             rotated = rotated90DegClockwise(rotated);
        }
        return rotated;
    }
    const int tileSize = tileDef.size();
    vector<string> rotated;
    for (int columnNumber = 0; columnNumber < tileSize; columnNumber++)
    {
        string rotatedRow;
        for (auto revRowIter = tileDef.rbegin(); revRowIter != tileDef.rend(); revRowIter++)
        {
            rotatedRow += (*revRowIter)[columnNumber];
        }
        rotated.push_back(rotatedRow);
    }
    return rotated;
}

struct TileConfig
{
    int tileId = -1;
    bool flipped = false;
    int num90DegRotatesClockwise = 0;

    vector<string> tileDef;
    vector<int> tileConfigsThatFitBelow;
    vector<int> tileConfigsThatFitToRight;
};
void findMetaTileArrangement(const vector<TileConfig>& tileConfigs, vector<vector<int>>& tileArrangement, int nextTileRow, int nextTileCol, map<int, bool>& isTileIdUsed, const int metaTileSize, vector<vector<int>>& destTileArrangement)
{
    if (nextTileRow == metaTileSize)
    {
        //assert(destTileArrangement.empty());
        destTileArrangement = tileArrangement;
        // Sanity checks (paranoia!)
        set<int> usedTileIds;
        for (int row = 0; row < metaTileSize; row++)
        {
            for (int col = 0; col < metaTileSize; col++)
            {
                const auto tileConfigId = tileArrangement[row][col];
                const auto& tileConfig = tileConfigs[tileConfigId];
                assert (usedTileIds.find(tileConfig.tileId) == usedTileIds.end());
                usedTileIds.insert(tileConfig.tileId);

                if (col > 0)
                {
                    const auto& tileConfigToLeft = tileConfigs[tileArrangement[row][col- 1]];
                    assert(find(tileConfigToLeft.tileConfigsThatFitToRight.begin(),tileConfigToLeft.tileConfigsThatFitToRight.end(),tileConfigId) != tileConfigToLeft.tileConfigsThatFitToRight.end());
                }
                if (row > 0)
                {
                    const auto& tileConfigAbove = tileConfigs[tileArrangement[row- 1][col]];
                    assert(find(tileConfigAbove.tileConfigsThatFitBelow.begin(),tileConfigAbove.tileConfigsThatFitBelow.end(),tileConfigId) != tileConfigAbove.tileConfigsThatFitBelow.end());
                }
            }
        }
        assert(usedTileIds.size() == metaTileSize * metaTileSize);
        
        cout << "found" << endl;
        return;
    }
    cout << "nextTileRow: " << nextTileRow << " nextTileCol: " << nextTileCol << endl;

    vector<int> candidateTileConfigs;
    if (nextTileRow == 0 && nextTileCol == 0)
    {
        for (int tileConfigId = 0; tileConfigId < tileConfigs.size(); tileConfigId++)
        {
            candidateTileConfigs.push_back(tileConfigId);
        }
        cout << " Blee: " << candidateTileConfigs.size() << endl;
    }
    else
    {
        if (nextTileRow != 0)
        {
            candidateTileConfigs = tileConfigs[tileArrangement[nextTileRow - 1][nextTileCol]].tileConfigsThatFitBelow;
        }
        else if (nextTileCol != 0)
        {
            candidateTileConfigs = tileConfigs[tileArrangement[nextTileRow][nextTileCol - 1]].tileConfigsThatFitToRight;
        }
    }
    candidateTileConfigs.erase(remove_if(candidateTileConfigs.begin(), candidateTileConfigs.end(), [&isTileIdUsed,&tileConfigs](const int tileConfigId)
                {
                
                    cout << " used? " << " tile id: " << tileConfigs[tileConfigId].tileId << isTileIdUsed[tileConfigs[tileConfigId].tileId] << endl;
                    return isTileIdUsed[tileConfigs[tileConfigId].tileId];
                }),
                candidateTileConfigs.end());
    cout << " after erasing used: " << candidateTileConfigs.size() << endl;
    if (nextTileRow != 0)
    {
        candidateTileConfigs.erase(remove_if(candidateTileConfigs.begin(), candidateTileConfigs.end(), [&tileArrangement,&tileConfigs,nextTileRow, nextTileCol](const int tileConfigId)
                    {
                    const auto& tileConfigAbove = tileConfigs[tileArrangement[nextTileRow - 1][nextTileCol]];
                    return (find(tileConfigAbove.tileConfigsThatFitBelow.begin(), tileConfigAbove.tileConfigsThatFitBelow.end(), tileConfigId) == tileConfigAbove.tileConfigsThatFitBelow.end());
                    }),
                candidateTileConfigs.end());
    }
    cout << " after erasing due to above: " << candidateTileConfigs.size() << endl;
    if (nextTileCol != 0)
    {
        candidateTileConfigs.erase(remove_if(candidateTileConfigs.begin(), candidateTileConfigs.end(), [&tileArrangement,&tileConfigs,nextTileRow, nextTileCol](const int tileConfigId)
                    {
                    const auto& tileConfigToLeft = tileConfigs[tileArrangement[nextTileRow][nextTileCol - 1]];
                    return (find(tileConfigToLeft.tileConfigsThatFitToRight.begin(), tileConfigToLeft.tileConfigsThatFitToRight.end(), tileConfigId) == tileConfigToLeft.tileConfigsThatFitToRight.end());
                    }),
                candidateTileConfigs.end());
    }
    cout << " after erasing due to left: " << candidateTileConfigs.size() << endl;

    cout << " # candidates: " << candidateTileConfigs.size() << endl;

    for (const auto tileConfigId : candidateTileConfigs)
    {
        tileArrangement[nextTileRow][nextTileCol] = tileConfigId;
        isTileIdUsed[tileConfigs[tileConfigId].tileId] = true;
        int nextNextTileRow = nextTileRow;
        int nextNextTileCol = nextTileCol;
        nextNextTileCol++;
        if (nextNextTileCol >= metaTileSize)
        {
            nextNextTileCol = 0;
            nextNextTileRow++;
        }

        findMetaTileArrangement(tileConfigs, tileArrangement, nextNextTileRow, nextNextTileCol, isTileIdUsed, metaTileSize, destTileArrangement);

        isTileIdUsed[tileConfigs[tileConfigId].tileId] = false;
        tileArrangement[nextTileRow][nextTileCol] = -1;
    }
}



int main()
{
    string line;
    map<int, vector<string>> tileById;
    int currentTileId = -1;
    while (getline(cin, line))
    {
        static regex tileIDRegex(R"(Tile\s*(\d*)\s*:)");

        if (line.empty())
        {
            currentTileId = -1;
            continue;
        }

        std::smatch tileIdMatchInfo;
        if (regex_match(line, tileIdMatchInfo, tileIDRegex))
        {
            currentTileId = stoi(tileIdMatchInfo[1]);
        }
        else if (currentTileId != -1)
        {
            tileById[currentTileId].push_back(line);
        }

    }

    int tileWidth = -1;
    int tileHeight = -1;
    for (const auto& [tileId, tileDef] : tileById)
    {
        cout << "Tile ID: " << tileId << endl;
        for (const auto& patternLine : tileDef)
        {
            assert((tileWidth == -1 || patternLine.size() == tileWidth));
            cout << patternLine << endl;
            tileWidth = patternLine.size();
        }
        cout << "----------------------" << endl;
        assert((tileHeight == -1 || tileDef.size() == tileHeight));
        tileHeight = tileDef.size();

        cout << "Flipped horizontally: " << endl;
        for (const auto& patternLine : flippedHorizontally(tileDef))
        {
            cout << patternLine << endl;
        }
        cout << " -- " << endl;
        cout << "Rotated 90 degrees clockwise: " << endl;
        for (const auto& patternLine : rotated90DegClockwise(tileDef))
        {
            cout << patternLine << endl;
        }
        cout << " -- " << endl;
        cout << "Rotated 270 degrees clockwise: " << endl;
        for (const auto& patternLine : rotated90DegClockwise(tileDef, 3))
        {
            cout << patternLine << endl;
        }
        cout << " -- " << endl;
    }
    assert(tileWidth == tileHeight);

    const int numTiles = static_cast<int>(tileById.size());
    int metaTileSize = 0;
    while (metaTileSize * metaTileSize < numTiles)
    {
        metaTileSize++;
    }
    assert(metaTileSize * metaTileSize == numTiles);

    cout << "# tiles: " << numTiles << " tileWidth: " << tileWidth << " tileHeight: " << tileHeight << " metaTileSize: " << metaTileSize << endl;


    vector<TileConfig> tileConfigs;
    for (const auto& [tileId, tileDef] : tileById)
    {
        for (bool flip : {false, true})
        {
            for (int num90DegRotatesClockwise : {0, 1, 2, 3})
            {
                TileConfig tileConfig{tileId, flip, num90DegRotatesClockwise, tileDef, {}, {}};
                if (flip)
                {
                    tileConfig.tileDef = flippedHorizontally(tileConfig.tileDef);
                }
                for (int i = 0; i < num90DegRotatesClockwise; i++)
                {
                    tileConfig.tileDef = rotated90DegClockwise(tileConfig.tileDef);
                }
                tileConfigs.push_back(tileConfig);
            }
        }
    }
    cout << "# tile configs: " << tileConfigs.size() << endl;
    for (auto& tileConfig : tileConfigs)
    {
        for (int otherTileConfigId = 0; otherTileConfigId < tileConfigs.size(); otherTileConfigId++)
        {
            const auto& otherTileConfig = tileConfigs[otherTileConfigId];
            if (otherTileConfig.tileId == tileConfig.tileId)
                continue;
            if (otherTileConfig.tileDef.front() == tileConfig.tileDef.back())
            {
                tileConfig.tileConfigsThatFitBelow.push_back(otherTileConfigId);
                cout << "Vertical match tiles: " << endl;
                for(const auto& patternLine : tileConfig.tileDef)
                {
                    cout << patternLine << endl;
                }
                cout << string(tileWidth, '|') << endl;
                for(const auto& patternLine : otherTileConfig.tileDef)
                {
                    cout << patternLine << endl;
                }
            }
            if (flippedHorizontally(rotated90DegClockwise(otherTileConfig.tileDef)).front() == rotated90DegClockwise(tileConfig.tileDef, 3).front())
            {
                tileConfig.tileConfigsThatFitToRight.push_back(otherTileConfigId);
                cout << "Horizontal match tiles: " << endl;
                for (int row = 0; row < tileHeight; row++)
                {
                    cout << tileConfig.tileDef[row] + "-" + otherTileConfig.tileDef[row] << endl;
                }
                cout << "--" << endl;
            }
        }
        cout << "Tile config has " << tileConfig.tileConfigsThatFitBelow.size() << " configs that fit below it and " << tileConfig.tileConfigsThatFitToRight.size() << " that fit to its right " << endl;
    }


    vector<vector<int>> tileArrangement(metaTileSize, vector<int>(metaTileSize, -1));
    map<int, bool> isTileIdUsed;
    vector<vector<int>> destTileArrangement;
    findMetaTileArrangement(tileConfigs, tileArrangement, 0, 0, isTileIdUsed, metaTileSize, destTileArrangement);
    //assert(!destTileArrangement.empty());
    assert(!destTileArrangement.empty());
#if 1
    int64_t result = 1;
    result *= tileConfigs[destTileArrangement[0][0]].tileId;
    result *= tileConfigs[destTileArrangement[0][metaTileSize - 1]].tileId;
    result *= tileConfigs[destTileArrangement[metaTileSize - 1][metaTileSize - 1]].tileId;
    result *= tileConfigs[destTileArrangement[metaTileSize - 1][0]].tileId;
    cout << result << endl;
#endif



}
