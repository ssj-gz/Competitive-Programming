#include <iostream>
#include <vector>
#include <ranges>
#include <regex>
#include <map>
#include <set>

#include <cassert>

using namespace std;

struct Part
{
    map<char, int64_t> valueOfCategory;
};
struct Rule
{
    enum ConditionType { None, LessThan, GreaterThan };
    ConditionType conditionType = None;
    char compareCategoryChar = '\0';
    int64_t compareValue = -1;
    string destWorkflowName;

    string destWorkFlowNameForPart(const Part& part) const
    {
        //std::cout << "destWorkFlowNameForPart" << std::endl;
        if (conditionType == None)
        {
            //std::cout << " no condition - passes" << std::endl;
            return destWorkflowName;
        }
        else
        {
            const auto categoryValue = part.valueOfCategory.find(compareCategoryChar)->second;
            //std::cout << " categoryValue: " << categoryValue << " conditionType:" << (conditionType == LessThan ? "LessThan" : "GreaterThan") << std::endl;
            if (conditionType == LessThan)
            {
                if (categoryValue < compareValue)
                    return destWorkflowName;
            }
            else if (conditionType == GreaterThan)
            {
                if (categoryValue > compareValue)
                    return destWorkflowName;
            }
            else
            {
                assert(false);
            }
        }
        return "";
    }
};

ostream& operator<<(ostream& os, const Rule& rule)
{
    if (rule.conditionType == Rule::None)
    {
        os << "[Rule: None]"; 
    }
    else
    {
        os << "[Rule: " << rule.compareCategoryChar << (rule.conditionType == Rule::LessThan ? "<" : ">") << " " << rule.compareValue << "]";
    }

    return os;
}
struct Range;
ostream& operator<<(ostream& os, const Range& range);
struct Range
{
    int64_t begin = 0;
    int64_t end = -1;
    bool intersects(const Range& other) const
    {
        if (other.end < begin || other.begin > end)
            return false;
        return true;
    }
    Range refinedToPassRule(const Rule& rule) const
    {
        std::cout << "refinedToPassRule: " << rule << " original begin: " << begin << " original end: " << end << std::endl;
        auto compute = [&]()
        {
            Range result = *this;
            if (rule.conditionType == Rule::None)
                return result;
            else if (rule.conditionType == Rule::LessThan)
            {
                if (rule.compareValue >= end)
                    return result;
                else if (rule.compareValue <= begin)
                    return Range({0, -1});
                else 
                {
                    result.end = rule.compareValue - 1;
                    return result;
                }
            }
            else if (rule.conditionType == Rule::GreaterThan)
            {
                if (rule.compareValue <= begin)
                    return result;
                else if (rule.compareValue >= end)
                    return Range({0, -1});
                else
                {
                    result.begin = rule.compareValue + 1;
                    return result;
                }
            }
            else
            {
                assert(false);
            }
        };
        auto result = compute();
        std::cout << " refined result: " << result << std::endl;
        {
            Part dummyPart;
            for (int i = begin; i < result.begin; i++)
            {
                dummyPart.valueOfCategory[rule.compareCategoryChar] = i;
                assert(rule.destWorkFlowNameForPart(dummyPart) == "");
            }
            for (int i = result.begin; i <= result.end; i++)
            {
                dummyPart.valueOfCategory[rule.compareCategoryChar] = i;
                assert(rule.destWorkFlowNameForPart(dummyPart) != "");
            }
            for (int i = result.end + 1; i <= end; i++)
            {
                dummyPart.valueOfCategory[rule.compareCategoryChar] = i;
                assert(rule.destWorkFlowNameForPart(dummyPart) == "");
            }
        }
        return result;
    }
    Range refinedToFailRule(const Rule& rule) const
    {
        auto compute = [&]()
        {
            Range result = *this;
            if (rule.conditionType == Rule::None)
                return Range({0, -1});
            else if (rule.conditionType == Rule::LessThan)
            {
                if (begin >= rule.compareValue)
                    return result;
                else if (end < Rule::LessThan)
                    return Range({0, -1});
                else 
                {
                    result.begin = rule.compareValue;
                    return result;
                }
            }
            else if (rule.conditionType == Rule::GreaterThan)
            {
                if (end <= Rule::GreaterThan)
                    return result;
                else if (begin > rule.compareValue)
                    return Range({0, -1});
                else
                {
                    result.end = rule.compareValue;
                    return result;
                }
            }
            else
            {
                assert(false);
            }
        };
        auto result = compute();
        {
            Part dummyPart;
            for (int i = begin; i < result.begin; i++)
            {
                dummyPart.valueOfCategory[rule.compareCategoryChar] = i;
                assert(rule.destWorkFlowNameForPart(dummyPart) != "");
            }
            for (int i = result.begin; i <= result.end; i++)
            {
                dummyPart.valueOfCategory[rule.compareCategoryChar] = i;
                assert(rule.destWorkFlowNameForPart(dummyPart) == "");
            }
            for (int i = result.end + 1; i <= end; i++)
            {
                dummyPart.valueOfCategory[rule.compareCategoryChar] = i;
                assert(rule.destWorkFlowNameForPart(dummyPart) != "");
            }
        }
        return result;
    }

    int64_t size() const
    {
        if (end >= begin)
            return end - begin + 1;
        return 0;
    }

    auto operator<=>(const Range& other) const = default;
};
ostream& operator<<(ostream& os, const Range& range)
{
    os << "[Range: " << range.begin << "-" << range.end << "]";
    return os;
}

struct CategoryRanges
{
    map<char, Range> rangeForCategory;
    int64_t size() const
    {
        //std::cout << "CategoryRanges::size" << std::endl;
        int64_t result = 1;
        for (const auto& [categoryChar, categoryRange] : rangeForCategory)
        {
            //std::cout << " size of " << categoryChar << " : " << categoryRange.size() << std::endl;
            result *= categoryRange.size();
        }
        return result;
    }
    auto operator<=>(const CategoryRanges& other) const = default;
};
struct Workflow
{
    string name;
    vector<Rule> rules;

    string destWorkFlowNameForPart(const Part& part) const
    {
        for (const auto& rule : rules)
        {
            const auto destWorkflowNameFromRule = rule.destWorkFlowNameForPart(part);
            if (!destWorkflowNameFromRule.empty())
                return destWorkflowNameFromRule;
        }
        assert(false);
        return "";
    }
};

void findAcceptedCategoryRanges(const CategoryRanges& currentCategoryRanges, const Workflow& currentWorkflow, map<string, Workflow>& workflowForName, set<string>& visitedWorkflowNames, vector<CategoryRanges>& acceptedCategoryRanges)
{
    std::cout << "Thingy - currentWorkflow: " << currentWorkflow.name << " currentCategoryRanges.size: " << currentCategoryRanges.size() << std::endl;
    assert(currentWorkflow.name != "");
    if (currentCategoryRanges.size() == 0)
        return;
    if (currentWorkflow.name == "A")
    {
        std::cout << "Whoo!" << currentCategoryRanges.size() << std::endl;
        acceptedCategoryRanges.push_back(currentCategoryRanges);
        return;
    }
    if (currentWorkflow.name == "R")
    {
        std::cout << "Boo!" << std::endl;
        return;
    }
    if (visitedWorkflowNames.contains(currentWorkflow.name))
        return;

    visitedWorkflowNames.insert(currentWorkflow.name);

    CategoryRanges categoryRanges = currentCategoryRanges;
    for (const auto& rule : currentWorkflow.rules)
    {
        if (categoryRanges.size() == 0)
            break;
        // Attempt to pass this rule.
        {
            CategoryRanges rangesToPassRule = categoryRanges;
            if (rule.conditionType != Rule::None)
            {
                rangesToPassRule.rangeForCategory[rule.compareCategoryChar] = rangesToPassRule.rangeForCategory[rule.compareCategoryChar].refinedToPassRule(rule);
            }
            Workflow& nextWorkflow = workflowForName[rule.destWorkflowName];
            findAcceptedCategoryRanges(rangesToPassRule, nextWorkflow, workflowForName, visitedWorkflowNames, acceptedCategoryRanges);
        }
        // Fail this rule.
        categoryRanges.rangeForCategory[rule.compareCategoryChar] = categoryRanges.rangeForCategory[rule.compareCategoryChar].refinedToFailRule(rule);
    }

    visitedWorkflowNames.erase(currentWorkflow.name);
}

int main()
{
    map<string, Workflow> workflowForName;
    string workflowLine;
    std::regex comparisonRuleRegex(R"(^([xmas])([<>])(\d+):(\w+)$)");
    while (getline(cin, workflowLine) && !workflowLine.empty())
    {
        std::cout << "workflowLine: " << workflowLine << std::endl;
        Workflow workFlow;
        const auto openingBracePos = workflowLine.find('{');
        assert(openingBracePos != string::npos);
        workFlow.name = workflowLine.substr(0, openingBracePos);
        std::cout << "work flow name: " << workFlow.name << std::endl;
        const auto closingBracePos = workflowLine.find('}');
        assert(closingBracePos == workflowLine.size() - 1);
        const auto rulesString = workflowLine.substr(openingBracePos + 1, closingBracePos - openingBracePos - 1);
        cout << "rulesString: " << rulesString << std::endl;
        auto ruleStrings = std::views::split(rulesString, ',');
        for (const auto& ruleStringRange : ruleStrings)
        {
            const std::string ruleString(ruleStringRange.begin(), ruleStringRange.end());
            std::cout << " rule: " << ruleString << std::endl;
            std::smatch comparisonRuleMatch;
            Rule rule;
            if (std::regex_match(ruleString, comparisonRuleMatch, comparisonRuleRegex))
            {
                //const auto categoryChar = std::string(comparisonRuleMatch[1]).front();
                //const auto comparisonChar = std::string(comparisonRuleMatch[2]).front();
                //const int64_t comparisonValue = std::stoll(comparisonRuleMatch[3]);
                //const auto destWorkflowName = comparisonRuleMatch[4];

                rule.compareCategoryChar = std::string(comparisonRuleMatch[1]).front();
                rule.conditionType = (std::string(comparisonRuleMatch[2]).front() == '<' ? Rule::LessThan : Rule::GreaterThan);
                rule.compareValue = std::stoll(comparisonRuleMatch[3]);
                rule.destWorkflowName = comparisonRuleMatch[4];

                std::cout << "  is comparison match: categoryChar: " << rule.compareCategoryChar << " conditionType: " << (rule.conditionType == Rule::LessThan ? "LessThan" : "GreaterThan") << " compareValue: " << rule.compareValue << " destWorkflowName: " << rule.destWorkflowName << std::endl;
            }
            else
            {
                std::cout << "  is not a comparison match" << std::endl;
                rule.destWorkflowName = ruleString;
            }
            workFlow.rules.push_back(rule);
        }
        workflowForName[workFlow.name] = workFlow;
    }

    string partLine;
    std::regex categoryValuesRegex(R"(^\{(x)=(\d+),(m)=(\d+),(a)=(\d+),(s)=(\d+)\}$)");
    vector<Part> parts;
    while (getline(cin, partLine))
    {
        Part part;
        std::cout << "partLine: " << partLine << std::endl;
        std::smatch categoryValuesMatch;
        const bool matchSuccesful = (std::regex_match(partLine, categoryValuesMatch, categoryValuesRegex));
        assert(matchSuccesful);
        for (int categoryIndex = 0; categoryIndex < 4; categoryIndex++)
        {
            const char categoryChar = std::string(categoryValuesMatch[categoryIndex * 2 + 1]).front();
            const int64_t value = std::stoll(categoryValuesMatch[categoryIndex * 2 + 2]);
            //std::cout << " categoryChar: " << categoryChar << " value: " << categoryValue << std::endl;
            part.valueOfCategory[categoryChar] = value;
        }
        parts.push_back(part);
    }

#if 0
    int64_t totalAcceptedPartsRatings = 0;
    for (const auto& part : parts)
    {
        string workflowName = "in";
        while (workflowName != "A" && workflowName != "R")
        {
            workflowName = workflowForName[workflowName].destWorkFlowNameForPart(part);
        }
        if (workflowName == "A")
        {
            int64_t partRatingSum = 0;
            for (const auto& [categoryChar, categoryValue] : part.valueOfCategory)
            {
                partRatingSum += categoryValue;
            }
            std::cout << "part accepted; partRatingSum: " << partRatingSum << std::endl;
            totalAcceptedPartsRatings += partRatingSum;
        }
        else
        {
            std::cout << "part rejected" << std::endl;
        }
    }
    std::cout << "totalAcceptedPartsRatings: " << totalAcceptedPartsRatings << std::endl;
#endif
    // Add dummy, terminal workflows for Accept and Reject;
    workflowForName["A"].name = "A";
    workflowForName["R"].name = "R";
    CategoryRanges initialCategoryRanges;
    for (const auto categoryChar : string("xmas"))
    {
        initialCategoryRanges.rangeForCategory[categoryChar] = { 1, 4000 };
    }

    set<string> visitedWorkflows;
    vector<CategoryRanges> acceptedCategoryRanges;
    findAcceptedCategoryRanges(initialCategoryRanges, workflowForName["in"], workflowForName, visitedWorkflows, acceptedCategoryRanges);
    for (auto& acceptedRanges : acceptedCategoryRanges)
    {
        for (auto& otherAcceptedRanges : acceptedCategoryRanges)
        {
            if (acceptedRanges == otherAcceptedRanges)
                continue;

            bool haveOverlap = true;
            for (const auto categoryChar : string("xmas"))
            {
                if (!acceptedRanges.rangeForCategory[categoryChar].intersects(otherAcceptedRanges.rangeForCategory[categoryChar]))
                {
                    haveOverlap = false;
                    break;
                }

            }
            assert(!haveOverlap && "Overcounting due to overlapping acceptedRanges is not handled!");
        }
    }
    int64_t result = 0;
    for (const auto& acceptedRanges : acceptedCategoryRanges)
    {
        result += acceptedRanges.size();
    }
    std::cout << "result: " << result << std::endl;
    return 0;
}
