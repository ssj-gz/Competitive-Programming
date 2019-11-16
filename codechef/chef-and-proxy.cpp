// Simon St James (ssjgz) - 2019-11-16
// 
// Solution to: https://www.codechef.com/problems/PROXYC
//
#include <iostream>
#include <vector>
#include <algorithm>

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

bool canBeMarkedAsProxy(const int dayIndex, const string& attendance)
{
    if (attendance[dayIndex] == 'P')
        return false;
    if (dayIndex - 2 < 0 || dayIndex + 2 >= attendance.size())
        return false;

    if ((attendance[dayIndex - 2] == 'P' || attendance[dayIndex - 1] == 'P') &&
        (attendance[dayIndex + 1] == 'P' || attendance[dayIndex + 2] == 'P'))
    {
        return true;
    }
    return false;
}

int integerCeilingDivide(int numerator, int denominator)
{
    if (numerator % denominator == 0)
        return numerator / denominator;
    else
        return numerator / denominator + 1;

};

int findMinProxyAttendances(int D, const string& attendance)
{
    const int numAttendancesFor75 = integerCeilingDivide(3 * D, 4);
    const int numActualAttendances = count(attendance.begin(), attendance.end(), 'P');

    int numPossibleProxyAttendances = 0;
    for (int dayIndex = 0; dayIndex < D; dayIndex++)
    {
        if (canBeMarkedAsProxy(dayIndex, attendance))
        {
            numPossibleProxyAttendances++;
        }
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

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int D = read<int>();
        const string S = read<string>();
        assert(S.length() == D);

        cout << findMinProxyAttendances(D, S) << endl;
    }

    assert(cin);
}
