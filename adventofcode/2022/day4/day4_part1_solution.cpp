#include <iostream>
#include <regex>
#include <cassert>

using namespace std;

struct Range
{
    int begin = -1;
    int end = -1;
    bool fullyContains(const Range& otherRange) const
    {
        if ((this->begin <= otherRange.begin) && 
                (this->end >= otherRange.end))
        {
            return true;
        }
        return false;
    }
};

int main()
{
    regex pairAssignmentsRegex(R"(^(\d+)-(\d+),(\d+)-(\d+)$)");
    std::smatch pairAssignmentMatch;
    string pairAssignmentsLine;
    int numPairsWithNestedRanges = 0;
    while (std::getline(cin, pairAssignmentsLine))
    {
        const bool matched = std::regex_match(pairAssignmentsLine, pairAssignmentMatch, pairAssignmentsRegex);
        assert(matched);
        assert(pairAssignmentMatch.size() == 5);
        Range firstRange;
        firstRange.begin = stoi(pairAssignmentMatch[1].str());
        firstRange.end = stoi(pairAssignmentMatch[2].str());
        Range secondRange;
        secondRange.begin = stoi(pairAssignmentMatch[3].str());
        secondRange.end = stoi(pairAssignmentMatch[4].str());

        if (firstRange.fullyContains(secondRange) || secondRange.fullyContains(firstRange))
        {
            numPairsWithNestedRanges++;
        }
    }

    std::cout << numPairsWithNestedRanges << std::endl;
}
