#include <iostream>
#include <regex>
#include <vector>

#include <cassert>

using namespace std;

int main()
{
    std::regex ingredientRegex(R"(^(\w+): capacity ([-0-9]+), durability ([-0-9]+), flavor ([-0-9]+), texture ([-0-9]+), calories ([-0-9]+)$)");
    string ingredientLine;
    struct Ingredient
    {
        string name;
        int capacity = -1;
        int durability = -1;
        int flavor = -1;
        int texture = -1;
        int calories = -1;
    };
    vector<Ingredient> ingredients;
    while (getline(cin, ingredientLine))
    {
        std::smatch ingredientMatch;
        const bool matchSuccessful = std::regex_match(ingredientLine, ingredientMatch, ingredientRegex);
        assert(matchSuccessful);
        Ingredient ingredient;
        ingredient.capacity = std::stoi(ingredientMatch.str(2));
        ingredient.durability = std::stoi(ingredientMatch.str(3));
        ingredient.flavor = std::stoi(ingredientMatch.str(4));
        ingredient.texture = std::stoi(ingredientMatch.str(5));
        ingredient.calories = std::stoi(ingredientMatch.str(6));

        ingredients.push_back(ingredient);
    }

    const auto numIngredients = static_cast<int>(ingredients.size());
    // Use "stars and bars" to find all amounts of each ingredient such
    // that the total amount of ingredient is 100;
    const auto numBars = 100;
    const auto numStars = numIngredients - 1;
    const auto numStarsAndBars = numStars + numBars;
    vector<int> starPositions;
    for (auto i = 0; i < numStars; i++)
    {
        starPositions.push_back(i);
    }
    int64_t bestCookieScore = 0;
    while (true)
    {
        std::cout << "Trying: " << std::endl;
        // Extract the number of ingredients from the stars and bars combination.
        int ingredientSum = 0;
        int64_t capacityTotal = 0;
        int64_t durabilityTotal = 0;
        int64_t flavorTotal = 0;
        int64_t textureTotal = 0;
        int64_t calorieTotal = 0;
        for (int ingredientIndex = 0; ingredientIndex < numIngredients; ingredientIndex++)
        {
            int amountOfIngredient = -1;
            if (ingredientIndex == 0)
                amountOfIngredient = starPositions[0];
            else if (ingredientIndex == numIngredients - 1)
                amountOfIngredient = numBars - ingredientSum;
            else 
                amountOfIngredient = starPositions[ingredientIndex] - starPositions[ingredientIndex - 1] - 1;
            assert(amountOfIngredient >= 0);
            std::cout << "amountOfIngredient " << ingredientIndex << ": " << amountOfIngredient << std::endl;
            ingredientSum += amountOfIngredient;

            capacityTotal += ingredients[ingredientIndex].capacity * amountOfIngredient;
            durabilityTotal += ingredients[ingredientIndex].durability * amountOfIngredient;
            flavorTotal += ingredients[ingredientIndex].flavor * amountOfIngredient;
            textureTotal += ingredients[ingredientIndex].texture * amountOfIngredient;
            calorieTotal += ingredients[ingredientIndex].calories * amountOfIngredient;
        }
        assert(ingredientSum == numBars);
        if (calorieTotal == 500)
        {
            std::cout << "capacityTotal: " << capacityTotal << std::endl;
            std::cout << "durabilityTotal: " << durabilityTotal << std::endl;
            std::cout << "flavorTotal: " << flavorTotal << std::endl;
            std::cout << "textureTotal: " << textureTotal << std::endl;
            std::cout << "calorieTotal: " << calorieTotal << std::endl;
            const int64_t cookieScore = std::max<int64_t>(capacityTotal, 0) * 
                std::max<int64_t>(durabilityTotal, 0) *
                std::max<int64_t>(flavorTotal, 0) *
                std::max<int64_t>(textureTotal, 0);
            std::cout << "cookieScore: " << cookieScore << std::endl;
            bestCookieScore = std::max(bestCookieScore, cookieScore);
        }


        // Move to combination of stars and bars.
        int starIndex = numStars - 1;
        int numStarsReachedLimit = 0;
        while (starIndex >= 0 && starPositions[starIndex] == numStarsAndBars - (numStars - starIndex))
        {
            starIndex--;
            numStarsReachedLimit++;
        }
        if (numStarsReachedLimit == numStars)
            break;
        int latestStarThatCanAdvance = numStars - 1 - numStarsReachedLimit;
        assert(latestStarThatCanAdvance >=0 && latestStarThatCanAdvance < numStars);
        starPositions[latestStarThatCanAdvance]++;
        for (int starToRight = latestStarThatCanAdvance + 1; starToRight < numStars; starToRight++)
        {
            starPositions[starToRight] = starPositions[starToRight - 1] + 1;
        }

    }
    std::cout << "bestCookieScore: " << bestCookieScore << std::endl;

    
    return 0;
}
