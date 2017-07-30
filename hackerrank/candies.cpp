// Simon St James (ssjgz) 2017-07-19
#define SUBMISSION
#ifndef SUBMISSION
#undef NDEBUG
#endif
#include <iostream>
#include <vector>
#include <limits>
#include <cassert>

using namespace std;

int main()
{
    // Ouch - quite an intricate one, and I misjudged it quite badly - probably part of the problem here is
    // that I tried to jump straight into an online, constant-time (per Student added), constant space solution XD
    //
    // So: as mentioned, there is a very efficient, online approach to this, but it is riddled with corner cases.
    // Imagine the first few ratings are all strictly increasing: the optimal approach to this is (and I hope
    // it's obvious XD) to give the first student 1 candy, and then each subsequent (higher rated than the previous)
    // student one more candy than the previous student.
    // Now imagine that the next student is rated lower than the previous one.  The optimal arrangement here is
    // to leave the previous students unchanged, then give this student 1 candy.  If the student after this is higher 
    // rated, then we give this student 1 more than the previous (i.e. give them 2); if the next studient is 
    // higher again, give them 3 etc.  So in this particular example, we have a series of increasing "ladders"
    // where the candies given increases as the student ratings (monotonically) increase, wrapping back
    // to 1 when a student has a lesser rank than the previous student.
    // Similarly, imagine we begin with a strictly increasing set of student ratings, and then the next rating
    // is equal to the previous rating: since there are no restrictions on adjacent identically-rated
    // students, the optimal thing to do is to again wrap back around to 1.  So far, so easy.
    // Here's where the problem comes in: imagine the first 4 students are rated as 1, 5, 7 and 10 and so
    // receive 1, 2, 3 and 4 candies respectively:
    //
    //  Student Rating
    //  1 5 7 10
    //  Student Candies
    //  1 2 3 4
    //
    // Now imagine that the next student has rating 6, which is less than the previous one; we wrap around and 
    // give him 1:
    //
    //  Student Rating
    //  1 5 7 10 6
    //  Student Candies
    //  1 2 3 4  1
    //
    // Now imagine the next student is even dumber: he gets a 5.  How many candies do we give him? "1" would
    // seem wise from the point of optimality, but see what happens:
    //
    //  Student Rating
    //  1 5 7 10 6 5
    //  Student Candies
    //  1 2 3 4  1 1
    //
    // We have adjacent students with ratings 6 & 5 respectively, but the same amount of candies! This is a
    // no-no, so we have to bump up the penultimate student's candies:
    //
    //  Student Rating
    //  1 5 7 10 6 5
    //  Student Candies
    //  1 2 3 4  2 1
    //
    // Now all is right with the world.  But what happens if the next student is dumber still and gets a 4?
    //
    //  Student Rating
    //  1 5 7 10 6 5 4
    //  Student Candies
    //  1 2 3 4  2 1 1
    //
    // Same problem, but now we have to bump up the last *two* student's candies:
    //
    //  Student Rating
    //  1 5 7 10 6 5 4
    //  Student Candies
    //  1 2 3 4  3 2 1
    //
    // So it seems that, when we start *descending* on a ladder, each subsequent lower rating means that
    // we have to bump up every student on the descending ladder.  Here, the "descending ladder" starts
    // at the student rated 6.  Note that we don't have to physically go back and add candies: since
    // we're only counting the totals, we keep a note of the current number of students on the descending
    // ladder (descendingLadderLength) and add that many to the total candies.  
    // All well and good, but let's add yet another dumber student, who gets a rating of 3:
    //
    //  Student Rating
    //  1 5 7 10 6 5 4 3
    //  Student Candies
    //  1 2 3 4  4 3 2 1
    //
    // Now things go wrong in a different way: we have adjacent students with ratings 10 & 6 respectively, but
    // the same number of candies (4).  Again, we must now bump this earlier student.  Note that he is the 
    // student immediately before the descending ladder began.
    //
    //  Student Rating
    //  1 5 7 10 6 5 4 3
    //  Student Candies
    //  1 2 3 5  4 3 2 1
    //
    // Let's add another dumb student, with rating 2.  We must now continue to bump up not only every student on the descending ladder,
    // but also the student immediately prior to the beginning of the descending ladder:
    //
    //  Student Rating
    //  1 5 7 10 6 5 4 3 2
    //  Student Candies
    //  1 2 3 6  5 4 3 2 1
    //
    // Thankfully, we don't have to go beyond this: it cannot be the case that the rating of the student prior to the student prior to the
    // beginning of the descending ladder is rated higher than the student prior to the beginning of the descending ladder, as then
    // the descending ladder would have begun one student earlier!
    // There's actually not much more to it than that; a rating equal to the previous one means that we are now off increasing or decreasing
    // ladder, and so acts as a nice "reset point": one we reach such a student, the students prior to him now have their candies fixed.
    // One more small corner case occurs when we begin a descending ladder and the previous student had one candy:
    //
    //  Student Rating
    //  10 10 10 6
    //  Student Candies
    //  1  1  1  1 
    //
    // We need to increase the previous student here, but do not need to go beyond him as, again, if the student prior to him had a higher
    // rating, the descending ladder would have started earlier!
    //
    //  Student Rating
    //  10 10 10 6
    //  Student Candies
    //  1  1  2  1 
    //

    int N;
    cin >> N;

    int64_t totalCandies = 0;
    int previousRating = std::numeric_limits<int>::max();

    bool isOnIncreasingLadder = false;
    int lastNumCandiesGivenOnIncreasingLadder = -1;

    bool isOnDescendingLadder = false;
    int descendingLadderLength = 1;
    int studentRatingAtDescendingLadderBegin = -1;
    int studentRatingBeforeDescendingLadderBegin = -1;
    int numCandiesForStudentBeforeDescendingLadderBegin = -1;
    bool haveIncorporatedStudentPriorToDescendingLadderBegin = false;
    int descendingLadderBeginIndex = -1;
    int lastTotalCandies = 0;
    int lastCandiesGiven = 0;

    for (int i = 0; i < N; i++)
    {
        int rating;
        cin >> rating;

        if (rating == previousRating)
        {
            totalCandies++;
            isOnIncreasingLadder = false;
            isOnDescendingLadder = false;
        }
        else if (rating < previousRating)
        {
            isOnIncreasingLadder = false;
            if (rating < previousRating)
            {
                if (isOnDescendingLadder)
                {
                    descendingLadderLength++;
                    if (!haveIncorporatedStudentPriorToDescendingLadderBegin)
                    {
                        const bool mayNeedToIncorporateStudentPriorToLadderBegin = (descendingLadderBeginIndex != 0) && (studentRatingBeforeDescendingLadderBegin > studentRatingAtDescendingLadderBegin);
                        if (mayNeedToIncorporateStudentPriorToLadderBegin)
                        {
                            const int currentCandiesForDescendingLadderBegin = descendingLadderLength;
                            if (currentCandiesForDescendingLadderBegin >= numCandiesForStudentBeforeDescendingLadderBegin)
                            {
                                haveIncorporatedStudentPriorToDescendingLadderBegin = true;
                            }
                        }
                    }
                }
                else
                {
                    isOnDescendingLadder = true;

                    descendingLadderLength = 1;
                    studentRatingAtDescendingLadderBegin = rating;
                    studentRatingBeforeDescendingLadderBegin = previousRating;
                    numCandiesForStudentBeforeDescendingLadderBegin = lastCandiesGiven;
                    haveIncorporatedStudentPriorToDescendingLadderBegin = false;
                    descendingLadderBeginIndex = i;

                    if (lastCandiesGiven == 1)
                        totalCandies++; // Need to bump the previous student's candies.
                }
                totalCandies += descendingLadderLength + (haveIncorporatedStudentPriorToDescendingLadderBegin ? 1 : 0);
            }
            
        }
        else if (rating > previousRating)
        {
            isOnDescendingLadder = false;
            if (!isOnIncreasingLadder)
            {
                lastNumCandiesGivenOnIncreasingLadder = (i == 0 ? 1 : 2);
                isOnIncreasingLadder = true;
            }
            else
            {
                lastNumCandiesGivenOnIncreasingLadder++;
            }
            totalCandies += lastNumCandiesGivenOnIncreasingLadder;
        }
        lastCandiesGiven = (totalCandies - lastTotalCandies);
        previousRating = rating;
        lastTotalCandies = totalCandies;

    }
    assert(cin);

    cout << totalCandies << endl;
}

