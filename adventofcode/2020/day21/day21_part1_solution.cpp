#include <iostream>
#include <sstream>
#include <vector>
#include <regex>
#include <set>
#include <cassert>

using namespace std;

int main()
{
    struct FoodLabel
    {
        vector<string> ingredients;
        vector<string> allergens;
    };
    string foodLine;
    set<string> allIngredients;
    set<string> allAllergens;
    vector<FoodLabel> foodLabels;
    while (getline(cin, foodLine))
    {
#if 0
        istringstream foodStream(foodLine);
        cout << "food line:" << foodLine << endl;

        // List of ingredients.
        cout << "Ingredients: " << endl;
        while (true)
        {
            char peekChar = '\0';
            foodStream >> peekChar;
            if (!foodStream)
                break;
            if (peekChar != '(')
            {
                foodStream.putback(peekChar);
                string ingredient;
                foodStream >> ingredient;
                cout << " ingredient: " << ingredient << endl;
            }
            else
            {
                break;
            }
        }

        cout << "Allergens": << endl;
        char peekChar = '\0';
        foodStream >> peekChar;
        if (peekChar == '(')
        {
            while (true)
            {
                string dummyString;
                foodStream >> dummyString;
                assert(dummyString == "contains");
                string allergen;
            }
        }

#endif
        auto parseWords = [](const string& wordsString)
        {
            vector<string> words;
            istringstream wordsStream(wordsString);
            string word;
            while (wordsStream >> word)
            {
                words.push_back(word);
            }
            return words;
        };
        replace(foodLine.begin(), foodLine.end(), ',', ' ');
        static regex foodLineRegex(R"((^.*)\(\s*contains\s*(.*)\)\s*$)");
        std::smatch foodLineMatch;
        if (regex_match(foodLine, foodLineMatch, foodLineRegex))
        {
            cout << "Ingredient list: " << endl;
            FoodLabel foodLabel;
            const auto ingredients = parseWords(foodLineMatch[1]);
            for (const auto& ingredient : ingredients)
            {
                cout << " " << ingredient <<  endl;
                allIngredients.insert(ingredient);
                foodLabel.ingredients.push_back(ingredient);
            }
            cout << "Allergen list: " << endl;
            const auto allergens = parseWords(foodLineMatch[2]);
            for (const auto& allergen : allergens)
            {
                cout << " " << allergen <<  endl;
                allAllergens.insert(allergen);
                foodLabel.allergens.push_back(allergen);
            }

            foodLabels.push_back(foodLabel);
        }
        else
        {
            assert(false);
        }
    }

    int numOccurrencesOfNonAllergenIngredient = 0;
    for (const auto& ingredient : allIngredients)
    {
        bool mayContainAnyAllergen = false;
        for (const auto& candidateAllergen : allAllergens)
        {
            bool contradictionFound = false;
            for (const auto& foodLabel : foodLabels)
            {
                if (find(foodLabel.allergens.begin(), foodLabel.allergens.end(), candidateAllergen) != foodLabel.allergens.end() &&
                    find(foodLabel.ingredients.begin(), foodLabel.ingredients.end(), ingredient)  == foodLabel.ingredients.end())
                {
                    contradictionFound = true;
                    break;
                }
            }
            if (!contradictionFound)
            {
                mayContainAnyAllergen = true;
                cout << "ingredient " << ingredient << " possibly contains allergen " << candidateAllergen << endl;
            }
        }

        if (!mayContainAnyAllergen)
        {
            cout << "ingredient " << ingredient << " cannot contain any allergens" << endl;
            for (const auto& foodLabel : foodLabels)
            {
                numOccurrencesOfNonAllergenIngredient += count(foodLabel.ingredients.begin(), foodLabel.ingredients.end(), ingredient);
            }
        }

    }
    cout << numOccurrencesOfNonAllergenIngredient << endl;

}

