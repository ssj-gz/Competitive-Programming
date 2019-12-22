// Simon St James (ssjgz) - 2019-12-22
// 
// Solution to: https://www.codechef.com/problems/CLEANUP
//
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <set>
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

#if 1
std::pair<vector<int>, vector<int>> solveBruteForce(const int numJobs, const vector<int>& completedJobs)
{
    vector<int> chefJobs;
    vector<int> assistantJobs;

    set<int> allJobs;
    for (int i = 1; i<= numJobs; i++)
    {
        allJobs.insert(i);
    }
    for (const auto completedJob : completedJobs)
    {
        allJobs.erase(completedJob);
    }
    vector<int> remainingJobs(allJobs.begin(), allJobs.end());

    for (int i = 0; i < allJobs.size(); i += 2)
    {
        chefJobs.push_back(remainingJobs[i]);
    }
    for (int i = 1; i < allJobs.size(); i += 2)
    {
        assistantJobs.push_back(remainingJobs[i]);
    }
    
    return {chefJobs, assistantJobs};
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
            const int numJobs = rand() % 20;
            const int numCompletedJobs = rand() % (numJobs + 1);

            vector<int> allJobs;
            for (int i = 1; i <= numJobs; i++)
            {
                allJobs.push_back(i);
            }
            random_shuffle(allJobs.begin(), allJobs.end());
            vector<int> completedJobs(allJobs.begin(), allJobs.begin() + numCompletedJobs);

            cout << numJobs << " " << numCompletedJobs << endl;

            for (int i = 0; i < completedJobs.size(); i++)
            {
                cout << completedJobs[i];
                if (i != completedJobs.size() - 1)
                    cout << " ";
            }
            cout << endl;
        }

        return 0;
    }
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int numJobs = read<int>();
        const int numCompletedJobs = read<int>();
        vector<int> completedJobs(numCompletedJobs);
        for(auto& jobId : completedJobs)
            jobId = read<int>();

        const auto result = solveBruteForce(numJobs, completedJobs);
        auto printVector = [](const vector<int>& toPrint)
        {
            for (int i = 0; i < toPrint.size(); i++)
            {
                cout << toPrint[i];
                if (i != toPrint.size() - 1)
                    cout << " ";
            }
            cout << endl;
        };

        printVector(result.first);
        printVector(result.second);
    }

    assert(cin);
}
