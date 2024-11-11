#include <iostream>
#include <vector>
#include <regex>
#include <limits>

#include <cassert>

using namespace std;

struct Item
{
    enum Type { Weapon = 0, Armor = 1, Ring = 2 };
    Type type;
    string name;
    int cost = -1;
    int damage = -1;
    int armor = -1;
};

constexpr auto playerInitialHitPoints = 100;

void chooseItemsAndFight(vector<Item>::iterator nextItemIter, const vector<Item>& allItems, vector<Item>& chosenItems, const int initialBossHitPoints, const int bossDamage, const int bossArmor, int& maxCostThatLoses)
{
    // Valid?
    int numWeapons = 0;
    int numArmor = 0;
    int numRings = 0;
    int cost = 0;
    int damage = 0;
    int armor = 0;
    for (const auto& item : chosenItems)
    {
        switch (item.type)
        {
            case Item::Weapon:
                numWeapons++;
                break;
            case Item::Armor:
                numArmor++;
                break;
            case Item::Ring:
                numRings++;
                break;
        }
        cost += item.cost;
        damage += item.damage;
        armor += item.armor;
    }
    if (numWeapons == 1 && numArmor <= 1 && numRings <= 2)
    {
        // Valid. Win?
        std::cout << "Trying with cost: " << cost << " damage: " << damage << " armor: " << armor << std::endl;
        bool isPlayerTurn = true;
        int playerHitPoints = playerInitialHitPoints;
        int bossHitPoints = initialBossHitPoints;
        while (playerHitPoints > 0 && bossHitPoints > 0)
        {
            if (isPlayerTurn)
            {
                const int damageDealt = std::max(1, damage - bossArmor);
                bossHitPoints -= damageDealt;
                std::cout << " player dealt " << damageDealt << " damage to boss; bossHitPoints now: " << bossHitPoints << std::endl;
            }
            else
            {
                const int damageDealt = std::max(1, bossDamage - armor);
                playerHitPoints -= damageDealt;
                std::cout << " boss dealt " << damageDealt << " damage to player; playerHitPoints now: " << playerHitPoints << std::endl;
            }

            isPlayerTurn = !isPlayerTurn;
        }
        if (playerHitPoints <= 0)
        {
            std::cout << " can lost with cost: " << cost << std::endl;
            if (cost > maxCostThatLoses)
            {
                maxCostThatLoses = cost;
                std::cout << "  new maxCostThatLoses: " << maxCostThatLoses << std::endl;
            }
        }
        else
        {
            std::cout << " Won!" << std::endl;
        }
    

    }
    if (nextItemIter == allItems.end())
        return;
    // Pick this Item.
    chosenItems.push_back(*nextItemIter);
    chooseItemsAndFight(std::next(nextItemIter), allItems, chosenItems, initialBossHitPoints, bossDamage, bossArmor, maxCostThatLoses);
    chosenItems.pop_back();
    // Don't pick this Item.
    chooseItemsAndFight(std::next(nextItemIter), allItems, chosenItems, initialBossHitPoints, bossDamage, bossArmor, maxCostThatLoses);

}

int main()
{
    vector<Item> items = 
    {
        {Item::Weapon, "Dagger", 8, 4, 0},
        {Item::Weapon, "Shortsword", 10, 5, 0},
        {Item::Weapon, "Warhammer", 25, 6, 0},
        {Item::Weapon, "Longsword", 40, 7, 0},
        {Item::Weapon, "Greataxe", 74, 8, 0},
        {Item::Armor, "Leather", 13, 0, 1},
        {Item::Armor, "Chainmail", 31, 0, 2},
        {Item::Armor, "Splintmail", 53, 0, 3},
        {Item::Armor, "Bandedmail", 75, 0, 4},
        {Item::Armor, "Platemail", 102, 0, 5},
        {Item::Ring, "Damage+1", 25, 1, 0},
        {Item::Ring, "Damage+2", 50, 2, 0},
        {Item::Ring, "Damage+3", 100, 3, 0},
        {Item::Ring, "Defense+1", 20, 0, 1},
        {Item::Ring, "Defense+2", 40, 0, 2},
        {Item::Ring, "Defense+3", 80, 0, 3}
    };

    std::regex bossStatRegex(R"(^(.*):\s*(\d+)$)");

    auto readStat = [&bossStatRegex]()
    {
        std::smatch bossStatMatch;
        string bossStatLine;
        getline(cin, bossStatLine);
        const bool matchSuccessful = std::regex_match(bossStatLine, bossStatMatch, bossStatRegex);
        assert(matchSuccessful);
        std::cout << "Read: >" << bossStatMatch.str(1) << "< : " << std::stoi(bossStatMatch.str(2)) << std::endl;
        return std::pair<string, int>(bossStatMatch.str(1), std::stoi(bossStatMatch.str(2)));
    };
    const auto [bossHitPointStr, initialBossHitPoints] = readStat();
    assert(bossHitPointStr == "Hit Points");
    const auto [bossDamageStr, bossDamage] = readStat();
    assert(bossDamageStr == "Damage");
    const auto [bossArmorStr, bossArmor] = readStat();
    assert(bossArmorStr == "Armor");

    int maxCostThatLoses = std::numeric_limits<int>::min();
    vector<Item> chosenItems;
    chooseItemsAndFight(items.begin(), items, chosenItems, initialBossHitPoints, bossDamage, bossArmor, maxCostThatLoses);
    std::cout << "maxCostThatLoses: " << maxCostThatLoses << std::endl;



    return 0;
}
