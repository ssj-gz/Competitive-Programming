#include "../shared/intcodecomputer.h"

#include <iostream>
#include <vector>

using namespace std;

int main()
{
    vector<int64_t> program;
    int64_t programInput;
    while (true)
    {
        cin >> programInput;
        program.push_back(programInput);
        assert(cin);
        char comma;
        cin >> comma;
        if (!cin)
            break;
        assert(comma == ',');
    }

    const int numComputers = 50;
    vector<IntCodeComputer> allComputersByAddress(numComputers, IntCodeComputer(program));
    for (int address = 0; address < numComputers; address++)
    {
        std::cout << "address: " << address;
        IntCodeComputer& intCodeComputer = allComputersByAddress[address];
        intCodeComputer.run();
        intCodeComputer.addInput(address);
        intCodeComputer.run();
        intCodeComputer.addInput(-1); // No messages yet.
        intCodeComputer.run();
    }

    struct Message
    {
        int64_t destAddress = -1;
        int64_t X = -1;
        int64_t Y = -1;
    };

    vector<Message> pendingMessages;

    bool haveFinished = false;
    while (!haveFinished)
    {
        // Deliver and clear all pending messages.
        for (const auto& message : pendingMessages)
        {
            if (message.destAddress == 255)
            {
                std::cout << "Woohoo! " << message.Y << std::endl;
                haveFinished = true;
                break;
            }
            IntCodeComputer& recipientComputer = allComputersByAddress[message.destAddress];
            recipientComputer.addInput(message.X);
            recipientComputer.addInput(message.Y);
        }
        pendingMessages.clear();
        for (int address = 0; address < numComputers; address++)
        {
            IntCodeComputer& intCodeComputer = allComputersByAddress[address];
            intCodeComputer.run();
            intCodeComputer.addInput(-1); // Nothing for you at the moment.

            auto output = intCodeComputer.takeOutput();
            while (!output.empty())
            {
                assert(output.size() >= 3);
                Message newMessage;
                newMessage.destAddress = output[0];
                newMessage.X = output[1];
                newMessage.Y = output[2];
                output.erase(output.begin(), output.begin() + 3);

                pendingMessages.push_back(newMessage);
            }

        }
    }

}

