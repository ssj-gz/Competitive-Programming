#include <iostream>
#include <vector>

#include <cassert>

using namespace std;

int main()
{
    int64_t numRecipesToMake = -1;
    cin >> numRecipesToMake;
    std::cout << "numRecipesToMake: " << numRecipesToMake << std::endl;
    assert(numRecipesToMake != -1);
    vector<int> recipes;
    recipes.push_back(3);
    recipes.push_back(7);
    int elf1CurrentIndex = 0;
    int elf2CurrentIndex = 1;

    int round = 0;
    while (recipes.size() < numRecipesToMake + 10)
    {
        if (recipes.size() % 1000 == 0)
            std::cout << "# recipes: " << recipes.size() << std::endl;
#if 0
        std::cout << "Round: " << (round + 1) << " recipes: " << std::endl;
        for (int j = 0; j < recipes.size(); j++)
        {
            if (j == elf1CurrentIndex)
                std::cout << "(" << recipes[j] << ")";
            else if (j == elf2CurrentIndex)
                std::cout << "[" << recipes[j] << "]";
            else
                std::cout << recipes[j];
            std::cout << " ";
        }
        std::cout << std::endl;
#endif

        string digitsOfSum = std::to_string(recipes[elf1CurrentIndex] + recipes[elf2CurrentIndex]);
        for (const auto digit : digitsOfSum)
            recipes.push_back(digit - '0');
        elf1CurrentIndex = (elf1CurrentIndex + 1 + recipes[elf1CurrentIndex]) % recipes.size();
        elf2CurrentIndex = (elf2CurrentIndex + 1 + recipes[elf2CurrentIndex]) % recipes.size();
        //std::cout << " elf1CurrentIndex now: " << elf1CurrentIndex << std::endl;
        //std::cout << " elf2CurrentIndex now: " << elf2CurrentIndex << std::endl;

        round++;
    }
    for (int i = numRecipesToMake; i < numRecipesToMake + 10; i++)
    {
        std::cout << recipes[i];
    }
    std::cout << std::endl;
            

    return 0;
}
