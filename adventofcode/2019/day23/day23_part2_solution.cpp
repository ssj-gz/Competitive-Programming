#define INTCODE_SILENT
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
    int64_t natLastX = -1;
    int64_t natLastY = -1;
    int64_t address0LastY = -1;
    while (!haveFinished)
    {
        // Deliver and clear all pending messages.
        for (const auto& message : pendingMessages)
        {
            if (message.destAddress == 255)
            {
                natLastX = message.X;
                natLastY = message.Y;
                std::cout << "Nat updated with: " << natLastX << ", " << natLastY << std::endl;
            }
            else
            {
                IntCodeComputer& recipientComputer = allComputersByAddress[message.destAddress];
                recipientComputer.addInput(message.X);
                recipientComputer.addInput(message.Y);
            }
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

        const bool neworkIsIdle = pendingMessages.empty();
        if (neworkIsIdle)
        {
            std::cout << "Idle network - sending " << natLastX << ", " << natLastY << " to #0" << std::endl;
            pendingMessages.push_back({0, natLastX, natLastY});
            if (natLastY == address0LastY)
            {
                std::cout << "done: " << address0LastY << std::endl;
                haveFinished = true;
            }
            address0LastY = natLastY;
        }

    }

}

