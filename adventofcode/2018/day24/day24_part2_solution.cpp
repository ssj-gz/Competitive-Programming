#pragma GCC optimize "trapv" // abort() on (signed) integer overflow.
#include <iostream>
#include <regex>
#include <ranges>
#include <sstream>

#include <cassert>

using namespace std;

struct Group
{
    enum Type { Immune, Infection };
    int id = -1;
    Type type;
    int unitCount = -1;
    int hitPoints = -1;
    int damageCount = -1;
    string damageType;
    int initiative = -1;
    vector<string> weaknesses;
    vector<string> immunities;
    bool isAlive =  true;
    int64_t effectivePower() const
    {
        return static_cast<int64_t>(unitCount) * damageCount;
    }
    int64_t effectiveDamageFrom(const auto* attacker) const
    {
        int64_t effectiveDamage = attacker->effectivePower();
        if (std::find(weaknesses.begin(), weaknesses.end(), attacker->damageType) != weaknesses.end())
        {
            effectiveDamage *= 2;
        }
        else if (std::find(immunities.begin(), immunities.end(), attacker->damageType) != immunities.end())
        {
            effectiveDamage = 0;
        }
        return effectiveDamage;
    }
};

bool doReindeerWin(const vector<Group>& groupsOrig)
{
    vector<Group> groups = groupsOrig;
    vector<Group*> remainingGroups;
    for (auto& group : groups)
        remainingGroups.push_back(&group);

    struct TargetResult
    {
        Group *attacker = nullptr;
        Group *defender = nullptr;
    };
    std::cout << "Beginning combat" << std::endl;
    while (true)
    {
        std::cout << "New round; #remainingGroups: " << remainingGroups.size() << std::endl;
        // Target selection.
        std::cout << "Target selection" << std::endl;
        vector<TargetResult> targetResults;
        sort(remainingGroups.begin(), remainingGroups.end(), [](const auto* lhsGroup, const auto* rhsGroup)
                {
                const int64_t lhsEffectivePower = lhsGroup->effectivePower();
                const int64_t rhsEffectivePower = rhsGroup->effectivePower();
                if (lhsEffectivePower != rhsEffectivePower)
                    return rhsEffectivePower < lhsEffectivePower;
                return rhsGroup->initiative < lhsGroup->initiative;
                });
        vector<Group*> targets = remainingGroups;
        for (auto* attacker : remainingGroups)
        {
            assert(attacker->isAlive);
            vector<Group*> targetableEnemies;
            for (auto* group : targets)
            {
                if (group->type != attacker->type)
                    targetableEnemies.push_back(group);
            }

            sort(targetableEnemies.begin(), targetableEnemies.end(), [attacker](const auto* lhsGroup, const auto* rhsGroup)
                    {
                    const int64_t effectiveDamageLhs = lhsGroup->effectiveDamageFrom(attacker);
                    const int64_t effectiveDamageRhs = rhsGroup->effectiveDamageFrom(attacker);
                    if (effectiveDamageLhs != effectiveDamageRhs)
                        return effectiveDamageRhs < effectiveDamageLhs;
                    if (lhsGroup->effectivePower() != rhsGroup->effectivePower())
                        return rhsGroup->effectivePower() < lhsGroup->effectivePower();
                    return rhsGroup->initiative < lhsGroup->initiative;
                    });
            if (!targetableEnemies.empty())
            {
                for (auto* target : targetableEnemies)
                {
                    assert(target->isAlive);
                    if (target->effectiveDamageFrom(attacker) == 0)
                    {
                        // "If it cannot deal any defending groups damage, it does not choose a target".
                        // Ambiguous as to whether we include the defenders hitpoints, here(?)
                        continue;
                    }
                    targetResults.push_back({attacker, target});
                    targets.erase(std::find(targets.begin(), targets.end(), target));
                    break;
                }
            }
        }

        std::cout << "Attack" << std::endl;
        std::sort(targetResults.begin(), targetResults.end(), [](const auto& lhsTargetPair, const auto& rhsTargetPair)
                {
                return rhsTargetPair.attacker->initiative < lhsTargetPair.attacker->initiative;
                });
        int64_t totalUnitsLost = 0;
        for (auto& [attacker, target] : targetResults)
        {
            if (!attacker->isAlive || !target->isAlive)
                continue;
            const int64_t effectiveDamageToTarget = target->effectiveDamageFrom(attacker);
            const int64_t unitsLost = std::min<int64_t>(effectiveDamageToTarget / target->hitPoints, target->unitCount);
            std::cout << "Attacker: " << (attacker->type == Group::Immune ? "Immune" : "Infection") << " " << attacker->id << " attacks: " << (target->type == Group::Immune ? "Immune" : "Infection") << " " << target->id << " dealing " << effectiveDamageToTarget << " damage, killing " << std::min<int64_t>(unitsLost, target->unitCount) << " units" << std::endl;
            target->unitCount -= unitsLost;
            totalUnitsLost += unitsLost;
            if (target->unitCount <= 0)
                target->isAlive = false;
        }
        // Who is left alive?
        remainingGroups.erase(std::remove_if(remainingGroups.begin(), remainingGroups.end(), [](const auto* group) { return !group->isAlive;}), remainingGroups.end());
        const int numInfectionsAlive = std::count_if(remainingGroups.begin(), remainingGroups.end(), [](const auto* group) { return group->type == Group::Infection;});
        const int numImmuneAlive = std::count_if(remainingGroups.begin(), remainingGroups.end(), [](const auto* group) { return group->type == Group::Immune;});
        std::cout << "numImmuneAlive: " << numImmuneAlive << " numInfectionsAlive: " << numInfectionsAlive << std::endl;
        if (numImmuneAlive == 0)
        {
            std::cout << " all reindeer died :(" << std::endl;
            return false;
        }
        if (numInfectionsAlive == 0)
        {
            std::cout << " infection destroyed!" << std::endl;
            int64_t numImmuneUnits = 0;
            for (auto* immuneGroup : remainingGroups)
            {
                assert(immuneGroup->type == Group::Immune);
                numImmuneUnits += immuneGroup->unitCount;
            }
            std::cout << "result: " << numImmuneUnits << std::endl;
            return true;
        }
        if (totalUnitsLost == 0)
        {
            std::cout << " stalemate" << std::endl;
            // "Stalemates" are not "wins".
            return false;
        }
    }
}

ostream& operator<<(ostream& os, const Group& group)
{
    os << group.unitCount << " units each with " << group.hitPoints << " hit points "; 
    if (!(group.weaknesses.empty() && group.immunities.empty()))
    {
        os << "(";
        if (!group.weaknesses.empty())
        {
            os << "weak to";
            for (auto weaknessIter = group.weaknesses.begin(); weaknessIter != group.weaknesses.end(); weaknessIter++)
            {
               os << " " << *weaknessIter; 
               if (std::next(weaknessIter) != group.weaknesses.end())
                   os << ",";
            }
        }
        if (!group.immunities.empty())
        {
            if(!group.weaknesses.empty()) os << "; ";
            os << "immune to";
            for (auto immunityIter = group.immunities.begin(); immunityIter != group.immunities.end(); immunityIter++)
            {
                os << " " << *immunityIter; 
                if (std::next(immunityIter) != group.immunities.end())
                    os << ",";
            }

        }
        os << ") ";
    }
    os << "with an attack that does " << group.damageCount << " " << group.damageType << " damage at initiative " << group.initiative;
    return os;
}

int main()
{
    string immuneSystemHeader;
    getline(cin, immuneSystemHeader);
    assert(immuneSystemHeader == "Immune System:");
    string groupDescrLine;
    std::regex groupDescrRegex(R"(^(\d+) units each with (\d+) hit points (\((.*)\) )?with an attack that does (\d+) (\w+) damage at initiative (\d+)$)");
    vector<Group> groups;

    auto parseGroup = [&groupDescrRegex](Group::Type type, int id, const std::string& groupDescrLine)
    {
        std::smatch groupDescrMatch;
        const bool matchSuccesful = std::regex_match(groupDescrLine, groupDescrMatch, groupDescrRegex);
        assert(matchSuccesful);
        const std::string weaknessesDescr = groupDescrMatch[4];
        Group group;
        group.type = type;
        group.id = id;
        group.unitCount = std::stoi(groupDescrMatch[1]);
        group.hitPoints = std::stoi(groupDescrMatch[2]);
        group.damageCount = std::stoi(groupDescrMatch[5]);
        group.damageType = groupDescrMatch[6];
        group.initiative = std::stoi(groupDescrMatch[7]);
        // Trimming and splitting a string is still clunky as hell, here in YToOL 2024.
        auto weaknessesDescrRange = std::views::split(std::string(weaknessesDescr), ';');
        for (const auto& weaknessRange : weaknessesDescrRange)
        {
            string weaknessDescr(weaknessRange.begin(), weaknessRange.end());
            //std::cout << " weaknessDescr: " << weaknessDescr << std::endl;
            std::smatch weaknessMatch;
            const bool matchSuccessful = std::regex_match(weaknessDescr, weaknessMatch, std::regex(R"(^\s*(weak|immune) to (.*)$)"));
            assert(matchSuccessful);
            const std::string weakOrImmune = weaknessMatch[1];
            std::string attackTypesDescr = weaknessMatch[2];
            attackTypesDescr.erase(std::remove(attackTypesDescr.begin(), attackTypesDescr.end(), ','), attackTypesDescr.end());
            auto& attackList = (weakOrImmune == "weak") ? group.weaknesses : group.immunities;
            auto attackTypesStream = istringstream(attackTypesDescr);
            string attackType;
            while (attackTypesStream >> attackType)
                attackList.push_back(attackType);
        }   
        return group;
    };
    int id = 1;
    while (getline(cin, groupDescrLine) && !groupDescrLine.empty())
    {
        std::cout << "groupDescrLine: " << groupDescrLine << std::endl;
        const auto group = parseGroup(Group::Immune, id, groupDescrLine);
        std::cout << "read group    : " << group << std::endl;
        groups.push_back(group);
        id++;
    }

    string infectionSystemHeader;
    getline(cin, infectionSystemHeader);
    assert(infectionSystemHeader == "Infection:");
    id = 1;
    while (getline(cin, groupDescrLine))
    {
        std::cout << "groupDescrLine: " << groupDescrLine << std::endl;
        const auto group = parseGroup(Group::Infection, id, groupDescrLine);
        std::cout << "read group    : " << group << std::endl;
        groups.push_back(group);
        id++;
    }

    int boost = 0;
    while (true)
    {
        if (!doReindeerWin(groups))
        {
            boost++;
            std::cout << "Trying with boost: " << boost << std::endl;
            for (auto& immuneGroup : groups)
            {
                if (immuneGroup.type == Group::Immune)
                    immuneGroup.damageCount++;
            }
        }
        else
            break;
    }
    std::cout << "Boost: " << boost << std::endl;

    return 0;
}
