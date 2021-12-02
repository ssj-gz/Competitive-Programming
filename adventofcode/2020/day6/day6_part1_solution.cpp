#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int64_t numQuestionsYesForAnyoneInGroup( const vector<string>& answersForGroup )
{
    constexpr int totalNumQuestions = 26;
    bool someoneAnsweredYesToQuestion[totalNumQuestions] = {};
    for (const auto& personsYesAnswers : answersForGroup)
    {
        for (const auto question : personsYesAnswers)
        {
            someoneAnsweredYesToQuestion[question - 'a'] = true;
        }
    }
    return count(std::begin(someoneAnsweredYesToQuestion), std::end(someoneAnsweredYesToQuestion), true);
}

int main()
{
    vector<string> answersForGroup;
    int64_t totalAnsweredYesAcrossGroups = 0;
    while (cin)
    {
        string line;
        std::getline(cin, line);
        if (cin && !line.empty())
        {
            answersForGroup.push_back(line);
        }
        else
        {
            totalAnsweredYesAcrossGroups += numQuestionsYesForAnyoneInGroup(answersForGroup);
            answersForGroup.clear();
        }
    }
    cout << totalAnsweredYesAcrossGroups << endl;
}
