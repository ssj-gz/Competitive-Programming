// Simon St James (ssjgz) - 2019-11-04
// 
// Solution to: https://www.codechef.com/NOV19A/problems/WEIRDO
//
//#define SUBMISSION
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

#if 0
SolutionType solveOptimised()
{
    SolutionType result;
    
    return result;
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
#if 0
        const auto solutionOptimised = solveOptimised();
        cout << "solutionOptimised:  " << solutionOptimised << endl;

        assert(solutionOptimised == solutionBruteForce);
#endif
#else
        const auto solutionOptimised = solveOptimised();
        cout << solutionOptimised << endl;
#endif
    }

    assert(cin);
}
