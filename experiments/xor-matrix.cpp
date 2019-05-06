#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cassert>

#include <sys/time.h>

using namespace std;

vector<vector<vector<int64_t>>> blee;
int narb = 0;

vector<int64_t> glarp(int row, int col)
{
    if (row >= blee.size())
    {
        blee.resize(row + 1);
    }
    if (col >= blee[row].size())
    {
        blee[row].resize(col + 1);
    }
    if (!blee[row][col].empty())
    {
        return blee[row][col];
    }
    if (row == 0)
    {
        vector<int64_t> floop(narb, 0);
        floop[col] = 1;
        blee[row][col] = floop;

        return floop;
    }
    const auto gleep1 = glarp(row - 1, col);
    const auto gleep2 = glarp(row - 1, (col + 1) % narb);
    vector<int64_t> floop(narb, 0);
    for (int i = 0; i < narb; i++)
    {
        //floop[i] = gleep1[i] + gleep2[i];
        floop[i] = (gleep1[i] + gleep2[i]) % 2;
    }
    blee[row][col] = floop;
    return floop;
}

#if 0
int main()
{
    map<string, int> blee;
    //string current = "00100000000011100000000100100000000000001111000000111110000000000000000000010";
    string current = "1010110101001011010101010000000000000000000000000000000000000000000000100011100111100111100111100111100111100111100111100111100111100111100111100111100111100111100111100111100111100111";
    //string current = "111000101010101010101";
    narb = current.size();
    cout << "size: " << current.size() << endl;
#if 0
    int count = 0;
    while (true)
    {
        cout << current << endl;
        if (blee.find(current) != blee.end())
        {
            //cout << "Count: " << blee[current] << " repeated at: " << count << " gap: " << (count - blee[current]) << endl;
        }
        blee[current] = count;
        string next;
        for (int i = 0; i < current.size(); i++)
        {
            const int blah = current[i] - '0';
            const int blah2 = current[(i + 1) % current.size()] - '0';
            next.push_back('0' + (blah ^ blah2));
        }
        current = next;
        count++;
    }
#endif
#if 0
    int row = 0;
    uint64_t powerOf2 = 1;
    while (true)
    {
        //cout << "row: " << row << " glarp: " << std::endl;
        int blork = 0;
        //if (row == powerOf2)
        {
            for (int i = 0; i < narb; i++)
            {
                const auto num = glarp(row, 0)[i];
                cout 
                    //<< " " 
                    << num;
                if (num == 1)
                    blork++;
                //cout << " " << (glarp(row, 0)[i] % 2);
            }
            cout << " row: " << row << " glarp: " << std::endl;
            powerOf2 *= 2;
        }
        //cout << endl;
        //if (blork == narb)
            //break;
        row++;
    }
#else
    for (int col = 0; col < narb; col++)
    {
        for (int row = 0; row < 100; row++)
        {
            cout << glarp(row, col)[0];
        }
        cout << endl;
    }
#endif
}
#endif

vector<int64_t> solutionBruteForce(const vector<int64_t>& originalA, int n, uint64_t m)
{
    vector<int64_t> currentRow(originalA);
    uint64_t row = 1;
    while (row != m)
    {

        vector<int64_t> nextRow(n);
        for (int i = 0; i < n; i++)
        {
            nextRow[i] = currentRow[i] ^ currentRow[(i + 1) % n];
        }

        row++;
        currentRow = nextRow;
    }
    return currentRow;
}

vector<int64_t> solution(const vector<int64_t>& originalA, int n, uint64_t m)
{
    m--; // Already know the 0th row.
    vector<int64_t> currentRow(originalA);

    uint64_t powerOf2 = static_cast<uint64_t>(1) << static_cast<uint64_t>(63);
    while (powerOf2 != 0)
    {
        //cout << "powerOf2: " << powerOf2 << " m: " << m << endl;
        if (m >= powerOf2)
        {
            //cout << "power of 2 is greater >= m" << std::endl;
            vector<int64_t> currentPlusPowerOf2thRow(n);
            for (int i = 0; i < n; i++)
            {
                currentPlusPowerOf2thRow[i] ^= currentRow[i];
                currentPlusPowerOf2thRow[i] ^= currentRow[(i + powerOf2) % n];
            }
            currentRow = currentPlusPowerOf2thRow;


            m -= powerOf2;
        }
        powerOf2 >>= 1;
    }
    return currentRow;
}

#if 1
int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));


        const int n = rand() % 20 + 1;
        const int m = rand() % 1000 + 1;
        cout << n << " " << m << endl;
        for (int i = 0; i < n; i++)
        {
            cout << (rand() % 50 + 1) <<  " ";
        }
        cout << endl;
        return 0;
    }
    int n;
    cin >> n;
    uint64_t m;
    cin >> m;

    vector<int64_t> a(n);
    for (int i = 0; i < n ; i++)
    {
        cin >> a[i];
    }

    const auto bruteForceSolution = solutionBruteForce(a, n, m);
    cout << "bruteForceSolution: " << endl; 
    for (int i = 0; i < n; i++)
    {
        cout << bruteForceSolution[i] << " ";
    }
    cout << endl;

    const auto optimisedSolution = solution(a, n, m);
    cout << "optimisedSolution: " << endl; 
    for (int i = 0; i < n; i++)
    {
        cout << optimisedSolution[i] << " ";
    }
    cout << endl;
    assert(bruteForceSolution == optimisedSolution);
}
#endif


