#include <iostream>
#include <regex>
#include <map>
#include <ranges>

#include <cassert>

using namespace std;

struct Module;
struct Pulse
{
    enum Type {High, Low};
    Module *sender = nullptr;
    Type type;
    Module *receiver = nullptr;
};

struct Module
{
    enum Type { Broadcast, FlipFlop, Conjunction, Sink, Button };
    Type type;
    string name;
    vector<Module*> targetModules;
    vector<Module*> inputModules;

    bool flipFlopOn = false;
    map<Module*, Pulse::Type> lastPulseTypeFromInput;

    vector<Pulse> pulsesEmittedAfterReceiving(const Pulse& pulseReceived)
    {
        switch (type)
        {
            case Broadcast:
                {
                    vector<Pulse> pulses;
                    for (auto* receiver : targetModules)
                    {
                        pulses.push_back({this, pulseReceived.type, receiver});
                    }
                    return pulses;
                }
            case FlipFlop:
                {
                    if (pulseReceived.type == Pulse::Low)
                    {
                        std::cout << " flipflop module " << name << " received low pulse while state was: " << flipFlopOn << std::endl;
                        vector<Pulse> pulses;
                        for (auto* receiver : targetModules)
                        {
                            pulses.push_back({this, flipFlopOn ? Pulse::Low : Pulse::High, receiver});
                        }
                        flipFlopOn = !flipFlopOn;
                        return pulses;
                    }
                    else 
                    {
                        std::cout << " flipflop module " << name << " received high pulse which it ignored entirely" << std::endl;
                        return vector<Pulse>();
                    }
                }
            case Sink:
                // Do nothing.
                return vector<Pulse>();
            case Conjunction:
                {
                    lastPulseTypeFromInput[pulseReceived.sender] = pulseReceived.type;
                    Pulse::Type pulseTypeToSend = Pulse::Low;
                    for (const auto& [sender, lastSentPulseType] : lastPulseTypeFromInput)
                    {
                        if (lastSentPulseType == Pulse::Low)
                        {
                            pulseTypeToSend = Pulse::High;
                            break;
                        }
                    }
                    vector<Pulse> pulses;
                    for (auto* receiver : targetModules)
                    {
                        pulses.push_back({this, pulseTypeToSend, receiver});
                    }
                    return pulses;
                }
            default:
                assert(false);
        }
    }
};

ostream& operator<<(ostream& os, const Pulse& pulse)
{
    os << "[Pulse: sender: " << pulse.sender->name << " type: " << (pulse.type == Pulse::Low ? "Low" : "Hight") << " receiver: " << pulse.receiver->name << "]";
    return os;
}

int main()
{
    string moduleConfigurationLine;
    std::regex moduleConfigRegex(R"(^([%&]?)(\w+)\s*->(.*)$)");
    map<string, vector<string>> targetModuleNamesForModule;
    map<string, Module> moduleForName;
    while (getline(cin, moduleConfigurationLine))
    {
        std::smatch moduleConfigurationMatch;
        std::cout << "moduleConfigurationLine: " << moduleConfigurationLine << std::endl;
        const bool matchSuccessful = std::regex_match(moduleConfigurationLine, moduleConfigurationMatch, moduleConfigRegex);
        assert(matchSuccessful);

        Module module;
        module.name = moduleConfigurationMatch[2];
        const string moduleTypeString = moduleConfigurationMatch[1];

        if (module.name == "broadcaster")
        {
            module.type = Module::Broadcast;
            assert(moduleTypeString.empty());
        }
        else
        {
            assert(!moduleTypeString.empty());
            std::cout << "module: " << module.name << " type string: " << moduleTypeString << std::endl;
            module.type = (moduleTypeString == "&" ? Module::Conjunction : Module::FlipFlop);
        }
        moduleForName[module.name] = module;
        // Trimming and splitting a string is still clunky as hell, here in YToOL 2024.
        auto splitTargetModuleNamesRange = std::views::split(std::string(moduleConfigurationMatch[3]), ',');
        for (const auto& moduleNameRange : splitTargetModuleNamesRange)
        {
            string moduleNameTrimmed(moduleNameRange.begin(), moduleNameRange.end());
            std::erase(moduleNameTrimmed, ' ');
            targetModuleNamesForModule[module.name].push_back(moduleNameTrimmed);
        }   

        std::cout << "Module: " << module.name << " has targets: " << std::endl;
        for (const auto& moduleName : targetModuleNamesForModule[module.name])
        {
            std::cout << " >" << moduleName << "<" << std::endl;
        }
    }

    for (const auto& [moduleName, targetModuleNames] : targetModuleNamesForModule)
    {
        Module* module = &(moduleForName[moduleName]);
        for (const auto& targetModuleName : targetModuleNames)
        {
            if (!moduleForName.contains(targetModuleName))
            {
                // The targetModuleName refers to a Module that is not listed in the puzzle input;
                // create a "sink" one.
                Module sinkModule;
                sinkModule.type = Module::Sink;
                sinkModule.name = targetModuleName;
                moduleForName[targetModuleName] = sinkModule;
            }
            Module *targetModule = &(moduleForName[targetModuleName]);
            module->targetModules.push_back(targetModule);
            if (targetModule->type == Module::Conjunction)
            {
                targetModule->inputModules.push_back(module);
                targetModule->lastPulseTypeFromInput[module] = Pulse::Low; 
            }

        }
    }

    // Button.
    moduleForName["button"] = {Module::Button, "button", { &(moduleForName["broadcaster"]) } };

    int64_t highPulsesSent = 0;
    int64_t lowPulsesSent = 0;
    for (int buttonPresses = 1; buttonPresses <= 1'000; buttonPresses++)
    {
        deque<Pulse> unhandledPulses = { { &(moduleForName["button"]), Pulse::Low, &(moduleForName["broadcaster"]) } };
        while (!unhandledPulses.empty())
        {
            std::cout << "# unhandled pulses: " << unhandledPulses.size() << std::endl;
            const auto pulse = unhandledPulses.front();
            (pulse.type == Pulse::Low ? lowPulsesSent : highPulsesSent)++;
            std::cout << "Handling pulse: " << pulse << std::endl;
            unhandledPulses.pop_front();
            const auto newPulses = pulse.receiver->pulsesEmittedAfterReceiving(pulse);
            std::cout << "Generated the following new pulses: " << std::endl;
            for (const auto& newPulse : newPulses)
                std::cout << " " << newPulse << std::endl;
            unhandledPulses.insert(unhandledPulses.end(), newPulses.begin(), newPulses.end());
        }
    }
    std::cout << "high pulses sent: " << highPulsesSent << std::endl;
    std::cout << "low pulses sent: " << lowPulsesSent << std::endl;
    std::cout << "Result: " << (highPulsesSent * lowPulsesSent) << std::endl;

    return 0;
}
