// Simon St James (ssjgz) - 2019-07-10
#include <iostream>
#include <vector>
#include <algorithm>

#include <cassert>

using namespace std;

/**
 * Simple vector-ish data structure that allows negative indices.
 */
template<typename T>
class Vec
{
    public:
        Vec(int minIndex, int maxIndex)
            : m_minIndex(minIndex),
            m_maxIndex(maxIndex),
            m_numElements(maxIndex - minIndex + 1),
            m_vector(m_numElements)
    {

    }
        int minIndex() const
        {
            return m_minIndex;
        }
        int maxIndex() const
        {
            return m_maxIndex;
        }
        T& operator[](int index)
        {
            assert(index >= m_minIndex);
            assert(index <= m_maxIndex);
            assert(index - m_minIndex < m_vector.size());
            return m_vector[index - m_minIndex];
        }
        const T& operator[](int index) const
        {
            assert(index >= m_minIndex);
            assert(index <= m_maxIndex);
            assert(index - m_minIndex < m_vector.size());
            return m_vector[index - m_minIndex];
        }
    private:
        int m_minIndex = -1;
        int m_maxIndex = -1;
        int m_numElements = -1;
        vector<T> m_vector;
};


vector<int> findNumCompletedIfStartAt(const vector<int>& extraTimeNeededForStudent)
{
    vector<int> numCompletedIfStartAt;
    const int n = extraTimeNeededForStudent.size();

    // The numNeedingTimeWhenTeacherVisitedAdjusted has the following property:
    // 
    //    if teacher started at student startPos, then 
    //  
    //      numNeedingTimeWhenTeacherVisitedAdjusted[x - startPos] 
    //
    //    will be the number of students who need time x to finish by the time
    //    the teacher visits them.
    //
    // The min and max index for numNeedingTimeWhenTeacherVisitedAdjusted are derived
    // from inspecting the code: we never query for an index < 2 * n, nor an index
    // > n.
    Vec<int> numNeedingTimeWhenTeacherVisitedAdjusted(-(2 * n), n);
    int numStudentsCompleted = 0;
    for (int i = 0; i < n; i++)
    {
        assert(0 <= extraTimeNeededForStudent[i] && extraTimeNeededForStudent[i] <= n);
        const int timeRequiredWhenTeacherVisits = extraTimeNeededForStudent[i] - i;
        if (timeRequiredWhenTeacherVisits <= 0)
            numStudentsCompleted++;

        numNeedingTimeWhenTeacherVisitedAdjusted[timeRequiredWhenTeacherVisits]++;
    }

    for (int startPos = 0; startPos < n; startPos++)
    {
        numCompletedIfStartAt.push_back(numStudentsCompleted);

        // Update lookups/ calculations resulting from the move of
        // starting position from startPos to startPos + 1.
        // Remove contribution of the student at startPos + 1 from numNeedingTimeWhenTeacherVisitedAdjusted.
        numNeedingTimeWhenTeacherVisitedAdjusted[extraTimeNeededForStudent[startPos] - startPos]--;
        if (extraTimeNeededForStudent[startPos] > 0)
        {
            // When starting at startPos, the student at startPos won't have finished;
            // remove him from the list of failures.
            numStudentsCompleted++;
        }
        // All students who currently need 0 time when teacher visits them when teacher starts at startPos will
        // fail when teacher starts from startPos + 1.
        const int numNewFailures = numNeedingTimeWhenTeacherVisitedAdjusted[-startPos];
        numStudentsCompleted -= numNewFailures;
        // Re-add student to numNeedingTimeWhenTeacherVisitedAdjusted - if starting at (startPos + 1), querying the time remaining
        // when the teacher visits the student at startPos should give the correct time remaining for this student, which will be 
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
    // Fundamentally easy, but the devil is in the details - actually took
    // me quite a long time to implement: if I'd sat down with pen and paper
    // instead of trying to do it all in my head, it would have been quicker XD
    //
    // The number of students completed when the teacher visits them when starting at
    // student startPos + 1 can be derived from the number completed when teacher
    // starts at startPos: those who *just* passed when starting from startPos - i.e
    // those who, when the time arrives, need precisely 0 additional time to finish (where 
    // "additional time to finish" can be negative, in the case of students who have
    // finished before the teacher visits them) will *not* pass when the teacher visits
    // them but starts at startPos + 1.  If we can compute the number who need precisely
    // 0 additional time, we'd be mostly done: however, the student currently at startPos
    // needs some special treatment, but we'll ignore them for now (see the code
    // for how to actually deal with them!).
    //
    // Let's say we knew, for each student, the additional time they'd need when the teacher
    // visits them while starting at startPos.  This time will increase by 1 for each student 
    // (again, ignoring the problematic student at startPos) when the teacher visits 
    // them while staring at startPos + 1.  Obviously, adding 1 to the additional time required
    // by each student would make this O(n ^ 2), but we can use the time-honoured trick
    // of "pretending" to add 1 by adjusting how we query this information.
    //
    // That's about it - the code is probably a better description of it all works from here on 
    // in.  numNeedingTimeWhenTeacherVisitedAdjusted is used for finding the number of students
    // who "just passed" when teacher starts at startPos, and so will fail when teacher starts from
    // startPos + 1 (numNewFailures).  We handle adding "1" to all values in this as startPos
    // increases by changing the query (if we want to find the number needing time t when 
    // teacher starts at startPos, we query for numNeedingTimeWhenTeacherVisitedAdjusted[x - startPos] instead
    // of numNeedingTimeWhenTeacherVisitedAdjusted[x]).  
    // 
    // The total complexity is O(n).
    int n;
    cin >> n;

    vector<int> extraTimeNeededForStudent(n);
    for (int i = 0; i < n; i++)
    {
        cin >> extraTimeNeededForStudent[i];
    }

    const auto numCompletedIfStartAt = findNumCompletedIfStartAt(extraTimeNeededForStudent);
    const auto maxStudentsCompleted = *max_element(numCompletedIfStartAt.begin(), numCompletedIfStartAt.end());
    int bestStartingPosition = -1;

    for (int startPos = 0; startPos < n; startPos++)
    {
        if (numCompletedIfStartAt[startPos] == maxStudentsCompleted)
        {
            bestStartingPosition = startPos;
            break;
        }
    }

    // We "+ 1" as the answer is expected to be 1-relative.
    cout << (bestStartingPosition + 1) << endl;
}
