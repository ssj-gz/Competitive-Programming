// Simon St James (ssjgz) - 2020-07-31
// 
// Solution to: https://www.codechef.com/problems/ENTEXAM
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

int main()
{
    ios::sync_with_stdio(false);

    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto numStudents = read<int>();
        const auto maxEnrollableStudents = read<int>();
        const auto numExams = read<int>();
        const auto maxExamScore = read<int>();

        vector<int64_t> totalScoresForOtherStudents(numStudents - 1, 0);
        for (auto& totalOtherStudentScore : totalScoresForOtherStudents)
        {
            for (int examNum = 0; examNum < numExams; examNum++)
            {
                const auto score = read<int>();
                totalOtherStudentScore += score;
            }
        }

        int64_t sergeyScoreSoFar = 0;
        for (int examNum = 0; examNum < numExams - 1; examNum++)
        {
            const auto score = read<int>();
            sergeyScoreSoFar += score;
        }

        vector<int64_t> otherStudentScoresSorted = totalScoresForOtherStudents;
        sort(otherStudentScoresSorted.begin(), otherStudentScoresSorted.end());

        const int64_t requiredTotalSergeyScoreForEnrollment = otherStudentScoresSorted[numStudents - maxEnrollableStudents - 1] + 1;
        const int64_t requiredSergeyFinalExamScore = max<int64_t>(0, requiredTotalSergeyScoreForEnrollment - sergeyScoreSoFar);
#ifdef DIAGNOSTICS
        cout << "otherStudentScoresSorted: " << endl;
        for (const auto x  : otherStudentScoresSorted)
        {
            cout << x << " ";
        }
        cout << endl;
        cout << "requiredTotalSergeyScoreForEnrollment: " << requiredTotalSergeyScoreForEnrollment << endl;
        cout << "sergeyScoreSoFar: " << sergeyScoreSoFar << endl;
#endif
        if (requiredSergeyFinalExamScore > maxExamScore)
        {
            cout << "Impossible" << endl;
        }
        else
        {
            cout << requiredSergeyFinalExamScore << endl;
        }
    }

    assert(cin);
}
