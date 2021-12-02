#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int64_t numQuestionsYesForAnyoneInGroup( const vector<string>& answersForGroup )
{
    constexpr int totalNumQuestions = 26;
    int numPeopleAnsweredQuestionYes[totalNumQuestions] = {};
    for (const auto& personsYesAnswers : answersForGroup)
    {
        for (const auto question : personsYesAnswers)
        {
            numPeopleAnsweredQuestionYes[question - 'a']++;
        }
    }
    return count(std::begin(numPeopleAnsweredQuestionYes), std::end(numPeopleAnsweredQuestionYes), answersForGroup.size());
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
