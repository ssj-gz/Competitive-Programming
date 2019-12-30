// Simon St James (ssjgz) - 2019-12-30
// 
// Solution to: https://www.codechef.com/problems/PD33
//
#include <iostream>
#include <vector>

#include <cassert>

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

string decrypt(const int numColumns, const string& encrypted)
{
    const int numRows = encrypted.length() / numColumns;

    vector<string> decryptedCells(numRows, string(numColumns, '.'));

    // Read into decryptedCells, go back and forth for each row.
    int index = 0;
    int direction = +1;
    for (int rowIndex = 0; rowIndex < numRows; rowIndex++)
    {
        for (int colIndex = 0; colIndex < numColumns; colIndex++)
        {
            decryptedCells[rowIndex][colIndex] = encrypted[index];
            index += direction;
        }
        index += numColumns - direction;
        direction = -direction;
    }

    // Read decryptedCells into decrypted, column-by-column.
    string decrypted;
    for (int colIndex = 0; colIndex < numColumns; colIndex++)
    {
        for (int rowIndex = 0; rowIndex < numRows; rowIndex++)
        {
            decrypted += decryptedCells[rowIndex][colIndex];
        }
    }

    return decrypted;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);

    while (true)
    {
        const int numColumns = read<int>();
        if (numColumns == 0)
            break;

        const auto encrypted = read<string>();

        cout << decrypt(numColumns, encrypted) << endl;

    }

    assert(cin);
}
