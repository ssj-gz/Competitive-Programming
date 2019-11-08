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
    int maxPrefixVMC = 0;
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
                && currentPrefixVMC - maxPrefixVMC < 0)
        {
            // Chopping off the prefix that gave rise to maxPrefixVMC from
            // the current prefix yields a string, of length >= 2, with
            // fewer vowels than consonants; this recipe is Bob's.
            return false;
        }

        previousPreviousVMC = previousVMC;
        previousVMC = currentPrefixVMC;
        maxPrefixVMC = max(maxPrefixVMC, previousPreviousVMC); // i.e. ensure that the maxPrefixVMC is for a prefix at least 2 less than
                                                               // the current prefix, as we only consider substrings of length >= 2.
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
