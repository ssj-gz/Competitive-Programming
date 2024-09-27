#include <iostream>
#include <vector>
#include <ranges>
#include <regex>
#include <map>

#include <cassert>

using namespace std;

int main()
{
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
            if (conditionType == None)
                return destWorkflowName;
            else
            {
                const auto categoryValue = part.valueOfCategory.find(compareCategoryChar)->second;
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
    return 0;
}
