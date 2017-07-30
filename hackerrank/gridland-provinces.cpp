#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <cassert>

using namespace std;

struct Pos
{
    Pos(int x, int y)
        : x(x), y(y)
    {
    }
    int x = -1;
    int y = -1;
};
struct Next;
// Represents the successor(s) of a ProvinceExplorer when following its Path.
// These successors are themselves ProvinceExplorers, but for efficiency we
// deal with their Ranks instead.
struct NextRanked
{
    bool isValid = true;
    bool twoNext = false; // A ProvinceExplorer will either create one or two successors.
    int next1rank;
    int next1letterIndex;
    int next2rank;
    int next2letterIndex;
    static NextRanked invalid()
    {
        NextRanked invalid;
        invalid.isValid = false;
        return invalid;
    }
};
// A bot/ automaton kind of thing that follows a Path in the Province - or rather,
// it gives one or more successor ProvinceExplorer's along the Path when next() is called.
class ProvinceExplorer
{
    public:
        enum Phase { Uninitialised, BeginLoop, Wiggle, EndLoop } phase;
        // The core attributes of a ProvinceExplorer.
        int x, y;
        int numSegments, progress;
        int startRow;
        enum Direction {Left, Right, None} startDirection;

        int rank() const
        {
            // Map any possible ProvinceExplorer to a unique number in range [0, maxRank].
            switch (phase)
            {
                case Uninitialised:
                    return x + y * N;
                case BeginLoop:
                    {
                        int rank = rankStartDirRowSegmentsAndProgress(*this);
                        rank += numUninitialisedRanks;
                        return rank;
                    }
                case Wiggle:
                    {
                        assert(progress >= 1 && progress <= 2 * N);
                        const int rank = (numUninitialisedRanks + numBeginLoopRanks) + (2 * startDirection + startRow) * (2 * N) + (progress - 1);
                        assert(rank >= numUninitialisedRanks + numBeginLoopRanks && rank < numBeginLoopRanks + numUninitialisedRanks + numWiggleRanks);
                        return rank;
                    }
                case EndLoop:
                    {
                        int rank = rankStartDirRowSegmentsAndProgress(*this);
                        rank += numUninitialisedRanks + numBeginLoopRanks + numWiggleRanks;
                        return rank;
                    }
            }
            return -1;
        }
        static ProvinceExplorer unrank(int rank)
        {
            // Opposite of rank() ;)
            const Phase phase = unrankedPhase(rank);
            switch (phase)
            {
                case Uninitialised:
                {
                    ProvinceExplorer unranked;
                    unranked.phase = Uninitialised;
                    unranked.y = rank / N;
                    unranked.x = rank % N;
                    return unranked;
                }
                case BeginLoop:
                {
                    rank -= numUninitialisedRanks;
                    ProvinceExplorer unranked;
                    unranked.phase = BeginLoop;
                    unrankStartDirRowSegmentsAndProgress(rank, unranked);
                    return unranked;
                }
                case Wiggle:
                {
                    rank -= numUninitialisedRanks + numBeginLoopRanks;
                    ProvinceExplorer unranked;
                    unranked.phase = Wiggle;
                    const int rankedStartDirAndRow = rank / (2 * N);
                    unranked.startDirection = static_cast<Direction>(rankedStartDirAndRow / 2);
                    unranked.startRow = rankedStartDirAndRow % 2;
                    unranked.progress = rank % (2 * N) + 1;
                    assert(unranked.progress >= 1 && unranked.progress <= 2 * N);
                    return unranked;
                }
                case EndLoop:
                {
                    rank -= numUninitialisedRanks + numBeginLoopRanks + numWiggleRanks;
                    ProvinceExplorer unranked;
                    unranked.phase = EndLoop;
                    unrankStartDirRowSegmentsAndProgress(rank, unranked);
                    return unranked;
                }
                default:
                    assert(false);
            }
            return ProvinceExplorer();
        }
        static Phase unrankedPhase(int rank)
        {
            if (rank < numUninitialisedRanks)
            {
                return Uninitialised;
            }
            rank -= numUninitialisedRanks;
            if (rank < numBeginLoopRanks)
            {
                return BeginLoop;
            }
            rank -= numBeginLoopRanks;
            if (rank < numWiggleRanks)
            {
                return Wiggle;
            }
            return EndLoop;
        }
        Pos computePos() const
        {
            switch (phase)
            {
                case Uninitialised:
                    return Pos(x, y);
                case BeginLoop:
                case EndLoop:
                    {
                        int x = -1;
                        int y = -1;
                        if (startDirection == Left)
                            x = numSegments - 1;
                        else
                            x  = N - numSegments;
                        y = startRow;
                        if (progress > numSegments)
                        {
                            y = 1 - startRow;
                            const int reverseDirProgress = progress - numSegments - 1;
                            if (startDirection == Left)
                                x = reverseDirProgress;
                            else
                                x = N - 1 - reverseDirProgress;
                        }
                        else
                        {
                            if (startDirection == Left)
                                x -= progress - 1;
                            else
                                x += progress - 1;
                        }
                        return Pos(x, y);
                    }
                case Wiggle:
                    {
                        int y = startRow;
                        const int numVerticalMoves = progress / 2;
                        if ((numVerticalMoves % 2) == 1)
                            y = 1 - startRow;
                        int x = -1;
                        const int numHorizontalMoves = (progress - 1) / 2;
                        if (startDirection == Left)
                            x = N - 1 - numHorizontalMoves;
                        else
                            x = numHorizontalMoves;
                        return Pos(x, y);
                    }
            }
            return Pos(-1, -1);
        };
        static void unrankStartDirRowSegmentsAndProgress(int rank, ProvinceExplorer& unranked)
        {
            const int rankedStartDirAndRow = rank / numSegmentsAndProgress;
            unranked.startDirection = static_cast<Direction>(rankedStartDirAndRow / 2);
            unranked.startRow = rankedStartDirAndRow % 2;
            rank = rank % numSegmentsAndProgress;
            const auto& progressAndNumSegments = progressAndNumSegmentsFromRank[rank];
            unranked.progress = progressAndNumSegments.first;
            unranked.numSegments = progressAndNumSegments.second;
            assert(unranked.numSegments >= 1);
            assert(unranked.numSegments <= N);
            assert(unranked.progress >= 1);
            assert(unranked.progress <= 2 * unranked.numSegments);
        }
        static int rankStartDirRowSegmentsAndProgress(const ProvinceExplorer& toRank)
        {
            const int progress = toRank.progress;    
            const int numSegments = toRank.numSegments;    
            const int startDirection = toRank.startDirection;    
            const int startRow = toRank.startRow;    
            assert(progress >= 1 && progress <= 2 * numSegments);
            const int rankedStartDirAndRow = (2 * startDirection + startRow) * numSegmentsAndProgress;
            const int rankedTotalStepsAndProgress = (numSegments - 1) * numSegments + (progress - 1);
            assert(rankedTotalStepsAndProgress < numSegmentsAndProgress);
            const int rank = rankedStartDirAndRow + rankedTotalStepsAndProgress;
            assert(rank >= 0 && rank < numBeginLoopRanks);
            return rank;
        }
        Next next() const;
        static void setProvince(const vector<string>& province);
        static int countUniquePaths();
        static void countUniquePathsAux(int numLettersMatched, int currentMatchStartIndex, int currentMatchEndIndex, int nextMatchStartIndex);
        static void calculateProvinceExplorerRankingData();
        static void reduceProvinceAlphabet();
        static void buildRankedProvinceExplorerSuccessorTable();
        static void buildSimpleSubProblemLookup();
        static const int maxNumLetters = 26;


        static int N;
        static vector<string> province;

        static int provinceAlphabetSize;

        static int answer;

        // Data for the perfect ranking/ unranking of ProvinceExplorers.
        static int numUninitialisedRanks;
        static int numSegmentsAndProgress;
        static int numBeginLoopRanks;
        static int numWiggleRanks;
        static int numEndLoopRanks;
        static int maxRank;
        static vector<pair<int, int>> progressAndNumSegmentsFromRank;
        // The main "next ranked ProvinceExplorer(s) for this ranked ProvinceExplorer" 
        // lookup - does a lot of the heavy lifting! Generated by buildRankedProvinceExplorerSuccessorTable().
        static vector<NextRanked> nextForRanked;

        // The lookup table for the (much, much simpler) subproblem frequently encountered 
        // in the search for an answer - see buildSimpleSubProblemLookup().
        static vector<int> numStringsAfterUniqueWiggle[2][2];

        static bool hasHorizontalSymmetry;

        // Stuff used by countUniquePathsAux().
        // currentMatchStack is how ranked ProvinceExplorers are passed along in recursive
        // calls to countUniquePathsAux().
        static vector<int> currentMatchStack;
        // Data that would ideally be stored on the stack, but is too big so we make it
        // a static and ensure that it is clear before we recurse into the next countUniquePathsAux().
        struct TempWorkingSet 
        {
            vector<char> seenMatch; // vector<char> is faster than vector<bool>.
            vector<int> matches;
            vector<vector<int>> matchesForLetter;
        };
        static TempWorkingSet tempWorkingSet;
        // Data concerning the passing of ranked ProvinceExplorers via currentMatchStack.
        struct MatchLetterInfo
        {
            struct MatchLetterItem
            {
                int letterIndex;
                int startIndex;
                int endIndex;
            };
            MatchLetterItem matchLetterItems[maxNumLetters];
            int numMatchLetterItems = 0;
        };

};

int ProvinceExplorer::numUninitialisedRanks;
int ProvinceExplorer::numSegmentsAndProgress;
int ProvinceExplorer::numBeginLoopRanks;
int ProvinceExplorer::numWiggleRanks;
int ProvinceExplorer::numEndLoopRanks;
int ProvinceExplorer::maxRank;
int ProvinceExplorer::N;
int ProvinceExplorer::answer;
bool ProvinceExplorer::hasHorizontalSymmetry;
const int ProvinceExplorer::maxNumLetters;
vector<string> ProvinceExplorer::province;
int ProvinceExplorer::provinceAlphabetSize;
vector<int> ProvinceExplorer::currentMatchStack;
ProvinceExplorer::TempWorkingSet ProvinceExplorer::tempWorkingSet;
vector<NextRanked> ProvinceExplorer::nextForRanked;
vector<int> ProvinceExplorer::numStringsAfterUniqueWiggle[2][2];
vector<pair<int, int>> ProvinceExplorer::progressAndNumSegmentsFromRank;

// The unoptimal version of NextRanked :)
struct Next
{
    static Next invalid()
    {
        Next invalid;
        invalid.isValid = false;
        return invalid;
    }
    bool isValid = true;
    bool twoNext = false;
    ProvinceExplorer next1;
    int nextLetterIndex1;
    ProvinceExplorer next2;
    int nextLetterIndex2;
};

void ProvinceExplorer::setProvince(const vector<string>& province)
{
    assert(province.size() == 2);
    const int N = province[0].size();
    ProvinceExplorer::N = N;
    ProvinceExplorer::province = province;

    reduceProvinceAlphabet();
    calculateProvinceExplorerRankingData();

    hasHorizontalSymmetry = (province[0] == string(province[0].rbegin(), province[0].rend()) &&
                             province[1] == string(province[1].rbegin(), province[1].rend()));

    buildRankedProvinceExplorerSuccessorTable();
               
    buildSimpleSubProblemLookup();
}

int ProvinceExplorer::countUniquePaths()
{
    currentMatchStack.resize(1'500'000);
    tempWorkingSet.seenMatch.resize(maxRank);
    tempWorkingSet.matchesForLetter.resize(maxNumLetters);

    assert(tempWorkingSet.matches.empty());
    answer = 0;
    currentMatchStack.push_back(-1); // Dummy.
    countUniquePathsAux(0, 0, 0, 1);
    return answer;
}

void ProvinceExplorer::countUniquePathsAux(int numLettersMatched, int currentMatchStartIndex, int currentMatchEndIndex, int nextMatchStartIndex)
{
    // Each "match" is the Rank of a ProvinceExplorer that has successfully followed the string built so far.
    if (numLettersMatched == 2 * N)
    {
        // Found a complete string.
        answer++;
        return;
    }
    const int numCurrentMatches = currentMatchEndIndex - currentMatchStartIndex + 1;
    if (numCurrentMatches == 1 && numLettersMatched > 0)
    {
        // We've followed a string such that there is only *one* path that produces that
        // string: we can optimise from here on in.
        const int match = currentMatchStack[currentMatchStartIndex];
        const Phase phase = unrankedPhase(match);
        if (phase == EndLoop)
        {
            // Only one possible continuation from here.
            answer++;
            return;
        }
        else if (phase == Wiggle)
        {
            // We have a special case where we essentially have a 2xM grid (M<=N) and want to 
            // fill it *starting from the first column* - this is a much simpler problem to 
            // solve, and we've constructed a lookup table of solutions (numStringsAfterUniqueWiggle)
            // for it elsewhere.
            const ProvinceExplorer wiggle = unrank(match);
            assert(wiggle.progress != 0);
            answer +=  numStringsAfterUniqueWiggle[wiggle.startDirection][wiggle.startRow][wiggle.progress];
            return;
        }
        else if (phase == BeginLoop)
        {
            ProvinceExplorer beginLoop = unrank(match);
            if (beginLoop.progress != 2 * beginLoop.numSegments)
            {
                // Advance to just before a Wiggle, and search from there.
                const int numLettersSkipped = 2 * beginLoop.numSegments - beginLoop.progress;
                beginLoop.progress = 2 * beginLoop.numSegments;
                currentMatchStack[nextMatchStartIndex] = beginLoop.rank();
                countUniquePathsAux(numLettersMatched + numLettersSkipped, nextMatchStartIndex, nextMatchStartIndex, nextMatchStartIndex + 1);
                return;
            }
        }
    }
    if (numCurrentMatches == 2)
    {
        // The case where there is two matches is quite common; let's optimise for it!
        const NextRanked& next1 = nextForRanked[currentMatchStack[currentMatchStartIndex]];
        const NextRanked& next2 = nextForRanked[currentMatchStack[currentMatchStartIndex + 1]];
        if (!next1.twoNext && !next2.twoNext)
        {
            if (next1.next1letterIndex == next2.next1letterIndex)
            {
                currentMatchStack[nextMatchStartIndex] = next1.next1rank;
                currentMatchStack[nextMatchStartIndex + 1] = next2.next1rank;
                countUniquePathsAux(numLettersMatched + 1, nextMatchStartIndex, nextMatchStartIndex + 1, nextMatchStartIndex + 2);
                return;
            }
            else
            {
                currentMatchStack[nextMatchStartIndex] = next1.next1rank;
                countUniquePathsAux(numLettersMatched + 1, nextMatchStartIndex, nextMatchStartIndex, nextMatchStartIndex + 1);
                currentMatchStack[nextMatchStartIndex] = next2.next1rank;
                countUniquePathsAux(numLettersMatched + 1, nextMatchStartIndex, nextMatchStartIndex, nextMatchStartIndex + 1);
                return;
            }
        }
    }
    auto addNextMatch = [](int match, int letterIndex)
    {
        if (tempWorkingSet.seenMatch[match])
            return;
        tempWorkingSet.seenMatch[match] = true;
        tempWorkingSet.matches.push_back(match);
        tempWorkingSet.matchesForLetter[letterIndex].push_back(match);
    };
    if (numLettersMatched != 0)
    {
        for (int i = currentMatchStartIndex; i <= currentMatchEndIndex; i++)
        {
            const int match = currentMatchStack[i];
            const NextRanked& nextRank = nextForRanked[match];
            addNextMatch(nextRank.next1rank, nextRank.next1letterIndex);
            if (nextRank.twoNext)
            {
                addNextMatch(nextRank.next2rank, nextRank.next2letterIndex);
            }
        }
    }
    else
    {
        // This is the first call to countUniquePathsAux: place our initial Explorers.
        for (int row = 0; row < 2; row++)
        {
            for (int col = 0; col < (hasHorizontalSymmetry ? (N + 1) / 2 : N); col++)
            {
                const int letterIndex = province[row][col] - 'a';
                ProvinceExplorer uninitialised;
                uninitialised.phase = Uninitialised;
                uninitialised.x = col;
                uninitialised.y = row;
                addNextMatch(uninitialised.rank(), letterIndex);
            }
        }
    }
    // Build up a list of ProvinceExplorers, collected by the letter they are sitting on :)
    // We dump them all into currentMatchStack for passing to further cals to countUniquePathsAux(), 
    // with a lookup that tells us the start and end indices in currentMatchStack for al ProvinceExplorers
    // sitting on each letter.
    MatchLetterInfo matchLetterInfo;
    int nextMatchIndex = nextMatchStartIndex;
    for (int letterIndex = 0; letterIndex < provinceAlphabetSize; letterIndex++)
    {
        vector<int>& matchesForLetter = tempWorkingSet.matchesForLetter[letterIndex];
        if (matchesForLetter.empty())
            continue;
        MatchLetterInfo::MatchLetterItem& currentMatchLetterItem = matchLetterInfo.matchLetterItems[matchLetterInfo.numMatchLetterItems];
        currentMatchLetterItem.letterIndex = letterIndex;
        currentMatchLetterItem.startIndex = nextMatchIndex;
        copy(matchesForLetter.begin(), matchesForLetter.end(), currentMatchStack.begin() + nextMatchIndex);
        nextMatchIndex += matchesForLetter.size();
        matchesForLetter.clear();
        currentMatchLetterItem.endIndex = nextMatchIndex - 1;
        matchLetterInfo.numMatchLetterItems++;
    }

    // Wipe tempWorkingSet in time for the next call.
    for (auto match : tempWorkingSet.matches)
    {
        tempWorkingSet.seenMatch[match] = false;
    }
    tempWorkingSet.matches.clear();

    for (int i = 0; i < matchLetterInfo.numMatchLetterItems; i++)
    {
        const MatchLetterInfo::MatchLetterItem& matchLetterItem = matchLetterInfo.matchLetterItems[i];
        {
            // Explore using the ProvinceExplorers for this letter.
            countUniquePathsAux(numLettersMatched + 1, matchLetterItem.startIndex, matchLetterItem.endIndex, nextMatchIndex);
        }
    }
}

// Given a province:
//   xyd
//   ada
//
// reduce to using the minimal letters in sequence
// starting from a i.e.
//   
//   cdb
//   aba
// 
// and update provinceAlphabetSize accordingly (4, in this
// example).
void ProvinceExplorer::reduceProvinceAlphabet()
{
    bool letterUsed[maxNumLetters] = {}; 
    for (int row = 0; row < 2; row++)
    {
        for (int col = 0; col < N; col++)
        {
            assert(province[row][col] >= 'a' && province[row][col] <= 'z');
            letterUsed[province[row][col] - 'a'] = true;
        }
    }
    int translatedLetter[maxNumLetters] = {};
    int numLettersUsed = 0;
    for (int i = 0; i < maxNumLetters; i++)
    {
        if (letterUsed[i])
        {
            translatedLetter[i] = numLettersUsed;
            numLettersUsed++;
        }
    }
    for (int row = 0; row < 2; row++)
    {
        for (auto& letter : province[row])
            letter = 'a' + (translatedLetter[letter - 'a']);
    }
    provinceAlphabetSize = numLettersUsed;
}

void ProvinceExplorer::buildRankedProvinceExplorerSuccessorTable()
{
    // Calculate a lookup table of the rank of the next successor(s)
    // for each ProvinceExplorer.  Note that it is often faster 
    // to manipulate rankings directly as opposed to taking a ProvinceExplorer
    // and calling .next() and then rank() on it, so we try to do that 
    // as much as possible.  Quite often, it is simpler a matter of 
    // adding "1", which corresponds to adding "1" to the progress of the 
    // ProvinceExplorer.
    nextForRanked.resize(maxRank);
    progressAndNumSegmentsFromRank.resize(numSegmentsAndProgress);
    for (int numSegments = 1; numSegments <= N; numSegments++)
    {
        for (int progress = 0; progress < 2 * numSegments; progress++)
            progressAndNumSegmentsFromRank[(numSegments - 1) * numSegments + progress] = make_pair(progress + 1, numSegments);
    }

    for (int startDirection = 0; startDirection <= 1; startDirection++)
    {
        for (int startRow = 0; startRow < 2; startRow++)
        {
            // Successors of begin/ end loops.
            for (int beginOrEnd = 0; beginOrEnd <= 2; beginOrEnd++)
            {
                const int loopStartDirAndRowRank = (beginOrEnd == 0 ? numUninitialisedRanks : numUninitialisedRanks + numBeginLoopRanks + numWiggleRanks)
                                                   + (startDirection * 2 + startRow) * numSegmentsAndProgress;
                for (int numSegments = 1; numSegments <= N; numSegments++)
                {
                    int x = (startDirection == Left) ? numSegments - 2 : N - numSegments + 1;
                    int y = startRow;
                    if (numSegments == 1)
                    {
                        // If we have only one segment, we reach "turning point" on the first move.
                        x += (startDirection == Left) ? 1 : -1;
                        y = (1 - y);
                    }
                    int loopStartDirRowAndNumSegmentsRank = loopStartDirAndRowRank + (numSegments - 1) * numSegments;
                    for (int progress = 0; progress < 2 * numSegments - 1; progress++)
                    {
                        const int loopRank = loopStartDirRowAndNumSegmentsRank + progress;
                        NextRanked& nextRank = nextForRanked[loopRank];
                        nextRank.twoNext = false;
                        nextRank.next1rank = loopStartDirRowAndNumSegmentsRank + (progress + 1);
                        assert(x >= 0 && x < N);
                        nextRank.next1letterIndex = ProvinceExplorer::province[y][x] - 'a';

                        if (progress < numSegments - 2)
                        {
                            if (startDirection == Left)
                            {
                                x--;
                            }
                            else
                            {
                                x++;
                            }
                        }
                        else if (progress == numSegments - 2)
                        {
                            y = 1 - y;
                        }
                        else
                        {
                            if (startDirection == Left)
                            {
                                x++;
                            }
                            else
                            {
                                x--;
                            }
                        }
                    }
                    ProvinceExplorer loopLast;
                    loopLast.phase = (beginOrEnd == 0 ? BeginLoop : EndLoop);
                    loopLast.numSegments = numSegments;
                    loopLast.progress = numSegments * 2;
                    loopLast.startDirection = static_cast<Direction>(startDirection);
                    loopLast.startRow = startRow;
                    Next next = loopLast.next();
                    NextRanked nextRank;
                    if (!next.isValid)
                    {
                        nextRank = NextRanked::invalid();
                    }
                    else
                    {
                        assert(next.twoNext);
                        nextRank.twoNext = true;
                        nextRank.next1rank = next.next1.rank();
                        const Pos next1Pos = next.next1.computePos();
                        nextRank.next1letterIndex = ProvinceExplorer::province[next1Pos.y][next1Pos.x] - 'a';
                        nextRank.next2rank = next.next2.rank();
                        const Pos next2Pos = next.next2.computePos();
                        nextRank.next2letterIndex = ProvinceExplorer::province[next2Pos.y][next2Pos.x] - 'a';
                    }
                    nextForRanked[loopLast.rank()] = nextRank;
                }
            }
        }
        if (hasHorizontalSymmetry)
            continue;
    }
    // Successors of Wiggles.
    for (int startDirection = 0; startDirection <= 1; startDirection++)
    {
        for (int startRow = 0; startRow < 2; startRow++)
        {
            const int wiggleStartDirAndRowRank = numUninitialisedRanks + numBeginLoopRanks + (startDirection * 2 + startRow) * (2 * N);
            ProvinceExplorer wiggle;
            wiggle.phase = Wiggle;
            wiggle.startDirection = static_cast<Direction>(startDirection);
            wiggle.startRow = startRow;
            for (int progress = 0; progress < 2 * N; progress++)
            {
                const int wiggleRank = wiggleStartDirAndRowRank + progress;
                wiggle.progress = progress + 1;
                Next next = wiggle.next();
                NextRanked nextRank;
                if (!next.isValid)
                    nextRank = NextRanked::invalid();
                else
                {
                    nextRank.twoNext = next.twoNext;
                    {
                        nextRank.next1rank = next.next1.rank();
                        const Pos next1Pos = next.next1.computePos();
                        nextRank.next1letterIndex = ProvinceExplorer::province[next1Pos.y][next1Pos.x] - 'a';
                    }
                    if (next.twoNext)
                    {
                        nextRank.next2rank = next.next2.rank();
                        const Pos next2Pos = next.next2.computePos();
                        nextRank.next2letterIndex = ProvinceExplorer::province[next2Pos.y][next2Pos.x] - 'a';
                    }
                }
                nextForRanked[wiggleRank] = nextRank;
            }
        }
    }
    // Successors of Uninitialiseds.
    {
        ProvinceExplorer uninitialised;
        uninitialised.phase = Uninitialised;
        for (int x = 0; x < N; x++)
        {
            for (int y = 0; y < 2; y++)
            {
                uninitialised.x = x;
                uninitialised.y = y;
                Next next = uninitialised.next();
                NextRanked nextRank;
                if (!next.isValid)
                {
                    nextRank = NextRanked::invalid();
                }
                else
                {
                    nextRank.twoNext = next.twoNext;
                    {
                        nextRank.next1rank = next.next1.rank();
                        const Pos next1Pos = next.next1.computePos();
                        nextRank.next1letterIndex = ProvinceExplorer::province[next1Pos.y][next1Pos.x] - 'a';
                    }
                    if (next.twoNext)
                    {
                        nextRank.next2rank = next.next2.rank();
                        const Pos next2Pos = next.next2.computePos();
                        nextRank.next2letterIndex = ProvinceExplorer::province[next2Pos.y][next2Pos.x] - 'a';
                    }
                }
                nextForRanked[uninitialised.rank()] = nextRank;
            }
        }
    }
}

void ProvinceExplorer::calculateProvinceExplorerRankingData()
{
    numUninitialisedRanks = 2 * N; // One for each location in the Province.
    numSegmentsAndProgress = N * (N + 1); // Length in segments (1 to N, inclusive - so N of these.);
    // progress through loop (up to 2 * num segments i.e. up to 2 * N).
    // [NB: 0 + 2 + ... + 2k === k * k + 1].
    numBeginLoopRanks = 2 * // Initial start direction (left/ right).
        2 * // Initial start row.
        numSegmentsAndProgress;
    numWiggleRanks = 2 * // Initial start direction (left/ right).
        2 * // Initial start row.
        2 * N; // Progress.
    numEndLoopRanks = numBeginLoopRanks;
    maxRank = numUninitialisedRanks + numBeginLoopRanks + numWiggleRanks + numEndLoopRanks;
}

void ProvinceExplorer::buildSimpleSubProblemLookup()
{
    // We encounter the following subproblem quite often, and it has a fairly simple solution: compute
    // a lookup table with the answers! 
    //
    // XXXXXXEFGH
    // XXXXXXABCD
    //
    // If the cities marked X are off-limits, leaving M columns remaining and the knight is at location A, 
    // how many distinct strings can we generate?  (Here, N = 10 and M = 4).  Any such string would
    // either begin with a wiggle AE or it would be the endloop ABCDHGFE.  Note that in the former
    // case, there is a one-to-one correspondence between such strings and those generated in the further
    // reduced grid (where M = 4 - 1):
    //
    // XXXXXXXFGH
    // XXXXXXXBCD
    //
    // where the Knight now starts at F i.e. a similar, smaller problem where the start row is opposite.
    // This hints at a recurrence relation and in fact we see that the number of possible strings in the
    // former case is equal to the number of strings in the latter case, plus one if and only if the 
    // endLoop ABCDHGFE does not generate the same string as any path beginning with wiggle AE (haveNewUnique).
    // Any such path would be of the form AE<some amount of wiggling><some endloop> and we can easily
    // detect if any of these gives the string ABCDHGFE by trying each such path.  The "some endloop" is 
    // referred to as checkEndLoop* in the implementation below. Note that for efficiency, we couch
    // the solution in terms of "wiggle progress" rather than the more intuitive "num segments (i.e. M)
    // remaining".
    vector<string> tempProvince(ProvinceExplorer::province);
    string wiggleLookups[2][2];
    for (int startDirection = 1; startDirection >= 0; startDirection--)
    {
        for (int startRow = 0; startRow < 2; startRow++)
        {
            numStringsAfterUniqueWiggle[startDirection][startRow].clear();
            numStringsAfterUniqueWiggle[startDirection][startRow].resize(2 * (N + 1));

            wiggleLookups[startDirection][startRow].resize(2 * N);
            int wiggleY = startRow;
            for (int i = 0; i < N; i++)
            {
                wiggleLookups[startDirection][startRow][i * 2] = tempProvince[wiggleY][i];
                wiggleLookups[startDirection][startRow][i * 2 + 1] = tempProvince[1 - wiggleY][i];
                wiggleY = 1 - wiggleY;
            }
        }
        reverse(tempProvince[0].begin(), tempProvince[0].end());
        reverse(tempProvince[1].begin(), tempProvince[1].end());
    }
    tempProvince[0] = ProvinceExplorer::province[0];
    tempProvince[1] = ProvinceExplorer::province[1];
    string endLoopLookups[2];
    endLoopLookups[0] = tempProvince[0] + string(tempProvince[1].rbegin(), tempProvince[1].rend());
    endLoopLookups[1] = tempProvince[1] + string(tempProvince[0].rbegin(), tempProvince[0].rend());

    for (int startDirection = 0; startDirection <= 1; startDirection++)
    {
        for (int startRow = 0; startRow < 2; startRow++)
        {
            numStringsAfterUniqueWiggle[startDirection][startRow].clear();
            numStringsAfterUniqueWiggle[startDirection][startRow].resize(2 * (N + 1));

            numStringsAfterUniqueWiggle[startDirection][startRow][2 * N - 2] = 1;
        }
    }
    assert(tempProvince == ProvinceExplorer::province);
    // Main bit :) Use the recurrence relation to fill in numStringsAfterUniqueWiggle.
    for (int startDirection = 1; startDirection >= 0; startDirection--)
    {
        for (int numSegmentsLeft = 2; numSegmentsLeft <= N; numSegmentsLeft++)
        {
            const int wiggleProgress = (N - numSegmentsLeft) * 2;
            for (int wiggleStartRow = 0; wiggleStartRow < 2; wiggleStartRow++)
            {
                int endLoopStartRow = (((wiggleProgress + 1) / 2) % 2 == 0) ? wiggleStartRow : 1 - wiggleStartRow;
                const string& endLoopLookup = endLoopLookups[endLoopStartRow];
                int endLoopLookupIndex = N - numSegmentsLeft;
                const string& wiggleLookup = wiggleLookups[startDirection][wiggleStartRow];
                int wiggleLookupIndex = wiggleProgress;

                // The main part of the recurrence relation, framed in terms of "wiggle progress" rather than
                // "segments left".
                numStringsAfterUniqueWiggle[startDirection][wiggleStartRow][wiggleProgress] = numStringsAfterUniqueWiggle[startDirection][wiggleStartRow][wiggleProgress + 2];
                // Second part of the recurrence relation: Can we generate a string matching endLoop by any other valid path?
                // If we can't, we may add "1" to our numStringsAfterUniqueWiggle.
                assert(endLoopLookup[endLoopLookupIndex] == wiggleLookup[wiggleLookupIndex]);
                endLoopLookupIndex++;
                wiggleLookupIndex++;
                bool haveNewUnique = true;
                if (endLoopLookup[endLoopLookupIndex] == wiggleLookup[wiggleLookupIndex])
                {
                    int checkEndLoopStartRow = 1 - endLoopStartRow;
                    int checkEndLoopIndex = N - (numSegmentsLeft - 1);
                    int checkEndLoopSize = 2 * (numSegmentsLeft - 1);
                    endLoopLookupIndex++;
                    wiggleLookupIndex++;
                    while (checkEndLoopSize > 0)
                    {
                        const string& checkEndLoopLookup = endLoopLookups[checkEndLoopStartRow];
                        const int endLoopPortionIndex = endLoopLookupIndex + 0; 
                        if (memcmp(&(endLoopLookup[endLoopPortionIndex]), &(checkEndLoopLookup[checkEndLoopIndex]), checkEndLoopSize) == 0)
                        {
                            // We generated an alternative Path with the same string as EndLoop.
                            haveNewUnique = false;
                            break;
                        }
                        // Not found a match yet: add an extra wiggle segment; remove a couple of chars from
                        // the beginning of endLoop, and prepare to try again with the remaining checkEndLoop
                        // and endLoop.
                        endLoopLookupIndex++;
                        wiggleLookupIndex++;
                        if (endLoopLookup[endLoopLookupIndex] != wiggleLookup[wiggleLookupIndex])
                        {
                            // Clashed with Wiggle: no alternative path.
                            break;
                        }
                        endLoopLookupIndex++;
                        wiggleLookupIndex++;
                        if (endLoopLookup[endLoopLookupIndex] != wiggleLookup[wiggleLookupIndex])
                        {
                            // Again, clashed with wiggle.
                            break;
                        }
                        checkEndLoopStartRow = 1 - checkEndLoopStartRow;
                        checkEndLoopIndex++;
                        checkEndLoopSize -= 2;
                    }
                }
                if (haveNewUnique)
                {
                    numStringsAfterUniqueWiggle[startDirection][wiggleStartRow][wiggleProgress]++;
                }
            }
        }
        // Flip string lookups horizontally.
        endLoopLookups[0] = string(tempProvince[0].rbegin(), tempProvince[0].rend()) + tempProvince[1];
        endLoopLookups[1] = string(tempProvince[1].rbegin(), tempProvince[1].rend()) + tempProvince[0];
        reverse(tempProvince[0].begin(), tempProvince[0].end());
        reverse(tempProvince[1].begin(), tempProvince[1].end());
    }
}

Next ProvinceExplorer::next() const
{
    switch(phase)
    {
        case Uninitialised:
        {
            if (x != 0 && x != N - 1)
            {
                // If not at the corners, create a BeginLoop to the left and the right.
                Next next;
                next.twoNext = true;
                next.next1.phase = BeginLoop;
                next.next1.numSegments = x + 1;
                next.next1.progress = 2;
                next.next1.startDirection = Left;
                next.next1.startRow = y;

                next.next2.phase = BeginLoop;
                next.next2.numSegments = N - x;
                next.next2.progress = 2;
                next.next2.startDirection = Right;
                next.next2.startRow = y;
                return next;
            }
            else
            {
                // If we are in a corner, create both a BeginLoop and a Wiggle.
                Next next;
                next.twoNext = true;
                next.next1.phase = BeginLoop;
                next.next1.numSegments = N;
                next.next1.progress = 2;
                next.next1.startDirection = (x == 0 ? Right : Left);
                next.next1.startRow = y;

                next.next2.phase = Wiggle;
                next.next2.numSegments = N;
                next.next2.progress = 2;
                next.next2.startDirection = (x == 0 ? Right : Left);
                next.next2.startRow = y;
                return next;
            }
        }
        case BeginLoop:
        {
            if (progress != numSegments * 2)
            {
                Next next;
                next.twoNext = false;
                next.next1 = *this;
                next.next1.progress++;
                return next;
            }
            else
            {
                if (numSegments == N)
                    return Next::invalid();
                Next next;
                next.twoNext = true;

                // Create a Wiggle ...
                {
                    ProvinceExplorer& wiggle = next.next1;
                    wiggle.phase = Wiggle;
                    wiggle.startDirection = (startDirection == Left ? Right : Left);
                    const int endY = 1 - startRow;
                    const int desiredWiggleStartY = endY;
                    wiggle.progress = 2 * numSegments + 1;
                    wiggle.numSegments = 2 * N;
                    const int wiggleYIfStartRowZero = (progress / 2) % 2;
                    wiggle.startRow = (wiggleYIfStartRowZero == desiredWiggleStartY ? 0 : 1);
                    assert(wiggle.progress >= 1 && wiggle.progress <= wiggle.numSegments * 2);
                }
                // ... and an end loop.
                {
                    ProvinceExplorer& endLoop = next.next2;
                    endLoop.phase = EndLoop;
                    endLoop.startDirection = (startDirection == Left ? Right : Left);
                    endLoop.startRow = 1 - startRow;
                    endLoop.progress = 1;
                    endLoop.numSegments = N - numSegments;
                }

                return next;
            }
        }
        case Wiggle:
        {
            if (progress == 2 * N)
                return Next::invalid();
            // Next Wiggle ...
            Next next;
            next.twoNext = false;
            next.next1 = *this;
            next.next1.progress++;
            if ((progress % 2) == 0)
            {
                // ... and if we've just moved horizontally, we should also 
                // create an EndLoop.
                const Pos pos = computePos();
                next.twoNext = true;
                next.next2.phase = EndLoop;
                next.next2.startRow = pos.y;
                next.next2.startDirection = startDirection;
                next.next2.progress = 1;
                next.next2.numSegments = N - (progress) / 2;
            }
            return next;
        }
        case EndLoop:
        {
            if (progress == 2 * numSegments)
                return Next::invalid();
            Next next;
            next.twoNext = false;
            next.next1 = *this;
            next.next1.progress++;
            return next;
        }
        default:
            assert(false);
    }
    return Next();
}

int main()
{
    // Holy wow - *boy* did I misjudge how hard this one would be!
    // OK, so - first and foremost - a gripe :) Having *finally*
    // gotten 100%, I've looked at the Editorial, and it seems - 
    // rather astonishingly - to assume that its imperfect hash is 
    // perfect i.e. that two strings hash to the same value if and only
    // if they are the same string.  This is, quite simply, wrong - 
    // it may work for these testcases - hell, it may even work for 
    // all N <= 600 - but given any imperfect hashing scheme, it should be possible
    // to come up with a testcase that breaks it i.e. where multiple
    // *distinct* strings all have the same hash.

    // A little bit of history - I've tried *many* approaches to this, and none
    // have succeeded until now - they've all been just that tiny bit too slow.  
    // It is - by a long way - the longest I've  spent on a hackerrank problem, 
    // so you can imagine how peeved I am that the Editorial "solution" is not 
    // really a solution ;) My first approach crucially miscalculated the number
    // of strings as being O(N^2) instead of O(N^3) - whoops :)

    // So - the solution.  Firstly, note that the slightly tricky language in the 
    // problem can be translated as "find all strings generated by visiting each
    // city in the province once and only once" i.e. treating the cities as nodes
    // in a graph connected to their neighbours, find the number of strings we
    // can generate by following a *path* (in the Graph Theory sense).

    // It's easy to see the general form of such a path: I'll assume that 
    // we start on the top row and head either left or down initially - the
    // other cases follow by symmetry.
    // We always begin with a "loop" (referred to as a BeginLoop), which can be
    // of zero size: it moves from the start point to the right of the province,
    // drops down to the bottom row, then heads left until we reach the column
    // we started in.  The number of squares covered by the BeginLoop is a multiple of 2.

    // XXXX>>V
    // XXXX<<<

    // We then have a "Wiggle", again possibly of zero size, where we "snake" along, generally
    // heading left and alternating the row:

    // XXv<XXX
    // XX<^XXX

    // Again, the wiggle covers a multiple of 2 squares.

    // Finally, we have a second loop, called the EndLoop, starting on the row the wiggle ended on,
    // heading all the way to the left, switching rows, and heading to the right until we reach
    // the column the EndLoop started on, at which point we've completed a Path:

    // >*XXXXX
    // ^<XXXXX

    // Easy peasy, and as you've probably guessed, it's the possibility of having multiple such paths
    // (remember, we need to consider the symmetrical case where we e.g. start on the bottom row and
    // head either left or up, etc) that causes the problems :) My approach is to use a set of "ProvinceExplorer"s -
    // little bots that are programmed to follow Paths of the kind described - which guide the creation of 
    // all possible strings.  Essentially, we try to create all possible strings recursively, pruning those
    // that cannot occur (because none of the current surviving ProvinceExplorers can generate the next letter 
    // by following their paths) and culling ProvinceExplorers that cannot generate the next letter in the string
    // by following their programmed paths: countUniquePathsAux(...) handles this.  Note that for Gridland Provinces the
    // asymptotic complexity is not really important, but the Constant Factor is crucially important: for this reason, ProvinceExplorers are
    // not manipulated directly by countUniquePathsAux(...) but it instead handles perfect *rankings* of them i.e. integers representing them
    // (it is easy to form a bijection between all possible ProvinceExplorers for a Grid and [0...maxRank] for 
    // some maxRank) plus a pre-computed lookup table that tells us where a ProvinceExplorer with a given rank will
    // end up next (note that a ProvinceExplorer may create further ProvinceExplorers where a "choice" occurs - 
    // for example, after moving horizontally in a Wiggle, it may decide not to Wiggle further but to start the 
    // EndLoop instead).  A large number of low-level optimisations are employed to keep this moving faster
    // (in particular, to avoid the use of std::set for the current set of active ProvinceExplorers after following
    // the string we've generated so far: std::set is far, far too slow) and to keep the stack footprint small-ish.
    // When we've generated a string of length 2 * N, we increment the answer: we don't need to worry about duplicates,
    // as this approach generates each valid string once and only once.

    // That's fundamentally it: there's tons of optimisations in there, one of the most effective being, if we've followed
    // a string and ended up with only one ProvinceExplorer remaining - i.e. there's only one path that generates the 
    // current string - to compute the number of possible continuations of this string without further calls to countUniquePathsAux():
    // The hardest and most effective of these computations is that which occurs when the one remaining ProvinceExplorer is a Wiggle: then 
    // we have a (simpler) subproblem: given a 2xM Grid for some M <= N, how many distinct strings can we generate *if we start in the first column* 
    // (or last, by symmetry)? This is contained in numStringsAfterUniqueWiggle, and may be computed by a reasonably simple
    // recurrence relation - see buildRankedProvinceExplorerSuccessorTable().

    int P;
    cin >> P;
    for (int p = 0; p < P; p++)
    {
        int N;
        cin >> N;
        string topRow;
        string bottomRow;

        cin >> topRow >> bottomRow;

        vector<string> province = { topRow, bottomRow };
        ProvinceExplorer::setProvince(province);
        cout << ProvinceExplorer::countUniquePaths() << endl;
    }
}