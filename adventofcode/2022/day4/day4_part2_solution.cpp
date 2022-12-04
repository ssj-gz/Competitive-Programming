#include <iostream>
#include <regex>
#include <cassert>

using namespace std;

struct Range
{
    int begin = -1;
    int end = -1;
    bool overlaps(const Range& otherRange) const
    {
        if ((this->end < otherRange.begin) || 
                (this->begin > otherRange.end))
        {
            return false;
        }
        return true;
    }
};

int main()
{
    regex pairAssignmentsRegex(R"(^(\d+)-(\d+),(\d+)-(\d+)$)");
    std::smatch pairAssignmentMatch;
    string pairAssignmentsLine;
    int numPairsWithOverlappingRanges = 0;
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

        if (firstRange.overlaps(secondRange))
        {
            numPairsWithOverlappingRanges++;
        }
    }

    std::cout << numPairsWithOverlappingRanges << std::endl;
}
