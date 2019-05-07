// Simon St James (ssjgz) - 2019-05-07
#include <iostream>
#include <vector>

using namespace std;

string calcRowOfMatrix(const string& originalProteinString, int n, uint64_t m)
{
    // Adapted from Xor Matrix.
    auto transform = [](char a, char b)
    {
        static const char transformTable[][4] =
        {
            {'A', 'B', 'C', 'D' },
            {'B', 'A', 'D', 'C' },
            {'C', 'D', 'A', 'B' },
            {'D', 'C', 'B', 'A' }
        };
        return transformTable[a - 'A'][b - 'A'];

    };

    string currentRow(originalProteinString);

    uint64_t powerOf2 = static_cast<uint64_t>(1) << static_cast<uint64_t>(63);
    while (powerOf2 != 0)
    {
        if (m >= powerOf2)
        {
            // Set the row to be all the identity element : we'll update it by 
            // transforming each element.
            string currentPlusPowerOf2thRow(n, 'A');

            for (int i = 0; i < n; i++)
            {
                currentPlusPowerOf2thRow[i] = transform(currentPlusPowerOf2thRow[i],  currentRow[i]);
                currentPlusPowerOf2thRow[i] = transform(currentPlusPowerOf2thRow[i], currentRow[(i + powerOf2) % n]);
            }
            currentRow = currentPlusPowerOf2thRow;

            m -= powerOf2;
        }
        powerOf2 >>= 1;
    }
    return currentRow;
}

int main(int argc, char* argv[])
{
    int N, K;
    cin >> N >> K;

    string proteinString;
    cin >> proteinString;

    const auto solution = calcRowOfMatrix(proteinString, N, K);
    cout << solution << endl;
}
