// Simon St James (ssjgz) - 2019-11-04
// 
// Solution to: https://www.codechef.com/NOV19A/problems/WEIRDO
//
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <limits>
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

bool isVowel(char letter)
{
    switch (letter)
    {
    case 'a':
    case 'e':
    case 'i':
    case 'o':
    case 'u':
        return true;
    }
    return false;
}

bool isAlicesRecipe(const string& recipe)
{
    // We need to check whether, for every substring (of length >= 2) of the string
    // "recipe", that substring contains at least as many vowels as consonants.
    // Let VMC(s) be the number of Vowels Minus the number of Consonants in a string s.
    //
    // Note that every substring of length >= 2 of recipe can be expressed as a prefix P of recipe with
    // a prefix of P, P' (itself a prefix of recipe) subject to |P| - |P'| >= 2 removed from the beginning of P.
    // Let P - P' be the string obtained by removing the prefix P' from the front of P.
    // Thus, it suffices to compute whether, for each prefix P of recipe, there is no prefix P'
    // of recipe with |P| - |P'| >= 2 and VMC(P - P') < 0.  Note that it is very easy to compute VMC(P) 
    // for each successive prefix P of recipe.
    //
    // Let P be a prefix of recipe.  How can we decide whether, for all prefixes P' with |P| - |P'| >= 2, VMC(P - P') < 0?
    // Easy - note that VMC(P - P') == VMC(P) - VMC(P').  This has its lowest value - and so is most likely to
    // be < 0 - when we pick such a P' with the *highest* value of VMC(P').
    //
    // Hopefully the code is now self-explanatory :)
    int highestPrefixVMC = 0;
    int currentPrefixVMC = 0;
    int previousVMC = -1;
    int previousPreviousVMC = -1;

    for (int i = 0; i < recipe.size(); i++)
    {
        const auto letter = recipe[i];
        if (isVowel(letter))
            currentPrefixVMC++;
        else
            currentPrefixVMC--;

        if (i >= 1 // We only consider substrings of length >= 2.
                && currentPrefixVMC - highestPrefixVMC < 0)
        {
            // Chopping off the prefix that gave rise to highestPrefixVMC from
            // the current prefix yields a string, of length >= 2, with
            // fewer vowels than consonants; this recipe is Bob's.
            return false;
        }

        previousPreviousVMC = previousVMC;
        previousVMC = currentPrefixVMC;
        highestPrefixVMC = max(highestPrefixVMC, previousPreviousVMC); // i.e. ensure that the highestPrefixVMC is the highest VMC for a prefix with *length at least 2 less than
                                                                       // the current prefix*, as we only want to consider substrings of length >= 2.
    }

    return true;
}

long double calcLogScore(const vector<string>& recipes)
{
    long double logScore = 0;
    for (char letter = 'a'; letter <= 'z'; letter++)
    {
        int numRecipesWithLetter = 0;
        int numOccurencesOfLetter = 0;
        for (const auto& recipe : recipes)
        {
            const int numOfLetterInRecipe = std::count(recipe.begin(), recipe.end(), letter);
            if (numOfLetterInRecipe > 0)
                numRecipesWithLetter++;

            numOccurencesOfLetter += numOfLetterInRecipe;
        }

        if (numOccurencesOfLetter > 0)
        {
            // Use the given formula for scores, but use addition of logarithms
            // in place of multiplications, and substraction of logarithms in
            // place of divisions, to give the log of the score.
            assert(numRecipesWithLetter > 0);
            logScore += logl(static_cast<long double>(numRecipesWithLetter));
            logScore -= recipes.size() * logl(static_cast<long double>(numOccurencesOfLetter));
        }
    }

    return logScore;
}

long double calcScoreRatio(const int numRecipes, const vector<string>& recipes)
{
    // Sort all recipes into Alice's recipes and Bob's recipes.
    vector<string> alicesRecipes;
    vector<string> bobsRecipes;
    for (const auto& recipe : recipes)
    {
        if (isAlicesRecipe(recipe))
            alicesRecipes.push_back(recipe);
        else
            bobsRecipes.push_back(recipe);
    }

    const auto aliceScoreLog = calcLogScore(alicesRecipes);
    const auto bobScoreLog = calcLogScore(bobsRecipes);

    const static auto logMaxRatio = logl(static_cast<long double>(10'000'000));

    // Use subtraction of logarithms in place of division to get the log of
    // the score ratio.
    const auto logScoreRatio = aliceScoreLog - bobScoreLog;

    if (logScoreRatio >= logMaxRatio)
        return -1; // "Infinity".
    
    // Anti-log the logScoreRatio to get the score ratio :)
    return expl(logScoreRatio);
}

int main(int argc, char* argv[])
{
    // Wasn't sure if logl and expl would be accurate enough for this, but thankfully
    // they were and it worked first time :)
    //
    // QUICK EXPLANATION
    //
    // Deciding which of Alice/ Bob a given recipe R belongs to can be easily decided
    // in O(R) by considering the "vowels minus consonants" (VMC) value of each prefix of
    // R, but that's not the main point of this Problem: the main point is to be able to
    // compute the ratio of two very large integers - each the product of several potentially
    // large integers raised to the power of another potentially large integer - to within
    // a given degree of precision.  This calculation can be easily performed using some
    // basic properties of logarithms.
    //
    // LONGER EXPLANATION
    //
    // The problem of deciding whether the Recipe is Alice/ Bob's is explained in isAlicesRecipe.
    // For the remainder of the problem - computing the ratio of two huge integers - 
    // we use the basic properties of logarithms (see any introductory maths text dealing with
    // logs - or the Wikipedia article: https://en.wikipedia.org/wiki/Logarithm - for more details):
    //
    //    log(ab) = log(a) + log(b) (i)
    //    log(a/b) = log(a) - lob(b)
    //
    // From (i) it follows, by induction, that:
    //
    //    log(a_1a_2 ... a_m) = log(a_1) + log(a_2) + ... + log(a_m)
    //
    // and it further follows, again by induction, that for integer k:
    //   
    //    log(a^k) = k log(a).
    //
    // With this in mind, the code is hopefully self-explanatory :)

    ios::sync_with_stdio(false);

    const auto T = read<int>();

    // High-precision output, please :)
    cout << std::setprecision (std::numeric_limits<long double>::digits10 + 1);

    for (int t = 0; t < T; t++)
    {
        const int numRecipes = read<int>();

        vector<string> recipes(numRecipes);
        for (auto& recipe : recipes)
            recipe = read<string>();

        const auto scoreRatio = calcScoreRatio(numRecipes, recipes);
        if (scoreRatio == -1)
            cout << "Infinity";
        else
            cout << scoreRatio;
        cout << endl;
    }

    assert(cin);
}
