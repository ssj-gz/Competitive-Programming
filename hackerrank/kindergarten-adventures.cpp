// Simon St James (ssjgz) - 2019-07-10
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#include <sys/time.h>
#include <cassert>

using namespace std;

vector<int> solveBruteForce(const vector<int>& extraTimeNeededForStudent)
{
    vector<int> numCompletedIfStartAt;
    const int n = extraTimeNeededForStudent.size();

    for (int startPos = 0; startPos < n; startPos++)
    {
        int numStudentsCompleted = 0;
        int studentId = startPos;
        for (int timeElapsed = 0; timeElapsed < n; timeElapsed++)
        {
            cout << " studentId: " << studentId << " startPos:" << startPos << " timeElapsed: " << timeElapsed << " extraTimeNeededForStudent: " << extraTimeNeededForStudent[studentId] << endl;
            if ( timeElapsed >= extraTimeNeededForStudent[studentId])
            {
                numStudentsCompleted++;
            }
            studentId = (studentId + 1) % n;
        }
        cout << "startPos: " << startPos << " numStudentsCompleted:" << numStudentsCompleted << endl;
        numCompletedIfStartAt.push_back(numStudentsCompleted);
    }

    return numCompletedIfStartAt;
}

vector<int> solveOptimised(const vector<int>& extraTimeNeededForStudent)
{
    vector<int> numCompletedIfStartAt;
    const int n = extraTimeNeededForStudent.size();

    // The numNeedingTimeWhenTeacherVisitedAdjusted has the following property:
    // if we started at student startPos, then 
    //  
    //   numNeedingTimeWhenTeacherVisitedAdjusted[x - startPos] 
    //
    // will be the number of students who need time x to finish by the time
    // the teacher visits them.
    map<int, int> numNeedingTimeWhenTeacherVisitedAdjusted;
    int numStudentsCompleted = 0;
    for (int i = 0; i < n; i++)
    {
        const int timeRequiredWhenTeacherVisits = extraTimeNeededForStudent[i] - i;
        if (timeRequiredWhenTeacherVisits <= 0)
            numStudentsCompleted++;

        numNeedingTimeWhenTeacherVisitedAdjusted[timeRequiredWhenTeacherVisits]++;
    }

    auto subtractFromBlah = [&numNeedingTimeWhenTeacherVisitedAdjusted](int index, int amount)
    {
        numNeedingTimeWhenTeacherVisitedAdjusted[index] -= amount;
        assert(numNeedingTimeWhenTeacherVisitedAdjusted[index] >= 0);
        if (numNeedingTimeWhenTeacherVisitedAdjusted[index] == 0)
            numNeedingTimeWhenTeacherVisitedAdjusted.erase(numNeedingTimeWhenTeacherVisitedAdjusted.find(index));

    };


    for (int startPos = 0; startPos < n; startPos++)
    {
        numCompletedIfStartAt.push_back(numStudentsCompleted);

        // Update lookups/ calculations resulting from the move of
        // starting position from startPos to startPos + 1.
        // Remove contribution of the student at startPos + 1 from numNeedingTimeWhenTeacherVisitedAdjusted.
        subtractFromBlah(extraTimeNeededForStudent[startPos] - startPos, 1);
        if (extraTimeNeededForStudent[startPos] > 0)
        {
            // When starting at startPos, the student at startPos won't have finished;
            // remove him from the list of failures.
            numStudentsCompleted++;
        }
        // All students who currently need 0 time when teacher visits them when teacher starts at startPos will
        // fail when we start from startPos.
        const int numNewFailures = numNeedingTimeWhenTeacherVisitedAdjusted[-startPos];
        numStudentsCompleted -= numNewFailures;
        // Re-add student to numNeedingTimeWhenTeacherVisitedAdjusted - if starting at (startPos + 1), querying the time remaining
        // when the teacher visits the student at startPos give the correct time remaining for this student, which will be 
        // extraTimeNeededForStudent[startPos] - (n - 1); i.e. we need to increase the value of 
        // numNeedingTimeWhenTeacherVisitedAdjusted[x - (startPos + 1)], where x = extraTimeNeededForStudent[startPos] - (n - 1).
        numNeedingTimeWhenTeacherVisitedAdjusted[extraTimeNeededForStudent[startPos] - (n - 1) -(startPos + 1)]++;

        if (extraTimeNeededForStudent[startPos] - (n - 1) > 0)
        {
            // When starting from startPos + 1, the student at startPos still won't have finished.
            numStudentsCompleted--;
        }
    }

    return numCompletedIfStartAt;
}

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));


        const int n = rand() % 100 + 1;
        cout << n << endl;

        const int maxTime = rand() % 1000 + 1;
        for (int i = 0; i < n; i++)
        {
            cout << ((rand() % maxTime) + 1) << " ";
        }
        cout << endl;

        return 0;
    }
    int n;
    cin >> n;

    vector<int> extraTimeNeededForStudent(n);
    for (int i = 0; i < n; i++)
    {
        cin >> extraTimeNeededForStudent[i];
    }

#ifdef BRUTE_FORCE
    const auto solutionBruteForce = solveBruteForce(extraTimeNeededForStudent);
    const auto solutionOptimised = solveOptimised(extraTimeNeededForStudent);
    assert(solutionOptimised == solutionBruteForce);
    int bestStartingPosition = -1;

    const auto maxStudentsPassing = *max_element(solutionOptimised.begin(), solutionOptimised.end());
    for (int startPos = 0; startPos < n; startPos++)
    {
        if (solutionBruteForce[startPos] == maxStudentsPassing)
        {
            bestStartingPosition = startPos;
            break;
        }
    }
    // We "+ 1" as the answer is expected to be 1-relative.
    cout << "solutionBruteForce: " << (bestStartingPosition + 1) << endl;
#else
    const auto solutionOptimised = solveOptimised(extraTimeNeededForStudent);
    const auto maxStudentsPassing = *max_element(solutionOptimised.begin(), solutionOptimised.end());
    int bestStartingPosition = -1;

    for (int startPos = 0; startPos < n; startPos++)
    {
        if (solutionOptimised[startPos] == maxStudentsPassing)
        {
            bestStartingPosition = startPos;
            break;
        }
    }

    // We "+ 1" as the answer is expected to be 1-relative.
    cout << (bestStartingPosition + 1) << endl;

#endif
}
