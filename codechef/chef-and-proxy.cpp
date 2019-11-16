// Simon St James (ssjgz) - 2019-11-16
// 
// Solution to: https://www.codechef.com/problems/PROXYC
//
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>

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

bool canBeMarkedAsProxy(const int dayIndex, const string& attendance)
{
    if (attendance[dayIndex] == 'P')
        return false;
    if (dayIndex - 2 < 0 || dayIndex + 2 > attendance.size())
        return false;

    if ((attendance[dayIndex - 2] == 'P' || attendance[dayIndex - 1] == 'P') &&
        (attendance[dayIndex + 1] == 'P' || attendance[dayIndex + 2] == 'P'))
    {
        return true;
    }
    return false;
}

#if 1
int solveBruteForce(int D, const string& attendance)
{
    const int numAttendancesFor75 = (D % 4 == 0) ? (3 * D / 4)  : (3 * D / 4 + 1);
    const int numActualAttendances = count(attendance.begin(), attendance.end(), 'P');

    int numPossibleProxyAttendances = 0;
    for (int dayIndex = 0; dayIndex < D; dayIndex++)
    {
        if (canBeMarkedAsProxy(dayIndex, attendance))
            numPossibleProxyAttendances++;
    }


    if (numActualAttendances < numAttendancesFor75)
    {
        if (numPossibleProxyAttendances + numActualAttendances >= numAttendancesFor75)
        {
            return numAttendancesFor75 - numActualAttendances;
        }
        else
        {
            return -1;
        }
    }
    
    return 0;
}
#endif

#if 0
SolutionType solveOptimised()
{
    SolutionType result;
    
    return result;
}
#endif


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
        //const int T = rand() % 100 + 1;
        const int T = 1;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
        }

        return 0;
    }
    
    // TODO - read in testcase.
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int D = read<int>();
        const string S = read<string>();


        const auto solutionBruteForce = solveBruteForce(D, S);
        cout << solutionBruteForce << endl;
    }

    assert(cin);
}
