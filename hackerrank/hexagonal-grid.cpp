// Simon St James (ssjgz) - 2018-01-29
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

bool canTileGrid(const int topRowNumTiled, const int bottomRowNumTiled, const vector<bool>& isTopCellBlackened, const vector<bool>& isBottomCellBlackened, vector<vector<int>>& canTileGridLookup)
{
    assert(topRowNumTiled >= 0 && bottomRowNumTiled >= 0);
    if (canTileGridLookup[topRowNumTiled][bottomRowNumTiled] != -1)
        return canTileGridLookup[topRowNumTiled][bottomRowNumTiled];

    bool canTile = false;

    if (topRowNumTiled > 0 && isTopCellBlackened[topRowNumTiled - 1])
    {
        // The last of the topRowNumTiled to tile is blackened, so can tile this if and only if we can
        // tile topRowNumTiled - 1 on the top, and bottomRowNumTiled on the bottom.
        canTile |= canTileGrid(topRowNumTiled - 1, bottomRowNumTiled, isTopCellBlackened, isBottomCellBlackened, canTileGridLookup);
    }
    if (bottomRowNumTiled > 0 && isBottomCellBlackened[bottomRowNumTiled - 1])
    {
        // As above, the last of the bottomRowNumTiled is blackened.
        canTile |= canTileGrid(topRowNumTiled, bottomRowNumTiled - 1, isTopCellBlackened, isBottomCellBlackened, canTileGridLookup);
    }
    
    // Can we complete the tiling by adding one last piece?
    // Piece a)
    if (topRowNumTiled >= 1 && bottomRowNumTiled == topRowNumTiled)
    {
        if (!isTopCellBlackened[topRowNumTiled - 1] && !isBottomCellBlackened[bottomRowNumTiled - 1])
            canTile = canTile || canTileGrid(topRowNumTiled - 1, bottomRowNumTiled - 1, isTopCellBlackened, isBottomCellBlackened, canTileGridLookup);
    }
    // Piece b)
    if (topRowNumTiled >= 2 && bottomRowNumTiled == topRowNumTiled - 1)
    {
        if (!isTopCellBlackened[topRowNumTiled - 1] && !isBottomCellBlackened[bottomRowNumTiled - 1])
            canTile = canTile || canTileGrid(topRowNumTiled - 1, bottomRowNumTiled - 1, isTopCellBlackened, isBottomCellBlackened, canTileGridLookup);
    }
    // Piece c) on top.
    if (topRowNumTiled >= 2)
    {
        if (!isTopCellBlackened[topRowNumTiled - 1] && !isTopCellBlackened[topRowNumTiled - 2])
            canTile = canTile || canTileGrid(topRowNumTiled - 2, bottomRowNumTiled, isTopCellBlackened, isBottomCellBlackened, canTileGridLookup);
    }
    // Piece c) on bottom.
    if (bottomRowNumTiled >= 2)
    {
        if (!isBottomCellBlackened[bottomRowNumTiled - 1] && !isBottomCellBlackened[bottomRowNumTiled - 2])
            canTile = canTile || canTileGrid(topRowNumTiled, bottomRowNumTiled - 2, isTopCellBlackened, isBottomCellBlackened, canTileGridLookup);
    }

    canTileGridLookup[topRowNumTiled][bottomRowNumTiled] = canTile;

    return canTile;
}

int main()
{
    // Fairly easy one, though again, lots of dumb mistakes on the way XD
    // Let canTileGridLookup[t][b] be true if and only if we can
    // add some tiles (including 0 tiles!) so that the first t tiles on the top
    // are all either tiled or blackened (and the rest untiled), and the first b tiles on the bottom
    // are all either tiled or blackened (and the rest untuiled).  There's an easy recurrence relation for this;
    // details are given inside canTileGrid, which is hopefully self-explanatory
    // (pieces a), b) and c) are the three tile pieces presented in the problem
    // description, in order).
    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        int n;
        cin >> n;

        auto readBooleanStringAsVector = []()
        {
            string booleanString;
            cin >> booleanString;

            vector<bool> booleanVector;
            for (const auto digit : booleanString)
            {
                booleanVector.push_back(digit == '1');
            }
            return booleanVector;
        };

        const auto isTopCellBlackened = readBooleanStringAsVector();
        const auto isBottomCellBlackened = readBooleanStringAsVector();

        vector<vector<int>> canTileGridLookup(n + 1, vector<int>(n + 1, -1));
        // Yes, we can certainly tile in such a way that no tiles are added!
        canTileGridLookup[0][0] = 1;

        const bool result = canTileGrid(n, n, isTopCellBlackened, isBottomCellBlackened, canTileGridLookup);
        if (result)
            cout << "YES";
        else
            cout << "NO";
        cout << endl;
    }

}
