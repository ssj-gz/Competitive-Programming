#include <iostream>
#include <vector>

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

int main()
{
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
}
