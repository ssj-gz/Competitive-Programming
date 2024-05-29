#include <iostream>
#include <map>
#include <vector>
#include <deque>
#include <cassert>

class IntCodeComputer
{
    public:
        IntCodeComputer(const std::vector<int64_t>& originalProgram)
        {
            for (int i = 0; i < static_cast<int>(originalProgram.size()); i++)
            {
                m_program[i] = originalProgram[i];
            }
        }
        enum ExitStatus { Terminated, WaitingForInput};
        ExitStatus run()
        {
            if (m_isTerminated)
                return Terminated;
            while (true)
            {
                const auto instruction = m_program[m_position++];
                const auto opCode = instruction % 100;
                const auto parameterModes = instruction / 100;

                auto nextParamValue = [this, firstParamPosition = m_position, parameterModes]() -> int64_t& 
                {
                    const auto paramIndex = m_position - firstParamPosition;
                    auto parameterMode = parameterModes;
                    for (auto i = 0; i < paramIndex; i++)
                        parameterMode /= 10;
                    parameterMode %= 10;

                    if (parameterMode == 0)
                    {
                        return m_program[m_program[m_position++]];
                    }
                    else if (parameterMode == 1)
                    {
                        return m_program[m_position++];
                    }
                    else if (parameterMode == 2)
                    {
                        return m_program[m_relativeBase + m_program[m_position++]];
                    }
                    else
                    {
                        assert(false);
                    }

                };

                switch(opCode)
                {
                    case 1:
                        {
                            const auto sumand1 = nextParamValue();
                            const auto sumand2 = nextParamValue();
                            auto& dest = nextParamValue();
                            dest = sumand1 + sumand2;
                            break;
                        }
                    case 2:
                        {
                            const auto multiplicand1 = nextParamValue();
                            const auto multiplicand2 = nextParamValue();
                            auto& dest = nextParamValue();
                            dest = multiplicand1 * multiplicand2;
                            break;
                        }
                    case 3:
                        {
                            if (m_inputs.empty())
                            {
#ifndef INTCODE_SILENT
                                std::cout << " no input: exiting with WaitingForInput" << std::endl;
#endif
                                m_position--;
                                return WaitingForInput;
                            }
                            const auto input = m_inputs.front();
                            m_inputs.pop_front();
                            auto& dest = nextParamValue();
                            dest = input;
                            break;
                        }
                    case 4:
                        {
                            const auto output = nextParamValue();
                            m_output.push_back(output);
                            break;
                        }
                    case 5:
                        {
                            const auto toCheck = nextParamValue();
                            const auto positionIfNonZero = nextParamValue();
                            if (toCheck != 0)
                                m_position = positionIfNonZero;
                            break;
                        }
                    case 6:
                        {
                            const auto toCheck = nextParamValue();
                            const auto positionIfZero = nextParamValue();
                            if (toCheck == 0)
                                m_position = positionIfZero;
                            break;
                        }
                    case 7:
                        {
                            const auto param1 = nextParamValue();
                            const auto param2 = nextParamValue();
                            auto& dest = nextParamValue();
                            if (param1  < param2)
                               dest = 1;
                            else
                               dest = 0;
                            break;
                        }
                    case 8:
                        {
                            const auto param1 = nextParamValue();
                            const auto param2 = nextParamValue();
                            auto& dest= nextParamValue();
                            if (param1 == param2)
                                dest = 1;
                            else
                                dest = 0;
                            break;
                        }
                    case 9:
                        {
                            const auto adjustment = nextParamValue();
                            m_relativeBase += adjustment;
                            break;
                        }
                    case 99:
#ifndef INTCODE_SILENT
                        std::cout << "Terminate!" << std::endl;
#endif
                        m_isTerminated = true;
                        return Terminated;
                    default:
                        std::cout << "Unknown opCode: " << opCode << std::endl;
                        assert(false);
                }
            }
            assert(false && "Unreachable");
            return Terminated;
        }
        void addInputs(const std::vector<int64_t>& input)
        {
            m_inputs.insert(m_inputs.end(), input.begin(), input.end());
        }
        void addInput(int64_t input)
        {
            m_inputs.push_back(input);
        }
        std::vector<int64_t> takeOutput()
        {
            const auto output = m_output;
            m_output.clear();
            return output;
        }
        std::vector<int64_t> output() const
        {
            return m_output;
        }
        void clearOutput()
        {
            m_output.clear();
        }
        bool isTerminated() const
        {
            return m_isTerminated;
        }
    private:
        bool m_isTerminated = false;
        std::map<int64_t, int64_t> m_program;
        int64_t m_position = 0;
        int64_t m_relativeBase = 0;
        std::deque<int64_t> m_inputs;
        std::vector<int64_t> m_output;
};
