// Simon St James (ssjgz) - 2019-07-10
#include <iostream>
#include <vector>
#include <map>

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

    map<int, int> blah;
    int numStudentsCompleted = 0;
    for (int i = 0; i < n; i++)
    {
        const int timeRequiredWhenTeacherVisits = extraTimeNeededForStudent[i] - i;
        if (timeRequiredWhenTeacherVisits <= 0)
            numStudentsCompleted++;

        blah[timeRequiredWhenTeacherVisits]++;
    }

    auto subtractFromBlah = [&blah](int index, int amount)
    {
        blah[index] -= amount;
        assert(blah[index] >= 0);
        if (blah[index] == 0)
            blah.erase(blah.find(index));

    };


    for (int startPos = 0; startPos < n; startPos++)
    {
        numCompletedIfStartAt.push_back(numStudentsCompleted);

        // Update lookups/ calculations resulting from the move of
        // starting position from startPos to startPos + 1.
        // Remove contribution of the student at startPos + 1 from blah.
        subtractFromBlah(extraTimeNeededForStudent[startPos] - startPos, 1);
        if (extraTimeNeededForStudent[startPos] > 0)
        {
            // When starting at startPos, the student at startPos won't have finished;
            // remove him from the list of failures.
            numStudentsCompleted++;
        }
        const int studentFailThreshold = -(startPos); 
        if (blah.find(studentFailThreshold) != blah.end())
            numStudentsCompleted -= blah[studentFailThreshold];
        // Re-add student to blah - if starting at (startPos + 1), querying the time remaining
        // when the teacher visits the student at startPos (via blah[-(startPos + 1)]) should
        // give the correct time remaining for this student, which will be 
        // extraTimeNeededForStudent[startPos] - (n - 1). 
        blah[extraTimeNeededForStudent[startPos] - (n - 1) -(startPos + 1)]++;

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

    const auto solutionBruteForce = solveBruteForce(extraTimeNeededForStudent);
    cout << "solutionBruteForce: ";
    for (const auto x : solutionBruteForce)
    {
        cout << x << " ";
    }
    cout << endl;
    const auto solutionOptimised = solveOptimised(extraTimeNeededForStudent);
    cout << "solutionBruteForce: ";
    for (const auto x : solutionOptimised)
    {
        cout << x << " ";
    }
    cout << endl;
    assert(solutionOptimised == solutionBruteForce);
}
