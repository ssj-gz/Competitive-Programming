// Simon St James (ssjgz) - 2019-11-04
// 
// Solution to: https://www.codechef.com/NOV19A/problems/WEIRDO
//
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <limits>
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

bool isAlicesRecipeBruteForce(const string& recipe)
{
    for (int startPos = 0; startPos < recipe.size(); startPos++)
    {
        for (int length = 2; startPos + length <= recipe.size(); length++)
        {
            const string substring = recipe.substr(startPos, length);
            int numVowels = 0;
            int numConsonants = 0;
            for (const auto letter : substring)
            {
                if (isVowel(letter))
                    numVowels++;
                else 
                    numConsonants++;
            }
            if (numVowels < numConsonants)
                return false;
        }
    }

    return true;
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

        if (i >= 1 && currentPrefixVMC - maxPrefixVMC < 0)
            return false;

        previousPreviousVMC = previousVMC;
        previousVMC = currentPrefixVMC;
        maxPrefixVMC = max(maxPrefixVMC, previousPreviousVMC);
    }

    return true;
}

long double calcScoreBruteForce(const vector<string>& recipes)
{
    cout << "calcScoreBruteForce: " << endl;
    long double score = 1;
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

        //cout << "Letter: " << letter << " numOccurencesOfLetter: " << numOccurencesOfLetter << " numRecipesWithLetter: " << numRecipesWithLetter << endl;


        if (numOccurencesOfLetter > 0)
        {
            assert(numRecipesWithLetter > 0);
            score *= numRecipesWithLetter;
            for (int i = 0; i < recipes.size(); i++)
            {
                score /= numOccurencesOfLetter;
            }
        }
    }

    cout << "score: " << score << endl;

    return score;
}

#if 1
long double solveBruteForce(const int numRecipes, const vector<string>& recipes)
{
    long double result = 0;

    vector<string> alicesRecipes;
    vector<string> bobsRecipes;

    for (const auto& recipe : recipes)
    {
        if (isAlicesRecipeBruteForce(recipe))
            alicesRecipes.push_back(recipe);
        else
            bobsRecipes.push_back(recipe);
    }

    cout << "Alice's recipes: " << endl;
    for (const auto recipe : alicesRecipes)
    {
        cout << " " << recipe << endl;
    }
    cout << endl;
    cout << "Bob's recipes: " << endl;
    for (const auto recipe : bobsRecipes)
    {
        cout << " " << recipe << endl;
    }
    cout << endl;

    const auto aliceScore = calcScoreBruteForce(alicesRecipes);
    const auto bobScore = calcScoreBruteForce(bobsRecipes);

    
    return aliceScore/ bobScore;
}
#endif

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
            assert(numRecipesWithLetter > 0);
            logScore += logl(static_cast<long double>(numRecipesWithLetter));
            logScore -= recipes.size() * logl(static_cast<long double>(numOccurencesOfLetter));
        }
    }

    //cout << "logScore: " << logScore << endl;

    return logScore;
}

#if 1
long double solveOptimised(const int numRecipes, const vector<string>& recipes)
{
    long double result = 0;

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

    const auto logMaxRatio = logl(static_cast<long double>(10'000'000));

    if (aliceScoreLog - bobScoreLog >= logMaxRatio)
        return -1;
    
    return expl(aliceScoreLog - bobScoreLog);
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
        }

        return 0;
    }

    auto intpower = [](int64_t base, int64_t exponent)
    {
        int64_t result = 1;
        for (int i = 0; i < exponent; i++)
        {
            result *= base;
        }
        return result;
    };

#if 0
    {
        cout <<  std::setprecision (std::numeric_limits<long double>::digits10 + 1);
        cout << "log 5: " << logl(5) << endl;
        cout << "Blee: " << static_cast<long double>(intpower(2, 12) * intpower(5, 8) * intpower(3, 20)) / (intpower(7, 4) * intpower(5, 7) * intpower(6, 5)) << endl;

        cout << "Bloo: " << expl(12 * logl(2) + 8 * logl(5) + 20 * logl(3) - (4 * logl(7) + 7 * logl(5) + 5 * logl(6))) << endl;

        return 0;
    }
#endif
#if 0
    {
        while (true)
        {
            const int stringLen = 1 + rand() % 100;
            string s;
            for (int i = 0; i < stringLen; i++)
            {
                s += 'a' + rand() % 2;
            }
            cout << "s: " << s << endl;
            const bool bruteForce = isAlicesRecipeBruteForce(s);
            const bool optimised = isAlicesRecipe(s);

            cout << "bruteForce: " << bruteForce << " optimised: " << optimised << endl;
            assert(bruteForce == optimised);
        }
    }
#endif

    
    const auto T = read<int>();

    cout <<  std::setprecision (std::numeric_limits<long double>::digits10 + 1);

    for (int t = 0; t < T; t++)
    {
        const int numRecipes = read<int>();
        vector<string> recipes(numRecipes);

        for (auto& recipe : recipes)
            recipe = read<string>();



#ifdef BRUTE_FORCE
#if 1
        const auto solutionBruteForce = solveBruteForce(numRecipes, recipes);
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
#endif
#if 1
        const auto solutionOptimised = solveOptimised(numRecipes, recipes);
        cout << "solutionOptimised:  ";
        if (solutionOptimised == -1)
            cout << "Infinity";
        else
            cout << solutionOptimised;
        cout << endl;

        //assert(solutionOptimised == solutionBruteForce);
#endif
#else
        const auto solutionOptimised = solveOptimised(numRecipes, recipes);
        if (solutionOptimised == -1)
            cout << "Infinity";
        else
            cout << solutionOptimised;
        cout << endl;
#endif
    }

    assert(cin);
}
