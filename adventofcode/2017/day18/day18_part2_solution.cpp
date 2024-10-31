#pragma GCC optimize "trapv" // abort() on (signed) integer overflow.
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <limits>

#include <cassert>

using namespace std;

struct Instruction
{
    enum Type { Send, Set, Add, Mul, Mod, Receive, Jump, Unknown };
    Type type = Unknown;
    char argReg[2] = { '\0', '\0'};
    int64_t argVal[2] = { std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::min()};
    int64_t getArgValue(int argNum, const map<char, int64_t>& registerValue) const
    {
        if (argReg[argNum] != '\0')
        {
            const auto regAndValIter = registerValue.find(argReg[argNum]);
            if (regAndValIter == registerValue.end())
                return 0;
            else
            {
                return regAndValIter->second;
            }
        }
        else
        {
            assert(argVal[argNum] != std::numeric_limits<int64_t>::min());
            return argVal[argNum];
        }
    };
    char getArgRegister(int argNum) const
    {
        assert(argReg[argNum] >= 'a' && argReg[argNum] <= 'z');
        return argReg[argNum];
    }
};

class Program
{
    public:
        Program(int id, const vector<Instruction>& instructions)
            : m_id{id},
              m_instructions{instructions}
        {
            m_registerValue['p'] = id;
        }
        void setOtherProgram(Program* otherProgram)
        {
            m_otherProgram = otherProgram;
        }
        bool execUntilBlocked()
        {
            std::cout << "Program: " << m_id << " execUntilBlocked" << std::endl; 
            bool completedAnyInstructions = false;
            while (true)
            {
                if(m_instructionPointer < 0 || m_instructionPointer >= static_cast<int64_t>(m_instructions.size()))
                {
                    return completedAnyInstructions;
                }
                const auto& instruction = m_instructions[m_instructionPointer];
                std::cout << " program: " << m_id << " executing instruction at " << m_instructionPointer << std::endl;
                switch (instruction.type)
                {
                    case Instruction::Send:
                        m_otherProgram->addIncomingMessage(instruction.getArgValue(0, m_registerValue));
                        std::cout << " program: " << m_id << " sending message " << instruction.getArgValue(0, m_registerValue) << std::endl;
                        m_numMessagesSent++;
                        break;
                    case Instruction::Set:
                        m_registerValue[instruction.getArgRegister(0)] = instruction.getArgValue(1, m_registerValue);
                        break;
                    case Instruction::Add:
                        m_registerValue[instruction.getArgRegister(0)] += instruction.getArgValue(1, m_registerValue);
                        break;
                    case Instruction::Mul:
                        m_registerValue[instruction.getArgRegister(0)] *= instruction.getArgValue(1, m_registerValue);
                        break;
                    case Instruction::Mod:
                        {
                            const int64_t X = instruction.getArgValue(0, m_registerValue);
                            const int64_t Y = instruction.getArgValue(1, m_registerValue);
                            assert(X >= 0);
                            assert(Y > 0);
                            m_registerValue[instruction.getArgRegister(0)] = X % Y;
                        }
                        break;
                    case Instruction::Receive:
                        if (m_incomingMessages.empty())
                        {
                            std::cout << " program: " << m_id << " blocking trying to read message" <<  std::endl;
                            return completedAnyInstructions;
                        }
                        else
                        {
                            std::cout << " program: " << m_id << " received message " << m_incomingMessages.front() << " into register: " << instruction.getArgRegister(0) << std::endl;
                            m_registerValue[instruction.getArgRegister(0)] = m_incomingMessages.front();
                            m_incomingMessages.erase(m_incomingMessages.begin());
                        }
                        break;
                    case Instruction::Jump:
                        if (instruction.getArgValue(0, m_registerValue) > 0)
                        {
                            m_instructionPointer += instruction.getArgValue(1, m_registerValue);
                            assert(m_instructionPointer >= 0 && m_instructionPointer < static_cast<int64_t>(m_instructions.size())); 
                            m_instructionPointer--; // Counteract the increment that always happens at the end of the loop.
                        }
                        break;
                    default:
                        break;
                }
                m_instructionPointer++;
                completedAnyInstructions = true;
#if 1
                std::cout << " program: " << m_id << " new values of registers: " << std::endl;
                for (const auto& [regName, value] : m_registerValue)
                {
                    std::cout << "  " << regName << ": " << value << std::endl; 
                }
#endif
            }
        }
        int64_t numMessagesSent() const
        {
            return m_numMessagesSent;
        }
        void addIncomingMessage(int64_t message)
        {
            //std::cout << "program: " << m_id << " addIncomingMessage: " << message << std::endl;
            m_incomingMessages.push_back(message);
        }
    private:
        int m_id = -1;
        vector<Instruction> m_instructions;
        Program* m_otherProgram = nullptr;
        int64_t m_instructionPointer = 0;
        map<char, int64_t> m_registerValue;
        vector<int64_t> m_incomingMessages;
        int64_t m_numMessagesSent = 0;

};

int main()
{
    vector<Instruction> instructions;
    string instructionLine;
    while (getline(cin, instructionLine))
    {
        std::cout << "instructionLine: " << instructionLine << std::endl;
        Instruction instruction;
        istringstream instructionStream(instructionLine);
        string instructionType;
        instructionStream >> instructionType;
        auto readRegOrValArg = [&instructionStream, &instruction](int argNum)
        {
            char peekChar = '\0';
            instructionStream >> peekChar;
            if (peekChar >= 'a' && peekChar <= 'z')
            {
                //std::cout << "reading *register* arg: " << peekChar << std::endl;
                instruction.argReg[argNum] = peekChar;
            }
            else
            {
                instructionStream.putback(peekChar);
                instructionStream >> instruction.argVal[argNum];
                //std::cout << "reading *integer* arg: " << instruction.argVal[argNum] << std::endl;
            }
        };
        if (instructionType == "snd")
        {
            instruction.type = Instruction::Send;
            readRegOrValArg(0);
        }
        else if (instructionType == "set")
        {
            instruction.type = Instruction::Set;
            instructionStream >> instruction.argReg[0];
            readRegOrValArg(1);
        }
        else if (instructionType == "add")
        {
            instruction.type = Instruction::Add;
            instructionStream >> instruction.argReg[0];
            readRegOrValArg(1);
        }
        else if (instructionType == "mul")
        {
            instruction.type = Instruction::Mul;
            instructionStream >> instruction.argReg[0];
            readRegOrValArg(1);
        }
        else if (instructionType == "mod")
        {
            instruction.type = Instruction::Mod;
            instructionStream >> instruction.argReg[0];
            readRegOrValArg(1);
        }
        else if (instructionType == "rcv")
        {
            instruction.type = Instruction::Receive;
            instructionStream >> instruction.argReg[0];
        }
        else if (instructionType == "jgz")
        {
            instruction.type = Instruction::Jump;
            readRegOrValArg(0);
            readRegOrValArg(1);
        }
        else
        {
            assert(false);
        }
        assert(instructionStream);
        instructions.push_back(instruction);
    }


    Program program0(0, instructions);
    Program program1(1, instructions);
    program0.setOtherProgram(&program1);
    program1.setOtherProgram(&program0);

    int slice = 0;
    int lastSliceWithNoDeadlock = -1;
    while (true)
    {
        const bool program0MadeProgress = program0.execUntilBlocked();
        const bool program1MadeProgress = program1.execUntilBlocked();
        if (program0MadeProgress || program1MadeProgress)
        {
            lastSliceWithNoDeadlock = slice;
        }
        else
        {
            if (slice > lastSliceWithNoDeadlock + 2)
                break;
        }
        slice++;
    }
    std::cout << "Deadlock; # messages sent by program1: " << program1.numMessagesSent() << std::endl;

    return 0;
}
