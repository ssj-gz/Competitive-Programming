// Simon St James (ssjgz) - 2019-12-22
// 
// Solution to: https://www.codechef.com/problems/CLEANUP
//
#include <iostream>
#include <vector>
#include <set>
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

std::pair<vector<int>, vector<int>> findChefAndAssistantJobs(const int numJobs, const vector<int>& completedJobs)
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

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int numJobs = read<int>();
        const int numCompletedJobs = read<int>();
        vector<int> completedJobs(numCompletedJobs);
        for(auto& jobId : completedJobs)
            jobId = read<int>();

        const auto result = findChefAndAssistantJobs(numJobs, completedJobs);
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

        printVector(result.first); // Chef's Jobs.
        printVector(result.second); // Assistant's Jobs.
    }

    assert(cin);
}
