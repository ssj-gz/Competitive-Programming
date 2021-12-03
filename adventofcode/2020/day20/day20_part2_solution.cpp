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
    //cout << "nextTileRow: " << nextTileRow << " nextTileCol: " << nextTileCol << endl;

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
                
                    //cout << " used? " << " tile id: " << tileConfigs[tileConfigId].tileId << isTileIdUsed[tileConfigs[tileConfigId].tileId] << endl;
                    return isTileIdUsed[tileConfigs[tileConfigId].tileId];
                }),
                candidateTileConfigs.end());
    //cout << " after erasing used: " << candidateTileConfigs.size() << endl;
    if (nextTileRow != 0)
    {
        candidateTileConfigs.erase(remove_if(candidateTileConfigs.begin(), candidateTileConfigs.end(), [&tileArrangement,&tileConfigs,nextTileRow, nextTileCol](const int tileConfigId)
                    {
                    const auto& tileConfigAbove = tileConfigs[tileArrangement[nextTileRow - 1][nextTileCol]];
                    return (find(tileConfigAbove.tileConfigsThatFitBelow.begin(), tileConfigAbove.tileConfigsThatFitBelow.end(), tileConfigId) == tileConfigAbove.tileConfigsThatFitBelow.end());
                    }),
                candidateTileConfigs.end());
    }
    //cout << " after erasing due to above: " << candidateTileConfigs.size() << endl;
    if (nextTileCol != 0)
    {
        candidateTileConfigs.erase(remove_if(candidateTileConfigs.begin(), candidateTileConfigs.end(), [&tileArrangement,&tileConfigs,nextTileRow, nextTileCol](const int tileConfigId)
                    {
                    const auto& tileConfigToLeft = tileConfigs[tileArrangement[nextTileRow][nextTileCol - 1]];
                    return (find(tileConfigToLeft.tileConfigsThatFitToRight.begin(), tileConfigToLeft.tileConfigsThatFitToRight.end(), tileConfigId) == tileConfigToLeft.tileConfigsThatFitToRight.end());
                    }),
                candidateTileConfigs.end());
    }
    //cout << " after erasing due to left: " << candidateTileConfigs.size() << endl;

    //cout << " # candidates: " << candidateTileConfigs.size() << endl;

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

pair<int, int> findSeamonsters(const vector<string>& metaImage)
{
    static const vector<string> seaMonsterTemplate = {
        "                  # ",
        "#    ##    ##    ###",
        " #  #  #  #  #  #   "
    };

    const int seaMonsterHeight = seaMonsterTemplate.size();
    const int seaMonsterWidth = seaMonsterTemplate.front().size();

    const int imageHeight = metaImage.size();
    const int imageWidth = metaImage.front().size();

    int numSeaMonsters = 0;

    vector<vector<bool>> isPartOfSeaMonster(imageHeight, vector<bool>(imageWidth, false));

    for (int imageRowStart = 0; imageRowStart < imageHeight; imageRowStart++)
    {
        for (int imageColStart = 0; imageColStart < imageWidth; imageColStart++)
        {
            bool seaMonsterFound = true;
            int blee = 0;
            for (int seaMonsterRow = 0; seaMonsterRow < seaMonsterHeight; seaMonsterRow++)
            {
                for (int seaMonsterCol = 0; seaMonsterCol < seaMonsterWidth; seaMonsterCol++)
                {
                    if (seaMonsterTemplate[seaMonsterRow][seaMonsterCol] == '#')
                    {
                        const int imageRow = imageRowStart + seaMonsterRow;
                        const int imageCol = imageColStart + seaMonsterCol;
                        blee++;

                        if (imageRow >= imageHeight || imageCol >= imageWidth || metaImage[imageRow][imageCol] != '#')
                        {
                            seaMonsterFound = false;
                            break;
                        }
                    }

                }
            }
            //cout << "blee: " << blee << endl;
            if (seaMonsterFound)
            {
                numSeaMonsters++;
                for (int seaMonsterRow = 0; seaMonsterRow < seaMonsterHeight; seaMonsterRow++)
                {
                    for (int seaMonsterCol = 0; seaMonsterCol < seaMonsterWidth; seaMonsterCol++)
                    {
                        if (seaMonsterTemplate[seaMonsterRow][seaMonsterCol] == '#')
                        {
                            const int imageRow = imageRowStart + seaMonsterRow;
                            const int imageCol = imageColStart + seaMonsterCol;
                            isPartOfSeaMonster[imageRow][imageCol] = true;
                        }

                    }
                }
            }
        }
    }
    int numNotPartOfSeaMonster = 0;
    for (int imageRow = 0; imageRow < imageHeight; imageRow++)
    {
        for (int imageCol = 0; imageCol < imageWidth; imageCol++)
        {
            if (metaImage[imageRow][imageCol] == '#' && !isPartOfSeaMonster[imageRow][imageCol])
            {
                numNotPartOfSeaMonster++;
            }
        }
    }
    return {numSeaMonsters, numNotPartOfSeaMonster};
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
#if 0
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
#endif
            }
            if (flippedHorizontally(rotated90DegClockwise(otherTileConfig.tileDef)).front() == rotated90DegClockwise(tileConfig.tileDef, 3).front())
            {
                tileConfig.tileConfigsThatFitToRight.push_back(otherTileConfigId);
#if 0
                cout << "Horizontal match tiles: " << endl;
                for (int row = 0; row < tileHeight; row++)
                {
                    cout << tileConfig.tileDef[row] + "-" + otherTileConfig.tileDef[row] << endl;
                }
                cout << "--" << endl;
#endif
            }
        }
        //cout << "Tile config has " << tileConfig.tileConfigsThatFitBelow.size() << " configs that fit below it and " << tileConfig.tileConfigsThatFitToRight.size() << " that fit to its right " << endl;
    }


    cout << "Looking for tile arrangement ... " << endl;
    vector<vector<int>> tileArrangement(metaTileSize, vector<int>(metaTileSize, -1));
    map<int, bool> isTileIdUsed;
    vector<vector<int>> destTileArrangement;
    findMetaTileArrangement(tileConfigs, tileArrangement, 0, 0, isTileIdUsed, metaTileSize, destTileArrangement);
    //assert(!destTileArrangement.empty());
    assert(!destTileArrangement.empty());
    cout << " ... done" << endl;
     // Strip borders.
    for (auto& tileConfig : tileConfigs)
    {
        tileConfig.tileDef.erase(tileConfig.tileDef.begin());
        tileConfig.tileDef.pop_back();
        for (auto& patternLine : tileConfig.tileDef)
        {
            patternLine.erase(patternLine.begin());
            patternLine.pop_back();
        }
    }

    const int newTileHeight = tileHeight - 2;
    const int newTileWidth = tileWidth - 2;
    vector<string> metaImage(metaTileSize * newTileHeight, string(metaTileSize * newTileWidth, 'X'));
#if 1
    for (int row = 0; row < metaTileSize; row++)
    {
        for (int col = 0; col < metaTileSize; col++)
        {
            const auto& tileDef = tileConfigs[destTileArrangement[row][col]].tileDef;
            for (int sourceX = 0; sourceX < newTileWidth; sourceX++)
            {
                for (int sourceY = 0; sourceY < newTileHeight; sourceY++)
                {
                    metaImage[row * newTileHeight + sourceY][col * newTileWidth + sourceX] = tileDef[sourceY][sourceX];
                }
            }

        }
    }
#endif
    cout << "Meta image: " << endl;
    for (const auto& patternLine : metaImage)
    {
        cout << patternLine << endl;
    }

    bool foundSeaMonsters = false;
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            metaImage = rotated90DegClockwise(metaImage);
            const auto [numSeaMonsters, numNotPartOfSeaMonster] = findSeamonsters(metaImage);
            if (numSeaMonsters > 0)
            {
                assert(!foundSeaMonsters);
                foundSeaMonsters = true;
                cout << numNotPartOfSeaMonster << endl;
            }
        }
        metaImage = flippedHorizontally(metaImage);
    }

#if 0
    int64_t result = 1;
    result *= tileConfigs[destTileArrangement[0][0]].tileId;
    result *= tileConfigs[destTileArrangement[0][metaTileSize - 1]].tileId;
    result *= tileConfigs[destTileArrangement[metaTileSize - 1][metaTileSize - 1]].tileId;
    result *= tileConfigs[destTileArrangement[metaTileSize - 1][0]].tileId;
    cout << result << endl;
#endif


}
