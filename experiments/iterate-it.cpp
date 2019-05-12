//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cassert>

#include <sys/time.h>
#include <chrono>

using namespace std;

int solutionBruteForce(const vector<int>& aOriginal)
{
    vector<int> a(aOriginal);
    int rep = 0;
    std::chrono::steady_clock::time_point totalBegin = std::chrono::steady_clock::now();
    while (!a.empty())
    {
        const int originalSize = a.size();
        std::chrono::steady_clock::time_point iterationBegin = std::chrono::steady_clock::now();


        cout << "Iterate: a: " << endl;
        for (const auto x : a)
        {
            cout << " " << x;
        }
        cout << endl;
        cout << " # elements: " << originalSize << " max element: " << *std::max_element(a.begin(), a.end()) << endl;
#if 0
        set<int> bSet;
        for (const auto x : a)
        {
            for (const auto y : a)
            {
                if (x != y)
                    bSet.insert(abs(x - y));
            }
        }
        a.assign(bSet.begin(), bSet.end());
#else
        vector<bool> b(*std::max_element(a.begin(), a.end()), false);
        for (const auto x : a)
        {
            for (const auto y : a)
            {
                if (x != y)
                    b[abs(x - y)] = true;
            }
        }
        a.clear();
        for (int i = 0; i < b.size(); i++)
        {
            if (b[i])
                a.push_back(i);
        }
#endif
        rep++;

        std::chrono::steady_clock::time_point end= std::chrono::steady_clock::now();
        std::cout << "ITeration of " << originalSize << " elements took : " << std::chrono::duration_cast<std::chrono::milliseconds>(end - iterationBegin).count() << "ms - total elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - totalBegin).count() << "ms"  <<std::endl;
    }
    return rep;
}

int solutionOptimised(const vector<int>& aOriginal)
{
    set<int> blah(aOriginal.begin(), aOriginal.end());
    vector<int> a(blah.begin(), blah.end());
    int numIterations = 0;
    const int numInBlock = 64;
    const int numBlocks = *std::max_element(a.begin(), a.end()) / numInBlock + 1;
    vector<uint64_t> blocks(numBlocks);
    for (const auto x : a)
    {
        const uint64_t glarp = (static_cast<uint64_t>(1) << static_cast<uint64_t>(x % numInBlock));
        cout << "x: " << x << " glarp: " << glarp << " gleep: " << (x / numInBlock) << endl;
        cout << "before: " << blocks[x / numInBlock] << endl;
        blocks[x / numInBlock] |= glarp;
        cout << "after: " << blocks[x / numInBlock] << endl;
    }

    auto blocksToBinaryString = [numInBlock, numBlocks](const vector<uint64_t>& blocks)
    {
        string binaryString;
        for (int j = 0; j < numBlocks; j++)
        {
            const uint64_t block = blocks[j];
            //cout << "block[" << j << "]: " << block << endl;
            for (int k = 0; k < numInBlock; k++)
            {
                if (((block >> static_cast<uint64_t>(k)) & 1) == 1)
                {
                    binaryString.push_back('1');
                }
                else
                {
                    binaryString.push_back('0');
                }
            }
        }
        return binaryString;
    };
#if 1
    vector<uint64_t> blocksShifted(numBlocks);
    vector<vector<uint64_t>> blocksShiftedBy;
    blocksShiftedBy.push_back(blocks);
    for (int i = 1; i < numInBlock; i++)
    {
        blocksShiftedBy.push_back(blocksShiftedBy.back());
        for (int blockNum = 0; blockNum < numBlocks; blockNum++)
        {
            blocksShiftedBy.back()[blockNum] >>= 1;
            if (blockNum + 1 < numBlocks)
            {
                //blocksShiftedBy.back()[blockNum] |= (blocksShiftedBy.back()[blockNum + 1] >> static_cast<uint64_t>(numInBlock - 1));
                cout << "Next block: " << (blocksShiftedBy.back()[blockNum + 1]) << endl;
                blocksShiftedBy.back()[blockNum] |= ((blocksShiftedBy.back()[blockNum + 1] & 1) << static_cast<uint64_t>(numInBlock - 1));
            }
        }
    }
#endif

    for (int i = 0; i < numInBlock; i++)
    {
        const auto& blocks = blocksShiftedBy[i];
        cout << "i: " << i << endl;
        cout << "shifted as binary string: " << blocksToBinaryString(blocks) << endl;
        vector<int> bBlah;
        for (auto x : a)
        {
            x -= i;
            if (x >= 0)
                bBlah.push_back(x);
        }
        vector<int> dbg;
        for (int j = 0; j < numBlocks; j++)
        {
            const uint64_t block = blocks[j];
            cout << "block[" << j << "]: " << block << endl;
            for (int k = 0; k < numInBlock; k++)
            {
                if (((block >> static_cast<uint64_t>(k)) & 1) == 1)
                {
                    dbg.push_back(k + j * numInBlock);
                }
            }
        }
        cout << "bBlah: " << endl;
        for (const auto x : bBlah)
        {
            cout << " " << x;
        }
        cout << endl;
        cout << "dbg: " << endl;
        for (const auto x : dbg)
        {
            cout << " " << x;
        }
        cout << endl;
        assert(bBlah == dbg);
    }


    return numIterations;
}

int main(int argc, char* argv[])
{
    if (argc == 2)
    {

#if 0
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));


        const int n = rand() % 1000;
#endif
        const int n = 25000;
        cout << (n + 1) << endl;
        for (int i = 0; i < n; i++)
        {
#if 0
            cout << (rand() % 10'000 + 1) << " ";
#endif
            cout << (i * 2) << " ";
        }
        cout << 25679;
        cout << endl;
        return 0;
    }
    int n;
    cin >> n;

    vector<int> a(n);
    for (int i = 0; i < n; i++)
    {
        cin >> a[i];
    }

    //const int bruteForceResult = solutionBruteForce(a);
    //cout << "bruteForceResult: " << bruteForceResult << endl;

    const int optimisedResult = solutionOptimised(a);
    cout << "optimisedResult: " << optimisedResult << endl;
}
