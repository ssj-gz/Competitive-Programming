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

#if 0
    int lastTopUnblackened = -1;
    for (int i = topRowNumTiled - 1; i >= 0; i--)
    {
        if (!isTopCellBlackened[i])
        {
            lastTopUnblackened = i;
            break;
        }
    }
    int lastBottomUnblackened = -1;
    for (int i = bottomRowNumTiled - 1; i >= 0; i--)
    {
        if (!isBottomCellBlackened[i])
        {
            lastBottomUnblackened = i;
            break;
        }
    }
    cout << "topRowNumTiled: " << topRowNumTiled << " bottomRowNumTiled: " << bottomRowNumTiled << " lastTopUnblackened: " << lastTopUnblackened << " lastBottomUnblackened: " << lastBottomUnblackened << endl;
    if (lastTopUnblackened != topRowNumTiled - 1 || lastBottomUnblackened != bottomRowNumTiled - 1)
    {
        // One of the last top/ bottom cells is blackened.
        cout << "Doing thing" << endl;
        canTile |= canTileGrid(lastTopUnblackened + 1, lastBottomUnblackened + 1, isTopCellBlackened, isBottomCellBlackened, canTileGridLookup);
    }
#endif
    if (topRowNumTiled > 0 && isTopCellBlackened[topRowNumTiled - 1])
    {
        canTile |= canTileGrid(topRowNumTiled - 1, bottomRowNumTiled, isTopCellBlackened, isBottomCellBlackened, canTileGridLookup);
        if (canTile)
        {
            //cout << "topRowNumTiled: " << topRowNumTiled << " bottomRowNumTiled: " << bottomRowNumTiled << " can tile due to thing" << isTopCellBlackened[topRowNumTiled - 1] << endl;
        }
    }
    if (bottomRowNumTiled > 0 && isBottomCellBlackened[bottomRowNumTiled - 1])
    {
        canTile |= canTileGrid(topRowNumTiled, bottomRowNumTiled - 1, isTopCellBlackened, isBottomCellBlackened, canTileGridLookup);
    }
    
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

    //cout << "topRowNumTiled: " << topRowNumTiled << " bottomRowNumTiled: " << bottomRowNumTiled << " can tile: " << canTile << endl;
    canTileGridLookup[topRowNumTiled][bottomRowNumTiled] = canTile;

    return canTile;
}

int main()
{
    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        int n;
        cin >> n;

        string topRow;
        cin >> topRow;

        vector<bool> isTopCellBlackened;
        for (const auto letter : topRow)
        {
            isTopCellBlackened.push_back(letter == '1');
        }

        string bottomRow;
        cin >> bottomRow;

        vector<bool> isBottomCellBlackened;
        for (const auto letter : bottomRow)
        {
            isBottomCellBlackened.push_back(letter == '1');
        }

        for (int i = 0; i < n; i++)
        {
            //cout << "isTopCellBlackened[" << i << "] = " << isTopCellBlackened[i] << endl;
        }

        vector<vector<int>> canTileGridLookup(n + 1, vector<int>(n + 1, -1));
        canTileGridLookup[0][0] = 1;

        const bool result = canTileGrid(n, n, isTopCellBlackened, isBottomCellBlackened, canTileGridLookup);
        if (result)
            cout << "YES";
        else
            cout << "NO";
        cout << endl;
    }

}
