// Simon St James (ssjgz) - 2019-12-30
// 
// Solution to: https://www.codechef.com/problems/PD33
//
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

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
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int T = 1 + rand() % 10;

        for (int t = 0; t < T; t++)
        {
            const int numColumns = 2 + rand() % 19;
            const int numRows = 1 + rand() % (20 / numColumns);
            const int maxLetter = 1 + rand() % 26;


            string s;
            for (int i = 0; i < numRows * numColumns; i++)
            {
                s += 'a' + rand() % maxLetter;
            }

            cout << numColumns << endl;
            cout << s << endl;
        }
        cout << 0 << endl;

        return 0;
    }

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
