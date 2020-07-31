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

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);

    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto numStudents = read<int>();
        const auto maxEnrollableStudents = read<int>();
        const auto numExams = read<int>();
        const auto maxExamScore = read<int>();

        vector<int64_t> totalScoreForOtherStudent(numStudents - 1, 0);
        for (int otherStudentIndex = 0; otherStudentIndex < numStudents - 1; otherStudentIndex++)
        {
            for (int examIndex = 0; examIndex < numExams; examIndex++)
            {
                const auto score = read<int>();
                totalScoreForOtherStudent[otherStudentIndex] += score;
            }
        }

        int64_t sergeysScore = 0;
        for (int examIndex = 0; examIndex < numExams - 1; examIndex++)
        {
            const auto score = read<int>();
            sergeysScore += score;
        }

        vector<int64_t> otherStudentScoresSorted = totalScoreForOtherStudent;
        sort(otherStudentScoresSorted.begin(), otherStudentScoresSorted.end());


        const int64_t requiredTotalScoreForEnrollment = otherStudentScoresSorted[numStudents - maxEnrollableStudents - 1] + 1;
#ifdef DIAGNOSTICS
        cout << "otherStudentScoresSorted: " << endl;
        for (const auto x  : otherStudentScoresSorted)
        {
            cout << x << " ";
        }
        cout << endl;
        cout << "requiredTotalScoreForEnrollment: " << requiredTotalScoreForEnrollment << endl;
        cout << "sergeysScore: " << sergeysScore << endl;
#endif
        if (requiredTotalScoreForEnrollment > sergeysScore + maxExamScore)
        {
            cout << "Impossible" << endl;
        }
        else
        {
            cout << max<int64_t>(0, requiredTotalScoreForEnrollment - sergeysScore) << endl;
        }
    }

    assert(cin);
}
