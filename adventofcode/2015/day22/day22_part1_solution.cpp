#include <iostream>
#include <regex>
#include <limits>

#include <cassert>

using namespace std;

struct Stats
{
    int hitpoints = -1;
    int damage = -1;
    int mana = -1;
    auto operator<=>(const Stats& other) const = default;
};

struct Effects
{
    int shieldCountdown = 0;
    int poisonCountdown = 0;
    int rechargeCountdown = 0;
    auto operator<=>(const Effects& other) const = default;
};

using State = std::tuple<bool, Stats, Stats, Effects>;
map<State, int> minManaToWinLookup;
constexpr auto NoWin = std::numeric_limits<int>::max();

int calcMinManaToWin(bool isPlayerTurn, Stats playerStats, Stats bossStats, Effects effects)
{
    const State state = {isPlayerTurn, playerStats, bossStats, effects};
    if (minManaToWinLookup.size() % 100'000 == 0)
    {
        std::cout << "minManaToWinLookup.size(): " << minManaToWinLookup.size() << std::endl;
    }
    if (minManaToWinLookup.contains(state))
    {
        return minManaToWinLookup[state];
    }

    int playerArmor = 0;
    if (effects.shieldCountdown > 0)
    {
        playerArmor = 7;
        effects.shieldCountdown--;
    }

    if (effects.poisonCountdown > 0)
    {
        bossStats.hitpoints -= 3;
        effects.poisonCountdown--;
    }

    if (effects.rechargeCountdown > 0)
    {
        playerStats.mana += 101;
        effects.rechargeCountdown--;
    }

    if (bossStats.hitpoints <= 0)
    {
        // This is a win!
        //std::cout << "got a win" << std::endl;
        return 0;
    }

    if (isPlayerTurn)
    {
        constexpr auto magicMissileManaCost = 53;
        constexpr auto drainManaCost = 73;
        constexpr auto shieldManaCost = 113;
        constexpr auto poisonManaCost = 173;
        constexpr auto rechargeManaCost = 229;

        int result = NoWin;
        bool hasCastASpell = false;

        auto tryWithSpell = [&playerStats, &bossStats, &effects, &result, &hasCastASpell](const auto manaCost, auto&& applySpellEffects)
        {
            if (playerStats.mana >= manaCost)
            {
                Stats playerStatsNext = playerStats;
                playerStatsNext.mana -= manaCost;

                Stats bossStatsNext = bossStats;
                Effects effectNext = effects;

                applySpellEffects(playerStatsNext, bossStatsNext, effectNext);
                hasCastASpell = true;

                const auto minWithThisChoice = calcMinManaToWin(false, playerStatsNext, bossStatsNext, effectNext);
                if (minWithThisChoice != NoWin)
                {
                    result = std::min(result, minWithThisChoice + manaCost);
                }
            }
        };

        // Try with casting Magic Missile.
        tryWithSpell(magicMissileManaCost, [](auto& playerStats [[maybe_unused]], auto& bossStats, auto& effects [[maybe_unused]])
                {
                    bossStats.hitpoints -= 4;
                });

        // Try with casting Drain.
        tryWithSpell(drainManaCost, [](auto& playerStats, auto& bossStats, auto& effects [[maybe_unused]])
                {
                    playerStats.hitpoints += 2;
                    bossStats.hitpoints -= 2;
                } );
        if (effects.shieldCountdown == 0)
        {
            // Try with casting Shield.
            tryWithSpell(shieldManaCost, [](auto& playerStats [[maybe_unused]], auto& bossStats [[maybe_unused]], auto& effects)
                    {
                        effects.shieldCountdown = 6;
                    } );
        }
        if (effects.poisonCountdown == 0)
        {
            // Try with casting Poison.
            tryWithSpell(poisonManaCost, [](auto& playerStats [[maybe_unused]], auto& bossStats [[maybe_unused]], auto& effects)
                    {
                        effects.poisonCountdown = 6;
                    } );
        }
        if (effects.rechargeCountdown == 0)
        {
            // Try with casting Recharge.
            tryWithSpell(rechargeManaCost, [](auto& playerStats [[maybe_unused]], auto& bossStats [[maybe_unused]], auto& effects)
                    {
                    effects.rechargeCountdown = 5;
                    } );
        }

        if (!hasCastASpell)
            assert(result == NoWin);

        assert(result == NoWin || result >= 0);
        minManaToWinLookup[state] = result;
        return result;
    }
    else
    {
        assert(bossStats.hitpoints > 0);
        assert(playerStats.hitpoints > 0);
        const int damageDealt = std::max(1, bossStats.damage - playerArmor);
        Stats playerStatsNext = playerStats;
        playerStatsNext.hitpoints -= damageDealt;
        if (playerStatsNext.hitpoints <= 0)
        {
            std::cout << " played died" << std::endl;
            return NoWin;
        }
        return calcMinManaToWin(true, playerStatsNext, bossStats, effects);

    }
}

int main()
{
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

    Stats playerStats = {50, -1, 500};
    Stats bossStats = {initialBossHitPoints, bossDamage, -1};
    Effects effects;
    const auto result = calcMinManaToWin(true, playerStats, bossStats, effects);
    std::cout << "result: " << result << std::endl;


    return 0;
}
