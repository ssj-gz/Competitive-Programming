#include <iostream>
#include <map>
#include <regex>

#include <cassert>

using namespace std;

int main()
{
    std::regex instructionRegex(R"(^(value (\d+) goes to bot (\d+))|(bot (\d+) gives low to (bot|output) (\d+) and high to (bot|output) (\d+))$)");
    string instruction;
    struct Bot
    {
        vector<int> chips;
        std::string lowReceiverType;
        int lowReceiverId = -1;
        std::string highReceiverType;
        int highReceiverId = -1;

    };

    map<int, Bot> botForId;
    map<int, vector<int>> outputBinContents;

    while (getline(cin, instruction))
    {
        std::smatch instructionMatch;
        std::cout << "instruction: " << instruction << std::endl;
        const bool matchSuccessful = std::regex_match(instruction, instructionMatch, instructionRegex);
        assert(matchSuccessful);
        if (!std::string(instructionMatch[1]).empty())
        {
            const auto valueToGive = std::stoi(instructionMatch[2]);
            const auto recipientBotId = std::stoi(instructionMatch[3]);
            botForId[recipientBotId].chips.push_back(valueToGive);
            assert(static_cast<int>(botForId[recipientBotId].chips.size()) <= 2);
        }
        else if (!std::string(instructionMatch[4]).empty())
        {
            const auto senderBotId = std::stoi(instructionMatch[5]);
            Bot& senderBot = botForId[senderBotId];
            senderBot.lowReceiverType = instructionMatch[6];
            senderBot.lowReceiverId = std::stoi(instructionMatch[7]);
            senderBot.highReceiverType = instructionMatch[8];
            senderBot.highReceiverId = std::stoi(instructionMatch[9]);
        }
        else
        {
            assert(false);
        }
    }

    bool stop = false;
    while (!stop)
    {
        for (auto& [id, bot] : botForId)
        {
            assert(static_cast<int>(bot.chips.size()) <= 2);
            if (static_cast<int>(bot.chips.size()) == 2)
            {
                std::sort(bot.chips.begin(), bot.chips.end());
                std::cout << "Bot: " << id << " comparing chips:" << bot.chips[0] << " & " << bot.chips[1] << std::endl;
                //if (bot.chips[0] == 2 && bot.chips[1] == 5)
                if (bot.chips[0] == 17 && bot.chips[1] == 61)
                {
                    std::cout << "result: " << id << std::endl;
                    stop = true;
                }
                if (bot.lowReceiverType == "output")
                    outputBinContents[bot.lowReceiverId].push_back(bot.chips[0]);
                else
                    botForId[bot.lowReceiverId].chips.push_back(bot.chips[0]);
                if (bot.highReceiverType == "output")
                    outputBinContents[bot.highReceiverId].push_back(bot.chips[1]);
                else
                    botForId[bot.highReceiverId].chips.push_back(bot.chips[1]);

                bot.chips.clear();
            }
        }
    }
    return 0;
}