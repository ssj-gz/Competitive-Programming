// Simon St James (ssjgz) - 2019-04-09
#include <iostream>
#include <vector>
#include <map>

#include <sys/time.h>

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


    for (int startPos = 0; startPos < n; startPos++)
    {
        numCompletedIfStartAt.push_back(numStudentsCompleted);

        blah[extraTimeNeededForStudent[startPos] - startPos]--;
        if (extraTimeNeededForStudent[startPos] - startPos > 0)
        {
            numStudentsCompleted--;
        }
        const int blee = extraTimeNeededForStudent[startPos] + n - startPos;
        blah[blee]++;
        if (blee + n - startPos <= 0)
        {
            numStudentsCompleted++;
        }
        numStudentsCompleted -= blah[startPos];
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
    for (const auto x : solutionBruteForce)
    {
        cout << x << " ";
    }
    cout << endl;
    const auto solutionOptimised = solveOptimised(extraTimeNeededForStudent);
    for (const auto x : solutionOptimised)
    {
        cout << x << " ";
    }
    cout << endl;
}
