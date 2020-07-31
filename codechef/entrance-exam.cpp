// Simon St James (ssjgz) - 2020-07-31
// 
// Solution to: https://www.codechef.com/problems/ENTEXAM
//
//#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#else
#define _GLIBCXX_DEBUG       // Iterator safety; out-of-bounds access for Containers, etc.
#pragma GCC optimize "trapv" // abort() on (signed) integer overflow.
#define BRUTE_FORCE
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
            const auto numStudents = 2 + rand() % 10'000;
            //const auto maxEnrollableStudents = 1 + rand() % (numStudents - 1);
            const auto maxEnrollableStudents = numStudents - 1;
            assert(maxEnrollableStudents < numStudents);
            //const auto numExams = 1 + rand() % 4;
            const auto numExams = 4;
            //const auto maxExamScore = 1 + rand() % 1'000'000'000;
            const auto maxExamScore = 1'000'000'000;

            cout << numStudents << " " << maxEnrollableStudents << " " << numExams << " " << maxExamScore << endl;

            for (int otherStudentIndex = 0; otherStudentIndex < numStudents - 1; otherStudentIndex++)
            {
                for (int examIndex = 0; examIndex < numExams; examIndex++)
                {
#if 0
                    const auto score = rand() % (maxExamScore + 1);
                    cout << score;
                    if (examIndex != numExams - 1)
                        cout << " ";
#else
                    int64_t score = 0;
                    if (rand() % 2 == 0)
                        score = rand() % 1000;
                    else
                        score = maxExamScore - rand() % 1000;
                    cout << score;
                    if (examIndex != numExams - 1)
                        cout << " ";
#endif
                }
                cout << endl;
            }
            for (int examIndex = 0; examIndex < numExams - 1; examIndex++)
            {
                const auto score = rand() % (maxExamScore + 1);
                cout << score;
                if (examIndex != numExams - 1)
                    cout << " ";
            }

        }

        return 0;
    }
    
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
        cout << "requiredTotalScoreForEnrollment: " << requiredTotalScoreForEnrollment << endl;
        for (const auto x  : otherStudentScoresSorted)
        {
            cout << x << " ";
        }
        cout << endl;
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
