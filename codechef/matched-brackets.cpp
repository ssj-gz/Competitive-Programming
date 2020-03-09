// Simon St James (ssjgz) - 2020-03-09
// 
// Solution to: https://www.codechef.com/ZCOPRAC/problems/ZCO12001/
//
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

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int initialN = 1 + rand() % 75;

        vector<int> s;

        int nestingDepth = 0;
        for (int i = 0; i < initialN; i++)
        {
            bool openBracket = ((rand() % 2 + 1) == 1);
            if (!openBracket && nestingDepth == 0)
            {
                openBracket = true;
            }
            if (openBracket)
            {
                s.push_back(1);
                nestingDepth++;
            }
            else
            {
                s.push_back(2);
                nestingDepth--;
            }
        }
        while (nestingDepth > 0)
        {
            s.push_back(2);
            nestingDepth--;
        }

        const int N = s.size();
        cout << N << endl;
        for (int i = 0; i < N; i++)
        {
            cout << s[i];
            if (i != N - 1)
                cout << " ";
        }


        return 0;
    }
    
#if 0
    const int N = read<int>();
    string s;

    for (int i = 0; i < N; i++)
    {
        if (read<int>() == 1)
            s += '(';
        else
            s += ')';
    }
#else
    string s = read<string>();
    const int N = s.size();
    cout << N << endl;
    for(const auto bracket : s)
    {
        if (bracket == '(')
            cout << 1 << " ";
        else
            cout << 2 << " ";
    }
    return 0;
#endif

    vector<int> positionsOfUnmatchedOpenBrackets;
    int nestingDepth = 0;
    int largestNestingDepth = -1;
    int largestNestingDepthPos = -1;
    int largestDistBetweenMatchedBrackets = -1;
    int largestDistBetweenMatchedBracketsPos = -1;
    for (int pos = 0; pos < N; pos++)
    {
        if (s[pos] == '(')
        {
            nestingDepth++;
            positionsOfUnmatchedOpenBrackets.push_back(pos);
            if (nestingDepth > largestNestingDepth)
            {
                largestNestingDepth = nestingDepth;
                largestNestingDepthPos = pos;
            }
        }
        else
        {
            const auto distBetweenMatchedBrackets = pos - positionsOfUnmatchedOpenBrackets.back() + 1;
            if (distBetweenMatchedBrackets > largestDistBetweenMatchedBrackets)
            {
                largestDistBetweenMatchedBrackets = distBetweenMatchedBrackets;
                largestDistBetweenMatchedBracketsPos = positionsOfUnmatchedOpenBrackets.back();
            }
            positionsOfUnmatchedOpenBrackets.pop_back();
            nestingDepth--;
            assert(nestingDepth >= 0);
        }
    }

    cout << largestNestingDepth << " " << (largestNestingDepthPos + 1) << " " << largestDistBetweenMatchedBrackets << " " << (largestDistBetweenMatchedBracketsPos + 1) << endl;

    assert(cin);
}
